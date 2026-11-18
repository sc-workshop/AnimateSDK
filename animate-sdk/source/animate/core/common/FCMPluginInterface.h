/******************************************************************************
 * ADOBE CONFIDENTIAL
 * ___________________
 *
 *  Copyright [2013] Adobe Systems Incorporated
 *  All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of Adobe Systems Incorporated and its suppliers,
 * if any.  The intellectual and technical concepts contained
 * herein are proprietary to Adobe Systems Incorporated and its
 * suppliers and are protected by all applicable intellectual
 * property laws, including trade secret and copyright laws.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Adobe Systems Incorporated.
 ******************************************************************************/

/**
 * @file  FCMPluginInterface.h
 *
 * @brief This file contains core definitions of the framework Flash Component Model (FCM).
 */

#pragma once

#include "FCMErrors.h"
#include "FCMMacros.h"
#include "FCMPublicIDs.h"
#include "FCMTypes.h"
#include "animate/app/Application/Service/IApplicationService.h"
#include "animate/app/Application/Service/IOutputConsoleService.h"
#include "animate/core/IFCMCallback.h"
#include "animate/core/IFCMCalloc.h"
#include "animate/core/IFCMClassFactory.h"
#include "animate/core/IFCMDictionary.h"
#include "animate/core/IFCMNoAggregationUnknown.h"
#include "animate/core/IFCMUnknown.h"
#include "animate/module/Types.hpp"
#include "assert.h"
#include "core/exception/exception.h"

#include <array>
#include <atomic>
#include <cstring>
#include <exception>
#include <filesystem>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <thread>

#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))
#include <intrin.h>
#endif

#include "animate/core/common/FCMPreConfig.h"

namespace Animate::Publisher {
    static FCM::Result RegisterPublisher(FCM::PIFCMDictionary, FCM::FCMCLSID, FCM::FCMCLSID, const ModuleInfo&);
}

namespace Animate::DocType {
    static FCM::Result RegisterDocument(FCM::PIFCMDictionary, FCM::ConstFCMCLSID, const ModuleInfo&);
}

namespace FCM {
    namespace Locale {
        std::u16string ToUtf16(const std::string& string);
        std::string ToUtf8(const std::u16string& string);
    }

    /** @cond HIDE_PRIVATE */
    typedef struct {
        FCMCLSID classID;
        FCM::U_Int32 classVersion;
        FCMIID interfaceID;
    } FCMClassInterfaceInfo;

    typedef FCMClassInterfaceInfo* PFCMClassInterfaceInfo;

    inline Result _Delegate(FCM::PVoid pv, FCMIID riid, PPVoid ppvObject, FCM::S_Int64 dw) {
        Result hRes = FCM_NO_INTERFACE;
        IFCMUnknown* p = *((IFCMUnknown**) ((FCM::S_Int64) pv + dw));
        if (p != 0)
            hRes = p->QueryInterface(riid, ppvObject);
        return hRes;
    }

    typedef Result(_FCM_CREATORARGFUNC)(FCM::PVoid pv, FCMIID riid, PPVoid ppvObject, FCM::S_Int64 dw);

    class PluginModule;

    typedef IFCMClassFactory* (*FactorCreatorProc)(PluginModule*);

    typedef struct {
        FCMIID iid;
        FCM::S_Int64 offset;
        _FCM_CREATORARGFUNC* innerObjectFunc;
    } FCMInterfaceMap;

    typedef FCMInterfaceMap* (*InterfaceMapGetProc)();

    typedef struct {
        FCMCLSID clsid;
        FactorCreatorProc pFactoryCreator;
        InterfaceMapGetProc pGetInterfaceTable;
        FCM::U_Int32 classVersion;
    } FLCMClassMap;

    /** @endcond */

    /**
     * @brief Recursive spin lock used as the host lifetime mutex.
     *
     * Shared by host object lifetime operations and host calls. `AddRef`/`Release` are not
     * passive: releasing the last reference of a host object runs host destruction code that
     * mutates document-global state, exactly like a DOM call does. If those run concurrently
     * with a DOM call from another thread the document heap is corrupted (observed as an
     * abort inside `FCMCalloc::Free`), so both kinds of access take this one mutex.
     *
     * The host mutex is taken ~16 million times per publish, and it is only ever held for one
     * host call (a few hundred nanoseconds). At that frequency a kernel-handoff mutex costs
     * more than the critical section itself: measured on the 103-root document, 30 s of the
     * 4-worker prepare was spent *waiting* for this lock versus 11 s holding it.
     *
     * Holders never block on another worker's symbol (PrepareSymbol enforces that), so a
     * waiter always makes progress; spinning is therefore safe. The spin backs off to
     * `yield()` so a holder that blocks inside the host cannot burn a core indefinitely.
     */
    class HostRecursiveSpinLock {
    public:
        void lock() {
            const uint32_t me = ThreadTag();
            if (m_owner.load(std::memory_order_relaxed) == me) {
                ++m_depth;
                return;
            }

            for (int spins = 0;; ++spins) {
                uint32_t expected = 0;
                if (m_owner.compare_exchange_weak(expected, me, std::memory_order_acquire,
                                                  std::memory_order_relaxed)) {
                    m_depth = 1;
                    return;
                }

                if (spins < 1024)
                    CpuPause();
                else
                    std::this_thread::yield();
            }
        }

        void unlock() {
            if (--m_depth == 0)
                m_owner.store(0, std::memory_order_release);
        }

    private:
        static void CpuPause() {
#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))
            _mm_pause();
#endif
        }

        static uint32_t ThreadTag() {
            static thread_local uint32_t tag = NextThreadTag();
            return tag;
        }

        static uint32_t NextThreadTag() {
            static std::atomic<uint32_t> counter{0};
            return counter.fetch_add(1, std::memory_order_relaxed) + 1;
        }

        std::atomic<uint32_t> m_owner{0};
        // Only the owning thread touches the depth, so it needs no atomic.
        uint32_t m_depth = 0;
    };

    inline HostRecursiveSpinLock& HostLifetimeMutex() {
        static HostRecursiveSpinLock mutex;
        return mutex;
    }

    /**
     * @brief RAII lock for HostLifetimeMutex(), with per-thread nesting accounting.
     *
     * The nesting depth is how a caller can tell whether it currently owns the host mutex.
     * That matters for deadlock detection: a worker must never block on another worker's
     * symbol while it owns the host mutex, because the other worker will need the same mutex
     * to finish building it.
     */
    struct HostLifetimeLock {
        HostLifetimeLock() {
            HostLifetimeMutex().lock();
            ++Depth();
        }

        ~HostLifetimeLock() {
            --Depth();
            HostLifetimeMutex().unlock();
        }

        HostLifetimeLock(const HostLifetimeLock&) = delete;
        HostLifetimeLock& operator=(const HostLifetimeLock&) = delete;

        /// Nesting depth of the host mutex on the calling thread; zero means "not held".
        static int& Depth() {
            static thread_local int depth = 0;
            return depth;
        }
    };

    /**
     * @brief This defines smart pointer to manage <tt>AddRef</tt> and <tt>Release</tt>
     *        calls to FCM objects. In <tt> AutoPtr<T> </tt>, T has to be an
     *        interface derived from IFCMUnknown. <tt> AutoPtr<T> </tt> can be
     *        used to control the lifetime of objects without explicitly
     *        calling <tt>AddRef</tt> and <tt>Release</tt>.
     */
    template <typename T>
    class AutoPtr {
    public:
        /** Raw pointer */
        T* m_Ptr;

        AutoPtr() { this->m_Ptr = 0; }

        /**
         * @brief  Constructs an <tt> AutoPtr<T> </tt> object.
         *
         * @param  p (IN)
         *         The pointer to object of type T. T has to be derived from IFCMUnknown
         *         The constructor in turn calls <tt>AddRef</tt> on the pointer p
         *
         */
        AutoPtr(T* p) {
            this->m_Ptr = p;

            HostLifetimeLock lock;
            FCM_ADDREF(this->m_Ptr);
        }

        /**
         * @brief  Copy constructor for <tt> AutoPtr<T> </tt>
         *
         * @param  pObj (IN)
         *         An object of <tt> AutoPtr<T> </tt>. The constructor in turn
         *         calls <tt>AddRef</tt> on the pointer held by pObj.
         *
         */
        AutoPtr(const AutoPtr<T>& pObj) {
            this->m_Ptr = pObj.m_Ptr;

            HostLifetimeLock lock;
            FCM_ADDREF(this->m_Ptr);
        }

        /**
         * @brief  Constructs an <tt> AutoPtr<T> </tt> object from a pointer to Q,
         *         where both T and Q have to be interfaces derived from
         *         IFCMUnknown. The construction succeeds only if
         *         the QueryInterface call for the interface T succeeds
         *         on the object passed.
         *
         * @param  p (IN)
         *         A pointer to an object of type Q. Q has to be derived
         *         from IFCMUnknown. The QueryInterface call for the interface
         *         T on the object p points to should succeed for successful
         *         construction of <tt> AutoPtr<T> </tt>. On successful construction,
         *         it calls <tt>AddRef</tt> on the pointer p. In case of failure, the
         *         <tt> AutoPtr<T> </tt> object created points to NULL.
         *
         */
        template <typename Q>
        AutoPtr(Q* p) {
            this->m_Ptr = 0;

            if (!IsSameUnknown(p)) {
                AssignPtrWithQIOwnership(this->m_Ptr, p);
            }
        }

        /**
         * @brief  Constructs an <tt> AutoPtr<T> </tt> object from an <tt> AutoPtr<Q> </tt>
         *         object, where both T and Q are interfaces derived from IFCMUnknown.
         *         The construction succeeds only if the QueryInterface
         *         call for the interface T succeeds on the object passed.
         *
         * @param  pObj (IN)
         *         An object of <tt> AutoPtr<Q> </tt>, where Q has to be derived
         *         from IFCMUnknown. The QueryInterface call for the interface
         *         T on the object pObj should succeed for successful
         *         construction of <tt> AutoPtr<T> </tt>. On successful construction
         *         it calls <tt>AddRef</tt> on the pObj. In case of failure the
         *         <tt> AutoPtr<T> </tt> object created will point to NULL.
         *
         */
        template <typename Q>
        AutoPtr(const AutoPtr<Q>& pObj) {
            this->m_Ptr = 0;

            if (!IsSameUnknown(pObj.m_Ptr)) {
                AssignPtrWithQIOwnership(this->m_Ptr, pObj.m_Ptr);
            }
        }

        ~AutoPtr() { Reset(); }

        /**
         * @brief  The operator to convert to <tt>T *</tt>.
         */
        operator T*() const { return this->m_Ptr; }

        /**
         * @brief  The member access operator to get the pointer held by this object.
         */
        T* operator->() const {
            assert(this->m_Ptr != 0);
            return (this->m_Ptr);
        }

        /**
         * @brief  The address operator that returns the address of pointer held by this object.
         */
        T** operator&() {
            assert(this->m_Ptr == 0);
            return &this->m_Ptr;
        }

        /**
         * @brief  The operator to convert to bool.
         *
         * @return Returns true if the pointer held by this object is not NULL, else returns false.
         */
        operator bool() const { return (this->m_Ptr != 0); }

        /**
         * @brief  The <tt> != </tt> comparison operator.
         *
         * @param  p (IN)
         *         The pointer to compare.
         *
         * @return Returns true if the pointer <tt>p</tt> is not same the one held by this object.
         */
        bool operator!=(T* p) const { return (this->m_Ptr != p); }

        /**
         * @brief  The == comparison operator.
         *
         * @param  p (IN)
         *         The pointer to compare.
         *
         * @return Returns true if the pointer <tt>p</tt> is same as the one held by this object.
         */
        bool operator==(T* p) const { return (this->m_Ptr == p); }

        /**
         * @brief  The overloaded assignment operator to assign a pointer.
         *
         * @param  p (IN)
         *         The pointer to assign.
         */
        void operator=(T* p) {
            if (this->m_Ptr != p) {
                AssignPtrWithOwnership(this->m_Ptr, p);
            }
        }

        /**
         * @brief  The overloaded assignment operator to assign an <tt> AutoPtr<T> </tt> object.
         *
         * @param  pObj (IN)
         *         The <tt> AutoPtr<T> </tt> object to assign.
         */
        void operator=(const AutoPtr<T>& pObj) {
            if (this->m_Ptr != pObj.m_Ptr) {
                AssignPtrWithOwnership(this->m_Ptr, pObj.m_Ptr);
            }
        }

        /**
         * @brief  The overloaded assignment operator.
         *         to assign an <tt> AutoPtr<Q> </tt> object
         *
         * @param  pObj (IN)
         *         The <tt> AutoPtr<Q> </tt> object to assign.
         *         The QueryInterface call for the interface
         *         T on the object <tt>pObj</tt> should succeed.
         */
        template <typename Q>
        void operator=(const AutoPtr<Q>& pObj) {
            if (!IsSameUnknown(pObj.m_Ptr)) {
                AssignPtrWithQIOwnership(this->m_Ptr, pObj.m_Ptr);
            }
        }

        /**
         * @brief  Overloaded assignment operator.
         *
         * @param  p (IN)
         *         The pointer to <tt>Q</tt> to assign.
         *         The QueryInterface call for the interface
         *         T on the object <tt>p</tt> should succeed.
         */
        template <typename Q>
        void operator=(Q* p) {
            if (!IsSameUnknown(p)) {
                AssignPtrWithQIOwnership(this->m_Ptr, p);
            }
        }

        /**
         * @brief  Resets the pointer after calling <tt>Release</tt>.
         *
         */
        void Reset() {
            HostLifetimeLock lock;
            FCM_RELEASE(this->m_Ptr);
        }

        /**
         * @brief  Detaches the pointer without calling <tt>Release</tt>.
         *
         */
        void Detach() { this->m_Ptr = 0; }

        /**
         * @brief  Attaches the pointer without calling <tt>AddRef</tt>.
         */
        void Attach(T* p) {
            Reset();
            this->m_Ptr = p;
        }

        /**
         * @brief  Returns true if the underlying <tt>IFCMUnknown</tt> pointer
         *         this object is same as that of the argument.
         */
        template <typename Q>
        bool IsSameUnknown(Q* pArg) {
            HostLifetimeLock lock;

            if ((pArg == 0) && (0 == this->m_Ptr))
                return true;
            if ((pArg == 0) || (0 == this->m_Ptr))
                return false;

            PIFCMUnknown unkThis = 0;
            this->m_Ptr->QueryInterface(IFCMUnknown::GetIID(), (PPVoid) &unkThis);

            PIFCMUnknown unkArg = 0;
            pArg->QueryInterface(IFCMUnknown::GetIID(), (PPVoid) &unkArg);

            if (unkThis)
                unkThis->Release();
            if (unkArg)
                unkArg->Release();

            return (unkThis == unkArg);
        }

    private:
        void AssignPtrWithOwnership(T*& pDest, T* pSource) {
            HostLifetimeLock lock;

            FCM_RELEASE(this->m_Ptr);
            FCM_ADDREF(pSource);
            pDest = pSource;
        }

        void AssignPtrWithQIOwnership(T*& pDest, PIFCMUnknown pSource) {
            HostLifetimeLock lock;

            FCM_RELEASE(this->m_Ptr);
            if (pSource)
                pSource->QueryInterface(T::GetIID(), (PPVoid) &pDest);
        }
    };

    /**
     * @brief  A class implementing the AutoPtr<IFCMList>.
     */
    class FCMListPtr : public AutoPtr<IFCMList> {
    public:
        /**
         * @brief  The array index operator to access the
         *         object at a particular index.
         *
         * @param  index (IN)
         *         The index to the object in the list
         *         that needs to be accessed.
         *
         * @return Returns the IFCMUnknown pointer to the
         *         object corresponding to <tt>index</tt>.
         */
        IFCMUnknown* operator[](FCM::U_Int32 index);
    };

    /** @cond HIDE_PRIVATE */

    template <typename IntfImpl>
    class FCMObject;

    template <typename T>
    class FCMClassFactory;

    template <class T>
    class IntfImpl_Traits;

    template <typename T>
    using FCMFactoryObj = FCMObject<FCMClassFactory<T>>;

    class PluginModule {
    private:
        PluginModule();
        PluginModule(const PluginModule&) = delete;
        PluginModule& operator=(const PluginModule&) = delete;

    public:
        static const size_t TraceBufferLength = 1024;
        static const size_t TraceWideBufferLength = TraceBufferLength * 2;

    public:
        struct ClassNode {
            FLCMClassMap m_ClassMap;
            ClassNode* next;
        };

        /// Raw 16-byte representation of an FCM::SRVCID, usable as a map key.
        using ServiceKey = std::array<FCM::U_Int8, 16>;

    public:
        // FCM Memory Control
        FCM::AutoPtr<FCM::IFCMCalloc> falloc = nullptr;

        // Raw Program Console Implementation
        FCM::AutoPtr<Animate::Application::Service::IOutputConsoleService> console = nullptr;

        // App Context
        FCM::AutoPtr<Animate::Application::Service::IApplicationService> appService = nullptr;

    protected:
        ClassNode* m_firstNode;
        std::atomic<FCM::U_Int32> m_objectCounter;
        PIFCMCallback m_callback;
        const Animate::ModuleInfo& m_module;

        // Service resolution is not thread-safe inside the host (IFCMCallback::GetService
        // inserts into an unsynchronised std::map), so it is serialised here and memoised.
        // The cache also keeps the hot path (per-keyframe service lookups) lock-free of the
        // host: after the first resolution the mutex is only taken for a map lookup.
        std::recursive_mutex m_serviceMutex;
        std::map<ServiceKey, FCM::AutoPtr<FCM::IFCMUnknown>> m_serviceCache;

        PluginModule(PIFCMCallback pCallback, const Animate::ModuleInfo& module);

        ~PluginModule() = default;

    public:
        virtual FCM::U_Int32 IncrementAliveCount();
        virtual FCM::U_Int32 DecrementAliveCount();

        virtual Result GetClassInfo(IFCMCalloc* pCalloc, FCM::PFCMClassInterfaceInfo* ppClassInfo);
        virtual Result GetClassObject(FCM::ConstRefFCMCLSID clsid, FCM::ConstRefFCMIID iid, PPVoid pAny);

        virtual FCM::U_Int32 CanUnloadNow(void) const;

        virtual void Finalize();

    public:
        static PluginModule& Instance() { return *PluginModule::m_instance; }

        static ServiceKey MakeServiceKey(const FCM::SRVCID& id) {
            static_assert(sizeof(FCM::SRVCID) == sizeof(ServiceKey), "Unexpected SRVCID size");
            ServiceKey key;
            std::memcpy(key.data(), &id, sizeof(ServiceKey));
            return key;
        }

        /**
         * @brief Categories of host calls, each with its own gate.
         *
         * The host keeps document-global, unsynchronised caches behind APIs such as
         * `IFrame::GetFrameElementsByType` and the shape/region services. Resolving a
         * service is not enough: the *calls themselves* mutate that shared state, so every
         * thread that issues one has to be serialised. Not every call touches the same
         * state, so the gate is split by category.
         */
        enum class HostScope : uint32_t {
            Structure = 0, // timeline / layer traversal
            Frames,        // frame element enumeration and frame state
            Elements,      // per-element instance / tweener calls
            Shape,         // shape + region services
            Library,       // library traversal, item properties, media info
            Count
        };

        /**
         * @brief RAII gate for one host call category.
         *
         * Recursive, so a gated call may issue further gated calls of the same category on the
         * same thread. Every category takes the same host lifetime mutex (see
         * HostLifetimeMutex): the scope only documents which kind of call is being serialised.
         * A whole-symbol ("job") gate would deadlock, because a build waits on symbols owned by
         * other workers and those workers need the same mutex to finish.
         */
        class HostGate {
        public:
            explicit HostGate(HostScope /*scope*/) {}

            HostGate(const HostGate&) = delete;
            HostGate& operator=(const HostGate&) = delete;

        private:
            HostLifetimeLock m_lock;
        };

        /**
         * @brief Resolves a service through the host, memoising it for later calls.
         *
         * Safe to call from multiple threads. The host-side call is serialised.
         */
        FCM::AutoPtr<FCM::IFCMUnknown> GetServiceRaw(const FCM::SRVCID& id) {
            const ServiceKey key = MakeServiceKey(id);

            // Lock order is host mutex -> m_serviceMutex, matching every other path: an
            // AutoPtr created or destroyed while m_serviceMutex is held takes the host mutex,
            // so taking m_serviceMutex first inverts the order and deadlocks against a thread
            // that already owns the host mutex and is resolving a service.
            HostLifetimeLock host_lock;
            std::lock_guard<std::recursive_mutex> lock(m_serviceMutex);

            auto cached = m_serviceCache.find(key);
            if (cached != m_serviceCache.end()) {
                return cached->second;
            }

            FCM::AutoPtr<FCM::IFCMUnknown> service;
            FCM::Result res = m_callback->GetService(id, service.m_Ptr);
            if (FCM_FAILURE_CODE(res)) {
                throw wk::Exception("Failed to initialize FCM Service");
            }

            m_serviceCache.emplace(key, service);
            return service;
        }

        template <typename T>
        FCM::AutoPtr<T> GetService(const FCM::SRVCID& id) {
            FCM::AutoPtr<FCM::IFCMUnknown> service = GetServiceRaw(id);
            return (FCM::AutoPtr<T>) service;
        };

        template <class... Args>
        void Trace(const char* message, Args... args) {
            std::string result = std::vformat(message, std::make_format_args(args...));

            console->Trace((FCM::CStringRep16) Locale::ToUtf16(result + "\n").c_str());
        }

        template <class... Args>
        void Trace(const char16_t* message, Args... args) {
#if defined(_WINDOWS)
            wchar_t buffer[TraceWideBufferLength];
            std::swprintf(buffer, TraceWideBufferLength, (const wchar_t*) message, args...);

            std::wstring result(buffer);
            result += L"\n";

            console->Trace((FCM::CStringRep16) result.c_str());
#elif defined(__APPLE__)
            // Kinda sus
            // But tbh i dont want to mess with locales in sdk project
            // So converting like utf8-format-utf16 should be enough, i hope
            Trace(Locale::ToUtf8(std::u16string(message)), args...);
#endif
        }

        template <class... Args>
        void Trace(const std::string& message, Args... args) {
            Trace(message.c_str(), args...);
        }

        template <class... Args>
        void Trace(const std::u16string& message, Args... args) {
            Trace(message.c_str(), args...);
        }

        template <typename Publisher, typename DocumentType, typename FeatureMatrix>
        void ConstructPlugin() {
            const auto& Plugin = Publisher::PluginID();
            AddClassEntry<DocumentType>(Plugin.DocumentTypeID);
            AddClassEntry<FeatureMatrix>(Plugin.FeatureMatrixID);
            AddClassEntry<Publisher>(Plugin.PublisherID);
        }

        template <typename Publisher, typename DocumentType>
        FCM::Result RegisterPlugin(FCM::PIFCMPluginDictionary pluginDict) {
            FCM::Result res = FCM_SUCCESS;
            const auto& Plugin = Publisher::PluginID();

            FCM::AutoPtr<FCM::IFCMDictionary> dict = pluginDict;

            FCM::AutoPtr<FCM::IFCMDictionary> plugins;
            dict->AddLevel((const FCM::StringRep8) kFCMComponent, plugins.m_Ptr);

            res = Animate::DocType::RegisterDocument(plugins, Plugin.DocumentTypeID, m_module);
            if (FCM_FAILURE_CODE(res)) {
                return res;
            }

            return Animate::Publisher::RegisterPublisher(plugins, Plugin.DocumentTypeID, Plugin.PublisherID, m_module);
        }

        template <typename T>
        void AddClassEntry(FCMCLSID clsid) {
            using Factory = FCMFactoryObj<T>;
            if (!falloc)
                return;

            ClassNode* newnode = static_cast<ClassNode*>(falloc->Alloc(sizeof(ClassNode)));
            if (!newnode)
                return;

            newnode->next = 0;
            newnode->m_ClassMap.clsid = clsid;
            newnode->m_ClassMap.pFactoryCreator = [](PluginModule* pModule) -> IFCMClassFactory* {
                Factory* pFact = nullptr;

                Result res = IntfImpl_Traits<Factory>::CreateInstance(pModule, nullptr, nullptr, pFact);
                if (FCM_FAILURE_CODE(res))
                    return nullptr;

                FCM_ADDREF(pFact);
                return pFact;
            };

            newnode->m_ClassMap.pGetInterfaceTable = &T::GetInterfaceMap;
            newnode->m_ClassMap.classVersion = m_module.version;

            if (!m_firstNode) {
                m_firstNode = newnode;
            } else {
                ClassNode* current = m_firstNode;
                ClassNode* prev = 0;
                while (current) {
                    prev = current;
                    current = current->next;
                }
                prev->next = newnode;
            }
        }

    public:
        enum class PathType {
            Module,    // Path to Binaries
            Extension, // Path to Extenson Root Folder
            Locale,    // Path to folder with locales
            Assets     // Path to folder with assets files
        };

        static std::filesystem::path CurrentPath(PathType type = PathType::Extension);

        static std::string SystemInfo();

        std::string LanguageCode() const;

    protected:
        // void AddClassEntry(FCMCLSID clsid, FactorCreatorProc pFactoryCreator, InterfaceMapGetProc pGetInterfaceTable,
        // FCM::U_Int32 classVersion);

    protected:
        static PluginModule* m_instance;
    };

    /** @endcond */

    /**
     * @brief Every class that implements an interface should be derived from
     *        this class.
     */
    class FCMObjectBase {
    public:
        /**
         * @brief This function is used by the framework and should not be called by clients.
         */
        Result InitDone() { return FCM_SUCCESS; }
    };

    /** @cond HIDE_PRIVATE */

    template <typename IntfImpl>
    class FCMObject : public IntfImpl, public IFCMNoAggregationUnknown {
    private:
        std::atomic<FCM::U_Int32> mRefCount;
        PIFCMCallback m_pPIFCMCallback;
        PIFCMUnknown m_pUnknownOuter;
        PluginModule* m_pModule;

    public:
        FCMObject(PluginModule* pModule) {
            mRefCount = 0;
            m_pPIFCMCallback = 0;
            m_pUnknownOuter = 0;
            m_pModule = pModule;
            assert(pModule);
            m_pModule->IncrementAliveCount();
        }
        virtual ~FCMObject() { m_pModule->DecrementAliveCount(); }

        void FCMInit(PIFCMCallback pIFCMCallback, PIFCMUnknown pUnknownOuter) {
            m_pPIFCMCallback = pIFCMCallback;

            if (!pUnknownOuter) {
                m_pUnknownOuter = reinterpret_cast<IFCMUnknown*>(static_cast<IFCMNoAggregationUnknown*>(this));
            } else {
                m_pUnknownOuter = pUnknownOuter;
            }
        }

        virtual PIFCMCallback GetCallback() { return m_pPIFCMCallback; }

        virtual PluginModule* GetPluginModule() { return m_pModule; }
        virtual FCM::U_Int32 InternalAddRef() { return AddRef(); }
        virtual FCM::U_Int32 InternalRelease() { return Release(); }
        virtual Result InternalQueryInterface(ConstRefFCMIID pInterfaceID, PPVoid ppvObj) {
            return QueryInterface(pInterfaceID, ppvObj);
        }

        Result QueryInterface(ConstRefFCMIID pInterfaceID, PPVoid ppvObj) {
            return m_pUnknownOuter->QueryInterface(pInterfaceID, ppvObj);
        }

        FCM::U_Int32 AddRef() { return m_pUnknownOuter->AddRef(); }
        FCM::U_Int32 Release() { return m_pUnknownOuter->Release(); }

        Result MainQueryInterface(ConstRefFCMIID pInterfaceID, PPVoid ppvObj) {
            FCMInterfaceMap* pIntfMap = IntfImpl::GetInterfaceMap();
            Result res = FCM_NO_INTERFACE;
            while (pIntfMap && pIntfMap->iid != FCMIID_NULL) {
                if (pInterfaceID == pIntfMap->iid && ppvObj) {
                    if (pIntfMap->innerObjectFunc == _FCM_SIMPLEMAPENTRY) {
                        if (pInterfaceID == ID_IFCMUnknown) {
                            *ppvObj = static_cast<IFCMNoAggregationUnknown*>(this);
                        } else {
                            *ppvObj = (IFCMUnknown*) ((FCM::S_Int64) this + pIntfMap->offset);
                        }
                        (reinterpret_cast<IFCMUnknown*>(*ppvObj))->AddRef();
                        res = FCM_SUCCESS;
                        break;
                    } else {
                        res = pIntfMap->innerObjectFunc((FCM::PVoid) this, pInterfaceID, ppvObj, pIntfMap->offset);
                        break;
                    }
                }

                ++pIntfMap;
            }
            return res;
        }
        FCM::U_Int32 NoAggregationAddRef() {
            mRefCount++;
            return mRefCount;
        }
        FCM::U_Int32 NoAggregationRelease() {
            assert(mRefCount > 0);
            FCM::U_Int32 refCount = --mRefCount;
            if (refCount == 0) {
                delete this;
            }
            return refCount;
        }
    };

    template <class T>
    class IntfImpl_Traits {
    public:
        static Result CreateInstance(PluginModule* pModule, PIFCMCallback pCallback, PIFCMUnknown pUnkOuter, T*& out) {
            Result res = FCM_SUCCESS;
            out = new T(pModule);
            out->FCMInit(pCallback, pUnkOuter);
            res = out->InitDone();
            if (FCM_FAILURE_CODE(res)) {
                delete out;
                out = 0;
                return res;
            }
            return res;
        }
    };

    template <typename T>
    class FCMClassFactory : public IFCMClassFactory, public FCMObjectBase {
    public:
        FCMClassFactory() {}
        BEGIN_INTERFACE_MAP(FCMClassFactory)
        INTERFACE_ENTRY(IFCMClassFactory)
        END_INTERFACE_MAP

        Result CreateInstance(PIFCMUnknown pUnkOuter, PIFCMCallback pCallback, ConstRefFCMIID iid, PPVoid pAny) {
            Result res = FCM_GENERAL_ERROR;
            if (pUnkOuter && (iid != ID_IFCMUnknown))
                return FCM_NO_AGGREGATION;

            FCMObject<T>* pNewObject = 0;
            res = IntfImpl_Traits<FCMObject<T>>::CreateInstance(GetPluginModule(), pCallback, pUnkOuter, pNewObject);
            if (FCM_FAILURE_CODE(res))
                return res;

            res = pNewObject->MainQueryInterface(iid, pAny);
            if (FCM_SUCCESS_CODE(res)) {
                return res;
            } else
                delete pNewObject;

            return res;
        }

        virtual PluginModule* GetPluginModule() = 0;
    };

    /** @endcond */

    /**
     * @typedef PluginBootProc
     *
     * @brief   Defines the type of a function that will be called
     *          immediately after the DLL/framework load. Perform any
     *          global initialization inside this.
     */
    typedef Result (*PluginBootProc)(PIFCMCallback pCallback);

    /**
     * @typedef PluginGetClassInfoProc
     *
     * @brief   Defines the type of a function used by FCM framework to
     *          know various classes implemented by the plug-in.
     *          Do no have any app-specific logic in this function
     *
     * @param   pCalloc (IN)
     *          The pointer to the memory allocator service object.
     *
     * @param   ppClassInfo (OUT)
     *          The pointer to an array of the FCMClassInterfaceInfo objects.
     *          The plug-in has to allocate the array using pCalloc
     */
    typedef Result (*PluginGetClassInfoProc)(PIFCMCalloc pCalloc, PFCMClassInterfaceInfo* ppClassInfo);

    /**
     * @typedef PluginGetClassObjectProc
     *
     * @brief   Defines the function called by FCM framework to get the
     *          factory objects for the classes implemented by the plug-in.
     *          Do no perform any app-specific logic inside this function
     *
     * @param   pUnkOuter (IN)
     *          If NULL, indicates that the object is not being created as part of
     *          an aggregate. If non-NULL, pointer to the aggregate object's
     *          IFCMUnknown interface.
     *
     * @param   clsid (IN)
     *          The class ID associated with the data and code that will be used to
     *          create the object.
     *
     * @param   iid (IN)
     *          A reference to the identifier of the interface to be used to
     *          communicate with the object.
     *
     * @param   pAny (OUT)
     *          Address of pointer variable that receives the interface pointer
     *          requested in interfaceID. Upon successful return, *ppvObj contains the
     *          requested interface pointer pointing to the class factory object for
     *          the class with ID clsid. Upon failure, *ppvObj contains NULL.
     *          FCM framework will call <tt>Release</tt> on ppvObj to release the
     *          class factory object.
     */
    typedef Result (*PluginGetClassObjectProc)(PIFCMUnknown pUnkOuter,
                                               ConstRefFCMCLSID clsid,
                                               ConstRefFCMIID iid,
                                               PPVoid pAny);

    /**
     * @typedef PluginRegisterProc
     *
     * @brief   Defines the function called by FCM framework to register
     *          the plug-in. The plug-in has to add the details of the
     *          the services and the various components it contains to this
     *          dictionary.
     *
     * @param   pPluginDict (IN)
     *          pointer to the registration dictionary that has to be filled
     *          by the plug-in.
     */
    typedef Result (*PluginRegisterProc)(PIFCMPluginDictionary pPluginDict);

    /**
     * @typedef PluginCanUnloadNowProc
     *
     * @brief   Defines the function called by FCM framework to check
     *          if the plug-in can be unloaded. This function returns
     *          the number of live object instances from by this plug-in.
     *          Just before the shutdown, all the instances should be deleted
     *          and zero should be returned
     *
     * @return  This function returns the number of live object instances
     *          from by this plug-in.
     */
    typedef FCM::U_Int32 (*PluginCanUnloadNowProc)(void);

    /**
     * @typedef PluginShutdownProc
     *
     * @brief   Defines the function called by FCM framework to notify
     *          the plug-in that the plug-in will be unloaded soon. This
     *          function allows the plug-in to perform proper clean-up
     *          before unloading.
     */
    typedef Result (*PluginShutdownProc)();

    /** @cond HIDE_PRIVATE */

    typedef struct {
        /* Boot function will be called immediately after the DLL/framework load.
         *  Perfrom any global intitialization inside this
         */
        PluginBootProc m_pBoot;

        /* GetClassinfo is used to find out the different classes implemented
         *  by the plugin. Do no perform any app specific logic here
         */
        PluginGetClassInfoProc m_pGetClassInfo;

        /* GetClassObject is used to create the factory for each class implemented
         *  by the plugin. Do no perform any app specific logic here
         */
        PluginGetClassObjectProc m_pGetClassObject;

        /* Register function is used publish the services and capabilities of the plugin.
         *  This may not be called for plugin loading
         */
        PluginRegisterProc m_pRegister;

        /* CanUnloadNow returns the number of live object instances from by this plugin.
         *  Just before the shutdown all the instances should be deleted and zero should be returned
         */
        PluginCanUnloadNowProc m_pCanUnloadNow;

        /* Shutdown allows the plugin to do proper clean-up before unloading.
         */
        PluginShutdownProc m_pShutdown;
    } PluginProcs;

    /** @endcond */
};

#include "animate/core/common/FCMPostConfig.h"

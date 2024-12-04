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

#include "FCMTypes.h"
#include "FCMErrors.h"
#include "FCMMacros.h"

#include "FCMPublicIDs.h"

#include "animate/core/IFCMUnknown.h"
#include "animate/core/IFCMNoAggregationUnknown.h"
#include "animate/core/IFCMClassFactory.h"

#include "animate/app/Application/Service/IApplicationService.h"
#include "animate/app/Application/Service/IOutputConsoleService.h"
#include "animate/module/Types.hpp"

#include "assert.h"

#include <string>
#include <exception>
#include <filesystem>

#include "animate/core/common/FCMPreConfig.h"

namespace Animate::Publisher
{
	static FCM::Result RegisterPublisher(FCM::PIFCMDictionary, FCM::FCMCLSID, const ModuleInfo&);
}

namespace Animate::DocType
{
	static FCM::Result RegisterDocument(FCM::PIFCMDictionary, FCM::ConstFCMCLSID, const ModuleInfo&);
}

namespace FCM
{
	/** @cond HIDE_PRIVATE */
	typedef struct {
		FCMCLSID        classID;
		FCM::U_Int32    classVersion;
		FCMIID          interfaceID;
	}    FCMClassInterfaceInfo;

	typedef FCMClassInterfaceInfo* PFCMClassInterfaceInfo;

	inline Result _Delegate(
		FCM::PVoid pv,
		FCMIID riid,
		PPVoid ppvObject,
		FCM::S_Int64 dw)
	{
		Result hRes = FCM_NO_INTERFACE;
		IFCMUnknown* p = *((IFCMUnknown**)((FCM::S_Int64)pv + dw));
		if (p != 0)
			hRes = p->QueryInterface(riid, ppvObject);
		return hRes;
	}

	typedef Result(_FCM_CREATORARGFUNC)(
		FCM::PVoid pv,
		FCMIID riid,
		PPVoid ppvObject,
		FCM::S_Int64 dw);

	class PluginModule;

	typedef IFCMClassFactory* (*FactorCreatorProc)(PluginModule*);

	typedef struct
	{
		FCMIID iid;
		FCM::S_Int64 offset;
		_FCM_CREATORARGFUNC* innerObjectFunc;
	}    FCMInterfaceMap;

	typedef FCMInterfaceMap* (*InterfaceMapGetProc)();

	typedef struct
	{
		FCMCLSID clsid;
		FactorCreatorProc   pFactoryCreator;
		InterfaceMapGetProc pGetInterfaceTable;
		FCM::U_Int32 classVersion;
	} FLCMClassMap;

	/** @endcond */

	/**
	* @brief This defines smart pointer to manage <tt>AddRef</tt> and <tt>Release</tt>
	*        calls to FCM objects. In <tt> AutoPtr<T> </tt>, T has to be an
	*        interface derived from IFCMUnknown. <tt> AutoPtr<T> </tt> can be
	*        used to control the lifetime of objects without explicitly
	*        calling <tt>AddRef</tt> and <tt>Release</tt>.
	*/
	template<typename T>
	class AutoPtr
	{
	public:

		/** Raw pointer */
		T* m_Ptr;

		AutoPtr()
		{
			this->m_Ptr = 0;
		}

		/**
		 * @brief  Constructs an <tt> AutoPtr<T> </tt> object.
		 *
		 * @param  p (IN)
		 *         The pointer to object of type T. T has to be derived from IFCMUnknown
		 *         The constructor in turn calls <tt>AddRef</tt> on the pointer p
		 *
		 */
		AutoPtr(T* p)
		{
			this->m_Ptr = p;

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
		AutoPtr(const AutoPtr<T>& pObj)
		{
			this->m_Ptr = pObj.m_Ptr;

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
		AutoPtr(Q* p)
		{
			this->m_Ptr = 0;

			if (!IsSameUnknown(p))
				AssignPtrWithQIOwnership(this->m_Ptr, p);
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
		AutoPtr(const AutoPtr<Q>& pObj)
		{
			this->m_Ptr = 0;

			if (!IsSameUnknown(pObj.m_Ptr))
				AssignPtrWithQIOwnership(this->m_Ptr, pObj.m_Ptr);
		}

		~AutoPtr()
		{
			Reset();
		}

		/**
		 * @brief  The operator to convert to <tt>T *</tt>.
		 */
		operator T* () const
		{
			return this->m_Ptr;
		}

		/**
		 * @brief  The member access operator to get the pointer held by this object.
		 */
		T* operator->() const
		{
			assert(this->m_Ptr != 0);
			return (this->m_Ptr);
		}

		/**
		 * @brief  The address operator that returns the address of pointer held by this object.
		 */
		T** operator &()
		{
			assert(this->m_Ptr == 0);
			return &this->m_Ptr;
		}

		/**
		 * @brief  The operator to convert to bool.
		 *
		 * @return Returns true if the pointer held by this object is not NULL, else returns false.
		 */
		operator bool() const
		{
			return (this->m_Ptr != 0);
		}

		/**
		 * @brief  The <tt> != </tt> comparison operator.
		 *
		 * @param  p (IN)
		 *         The pointer to compare.
		 *
		 * @return Returns true if the pointer <tt>p</tt> is not same the one held by this object.
		 */
		bool operator != (T* p) const
		{
			return  (this->m_Ptr != p);
		}

		/**
		 * @brief  The == comparison operator.
		 *
		 * @param  p (IN)
		 *         The pointer to compare.
		 *
		 * @return Returns true if the pointer <tt>p</tt> is same as the one held by this object.
		 */
		bool operator == (T* p) const
		{
			return  (this->m_Ptr == p);
		}

		/**
		 * @brief  The overloaded assignment operator to assign a pointer.
		 *
		 * @param  p (IN)
		 *         The pointer to assign.
		 */
		void operator =(T* p)
		{
			if (this->m_Ptr != p)
			{
				AssignPtrWithOwnership(this->m_Ptr, p);
			}
		}

		/**
		 * @brief  The overloaded assignment operator to assign an <tt> AutoPtr<T> </tt> object.
		 *
		 * @param  pObj (IN)
		 *         The <tt> AutoPtr<T> </tt> object to assign.
		 */
		void operator =(const AutoPtr<T>& pObj)
		{
			if (this->m_Ptr != pObj.m_Ptr)
			{
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
		void operator =(const AutoPtr<Q>& pObj)
		{
			if (!IsSameUnknown(pObj.m_Ptr))
			{
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
		void operator =(Q* p)
		{
			if (!IsSameUnknown(p))
			{
				AssignPtrWithQIOwnership(this->m_Ptr, p);
			}
		}

		/**
		 * @brief  Resets the pointer after calling <tt>Release</tt>.
		 *
		 */
		void Reset()
		{
			FCM_RELEASE(this->m_Ptr);
		}

		/**
		 * @brief  Detaches the pointer without calling <tt>Release</tt>.
		 *
		 */
		void Detach()
		{
			this->m_Ptr = 0;
		}

		/**
		 * @brief  Attaches the pointer without calling <tt>AddRef</tt>.
		 */
		void Attach(T* p)
		{
			Reset();
			this->m_Ptr = p;
		}

		/**
		 * @brief  Returns true if the underlying <tt>IFCMUnknown</tt> pointer
		 *         this object is same as that of the argument.
		 */
		template <typename Q>
		bool IsSameUnknown(Q* pArg)
		{
			if ((pArg == 0) && (0 == this->m_Ptr))
				return true;
			if ((pArg == 0) || (0 == this->m_Ptr))
				return false;

			PIFCMUnknown unkThis = 0;
			this->m_Ptr->QueryInterface(IFCMUnknown::GetIID(), (PPVoid)&unkThis);

			PIFCMUnknown unkArg = 0;
			pArg->QueryInterface(IFCMUnknown::GetIID(), (PPVoid)&unkArg);

			if (unkThis)
				unkThis->Release();
			if (unkArg)
				unkArg->Release();

			return (unkThis == unkArg);
		}

	private:

		void AssignPtrWithOwnership(T*& pDest, T* pSource)
		{
			Reset();

			FCM_ADDREF(pSource);

			pDest = pSource;
		}

		void AssignPtrWithQIOwnership(T*& pDest, PIFCMUnknown pSource)
		{
			Reset();

			if (pSource)
				pSource->QueryInterface(T::GetIID(), (PPVoid)&pDest);
		}
	};

	/**
	 * @brief  A class implementing the AutoPtr<IFCMList>.
	 */
	class FCMListPtr : public AutoPtr<IFCMList>
	{
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

	class PluginModule
	{
	public:
		static const size_t	TraceBufferLength = 1024;
		static const size_t	TraceWideBufferLength = TraceBufferLength * 2;

	public:
		struct ClassNode
		{
			FLCMClassMap m_ClassMap;
			ClassNode* next;
		};

	public:
		// FCM Memory Control
		FCM::AutoPtr<FCM::IFCMCalloc> falloc = nullptr;

		// Raw Program Console Implementation
		FCM::AutoPtr<Animate::Application::Service::IOutputConsoleService> console = nullptr;

		// App Context
		FCM::AutoPtr<Animate::Application::Service::IApplicationService> appService = nullptr;

	protected:
		ClassNode* m_firstNode;
		FCM::U_Int32					m_objectCounter;
		PIFCMCallback					m_callback;
		const Animate::ModuleInfo& m_module;

		PluginModule(PIFCMCallback pCallback, const Animate::ModuleInfo& module);
		virtual ~PluginModule() = default;

	public:
		virtual FCM::U_Int32 IncrementAliveCount();
		virtual FCM::U_Int32 DecrementAliveCount();

		virtual Result GetClassInfo(IFCMCalloc* pCalloc, FCM::PFCMClassInterfaceInfo* ppClassInfo);
		virtual Result GetClassObject(FCM::ConstRefFCMCLSID clsid, FCM::ConstRefFCMIID iid, PPVoid pAny);

		virtual FCM::U_Int32 CanUnloadNow(void) const;

		virtual void Finalize();

	public:
		static PluginModule& Instance()
		{
			return *PluginModule::m_instance;
		}

		template<typename T>
		FCM::AutoPtr<T> GetService(const FCM::SRVCID& id) {
			FCM::AutoPtr<FCM::IFCMUnknown> service;

			FCM::Result res = m_callback->GetService(id, service.m_Ptr);
			if (FCM_FAILURE_CODE(res)) {
				throw std::exception("Failed to initialize FCM Service");
			}

			return (FCM::AutoPtr<T>)service;
		};

		template <class ... Args>
		void Trace(const char* message, Args ... args) {
			char buffer[TraceBufferLength];
			std::snprintf(buffer, TraceBufferLength, message, args...);

			std::string result(buffer);
			console->Trace((FCM::CStringRep16)Locale::ToUtf16(result + "\n").c_str());
		}

		template <class ... Args>
		void Trace(const char16_t* message, Args ... args)
		{
			wchar_t buffer[TraceWideBufferLength];
			std::swprintf(buffer, TraceWideBufferLength, (const wchar_t*)message, args...);

			std::wstring result(buffer);
			result += L"\n";

			console->Trace((FCM::CStringRep16)result.c_str());
		}

		template <class ... Args>
		void Trace(const std::u16string& message, Args ... args)
		{
			Trace(message.c_str(), args...);
		}

		template<typename Publisher, typename DocumentType, typename FeatureMatrix>
		void ConstructPlugin()
		{
			AddClassEntry<Publisher>(Publisher::PluginID.PublisherID);
			AddClassEntry<DocumentType>(Publisher::PluginID.DocumentTypeID);
			AddClassEntry<FeatureMatrix>(Publisher::PluginID.FeatureMatrixID);
		}

		template<typename Publisher, typename DocumentType>
		FCM::Result RegisterPlugin(FCM::PIFCMPluginDictionary pluginDict)
		{
			FCM::Result res = FCM_SUCCESS;

			FCM::AutoPtr<FCM::IFCMDictionary> dict = pluginDict;

			FCM::AutoPtr<FCM::IFCMDictionary> plugins;
			dict->AddLevel((const FCM::StringRep8)kFCMComponent, plugins.m_Ptr);

			res = Animate::DocType::RegisterDocument(plugins, Publisher::PluginID.DocumentTypeID, m_module);
			if (FCM_FAILURE_CODE(res))
			{
				return res;
			}

			return Animate::Publisher::RegisterPublisher(plugins, Publisher::PluginID.PublisherID, m_module);
		}

		template<typename T = FCM::FCMObjectBase>
		void AddClassEntry(FCMCLSID clsid)
		{
			AddClassEntry(clsid, &FCM::FCMClassFactory<T>::GetFactory, &T::GetInterfaceMap, m_module.version);
		}

	public:
		std::string LanguageCode() const;

	public:
		enum class PathType
		{
			Module,		// Path to Binaries
			Extension,	// Path to Extenson Root Folder
			Locale,		// Path to folder with locales
			Assets		// Path to folder with assets files
		};

		static std::filesystem::path CurrentPath(PathType type = PathType::Extension);

		static std::string SystemInfo();

	protected:
		void AddClassEntry(FCMCLSID clsid, FactorCreatorProc pFactoryCreator, InterfaceMapGetProc pGetInterfaceTable, FCM::U_Int32 classVersion);

	protected:
		static PluginModule* m_instance;
	};

	namespace Locale
	{
		std::u16string ToUtf16(const std::string& string);
		std::string ToUtf8(const std::u16string& string);
	}

	/** @endcond */

	   /**
		* @brief Every class that implements an interface should be derived from
		*        this class.
		*/
	class FCMObjectBase
	{
	public:

		/**
		 * @brief This function is used by the framework and should not be called by clients.
		 */
		Result InitDone()
		{
			return FCM_SUCCESS;
		}
	};

	/** @cond HIDE_PRIVATE */

	template<typename IntfImpl>
	class FCMObject : public IntfImpl, public  IFCMNoAggregationUnknown
	{
	private:
		FCM::U_Int32 mRefCount;
		PIFCMCallback m_pPIFCMCallback;
		PIFCMUnknown  m_pUnknownOuter;
		PluginModule* m_pModule;

	public:
		FCMObject(PluginModule* pModule)
		{
			mRefCount = 0;
			m_pPIFCMCallback = 0;
			m_pUnknownOuter = 0;
			m_pModule = pModule;
			assert(pModule);
			m_pModule->IncrementAliveCount();
		}
		virtual ~FCMObject()
		{
			m_pModule->DecrementAliveCount();
		}

		void FCMInit(PIFCMCallback pIFCMCallback, PIFCMUnknown pUnknownOuter)
		{
			m_pPIFCMCallback = pIFCMCallback;

			if (!pUnknownOuter)
			{
				m_pUnknownOuter = reinterpret_cast<IFCMUnknown*>(
					static_cast<IFCMNoAggregationUnknown*>(this));
			}
			else
			{
				m_pUnknownOuter = pUnknownOuter;
			}
		}

		virtual PIFCMCallback GetCallback()
		{
			return m_pPIFCMCallback;
		}

		virtual PluginModule* GetPluginModule()
		{
			return m_pModule;
		}
		virtual FCM::U_Int32 InternalAddRef()
		{
			return AddRef();
		}
		virtual FCM::U_Int32 InternalRelease()
		{
			return Release();
		}
		virtual Result InternalQueryInterface(ConstRefFCMIID pInterfaceID, PPVoid ppvObj)
		{
			return QueryInterface(pInterfaceID, ppvObj);
		}

		Result QueryInterface(ConstRefFCMIID pInterfaceID, PPVoid ppvObj)
		{
			return m_pUnknownOuter->QueryInterface(pInterfaceID, ppvObj);
		}

		FCM::U_Int32 AddRef()
		{
			return m_pUnknownOuter->AddRef();
		}
		FCM::U_Int32 Release()
		{
			return m_pUnknownOuter->Release();
		}

		Result MainQueryInterface(ConstRefFCMIID pInterfaceID, PPVoid ppvObj)
		{
			FCMInterfaceMap* pIntfMap = IntfImpl::GetInterfaceMap();
			Result res = FCM_NO_INTERFACE;
			while (pIntfMap && pIntfMap->iid != FCMIID_NULL)
			{
				if (pInterfaceID == pIntfMap->iid && ppvObj)
				{
					if (pIntfMap->innerObjectFunc == _FCM_SIMPLEMAPENTRY)
					{
						if (pInterfaceID == ID_IFCMUnknown) {
							*ppvObj = static_cast<IFCMNoAggregationUnknown*>(this);
						}
						else
						{
							*ppvObj = (IFCMUnknown*)((FCM::S_Int64)this + pIntfMap->offset);
						}
						(reinterpret_cast<IFCMUnknown*>(*ppvObj))->AddRef();
						res = FCM_SUCCESS;
						break;
					}
					else
					{
						res = pIntfMap->innerObjectFunc((FCM::PVoid)this, pInterfaceID, ppvObj, pIntfMap->offset);
						break;
					}
				}

				++pIntfMap;
			}
			return res;
		}
		FCM::U_Int32 NoAggregationAddRef()
		{
			mRefCount++;
			return mRefCount;
		}
		FCM::U_Int32 NoAggregationRelease()
		{
			assert(mRefCount > 0);
			FCM::U_Int32 refCount = --mRefCount;
			if (refCount == 0)
			{
				delete this;
			}
			return refCount;
		}
	};

	template<class T>
	class IntfImpl_Traits
	{
	public:
		static  Result CreateInstance(PluginModule* pModule, PIFCMCallback pCallback, PIFCMUnknown pUnkOuter, T*& out)
		{
			Result res = FCM_SUCCESS;
			out = new T(pModule);
			out->FCMInit(pCallback, pUnkOuter);
			res = out->InitDone();
			if (FCM_FAILURE_CODE(res))
			{
				delete out;
				out = 0;
				return res;
			}
			return res;
		}
	};

	template<class T>
	class FCMClassFactory : public IFCMClassFactory, public FCMObjectBase
	{
	public:
		FCMClassFactory()
		{
		}
		BEGIN_INTERFACE_MAP(FCMClassFactory)
			INTERFACE_ENTRY(IFCMClassFactory)
		END_INTERFACE_MAP

		Result CreateInstance(PIFCMUnknown pUnkOuter, PIFCMCallback pCallback, ConstRefFCMIID iid, PPVoid pAny)
		{
			Result res = FCM_GENERAL_ERROR;
			if (pUnkOuter && (iid != ID_IFCMUnknown))
				return FCM_NO_AGGREGATION;

			FCMObject<T>* pNewObject = 0;
			res = IntfImpl_Traits<FCMObject<T> >::CreateInstance(GetPluginModule(), pCallback, pUnkOuter, pNewObject);
			if (FCM_FAILURE_CODE(res))
				return res;

			res = pNewObject->MainQueryInterface(iid, pAny);
			if (FCM_SUCCESS_CODE(res))
			{
				return res;
			}
			else
				delete pNewObject;

			return res;
		}
		static PIFCMClassFactory GetFactory(PluginModule* pModule)
		{
			FCMObject<FCMClassFactory<T> >* pFact = 0;
			Result res = IntfImpl_Traits<FCMObject<FCMClassFactory<T> > >::CreateInstance(pModule, 0, 0, pFact);
			if (FCM_FAILURE_CODE(res))
				return 0;
			FCM_ADDREF(pFact);
			return pFact;
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
	typedef Result(*PluginBootProc)(PIFCMCallback pCallback);

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
	typedef Result(*PluginGetClassInfoProc)(PIFCMCalloc pCalloc, PFCMClassInterfaceInfo* ppClassInfo);

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
	typedef Result(*PluginGetClassObjectProc)(PIFCMUnknown pUnkOuter,
		ConstRefFCMCLSID clsid, ConstRefFCMIID iid, PPVoid pAny);

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
	typedef Result(*PluginRegisterProc)(PIFCMPluginDictionary pPluginDict);

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
	typedef FCM::U_Int32(*PluginCanUnloadNowProc)(void);

	/**
	 * @typedef PluginShutdownProc
	 *
	 * @brief   Defines the function called by FCM framework to notify
	 *          the plug-in that the plug-in will be unloaded soon. This
	 *          function allows the plug-in to perform proper clean-up
	 *          before unloading.
	 */
	typedef Result(*PluginShutdownProc)();

	/** @cond HIDE_PRIVATE */

	typedef struct
	{
		/* Boot function will be called immediately after the DLL/framework load.
		*  Perfrom any global intitialization inside this
		*/
		PluginBootProc                m_pBoot;

		/* GetClassinfo is used to find out the different classes implemented
		*  by the plugin. Do no perform any app specific logic here
		*/
		PluginGetClassInfoProc        m_pGetClassInfo;

		/* GetClassObject is used to create the factory for each class implemented
		*  by the plugin. Do no perform any app specific logic here
		*/
		PluginGetClassObjectProc      m_pGetClassObject;

		/* Register function is used publish the services and capabilities of the plugin.
		*  This may not be called for plugin loading
		*/
		PluginRegisterProc            m_pRegister;

		/* CanUnloadNow returns the number of live object instances from by this plugin.
		*  Just before the shutdown all the instances should be deleted and zero should be returned
		*/
		PluginCanUnloadNowProc        m_pCanUnloadNow;

		/* Shutdown allows the plugin to do proper clean-up before unloading.
		*/
		PluginShutdownProc            m_pShutdown;
	} PluginProcs;

	/** @endcond */
};

#include "animate/core/common/FCMPostConfig.h"
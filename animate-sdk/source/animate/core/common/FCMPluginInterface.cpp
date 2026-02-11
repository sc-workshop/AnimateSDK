#include "AnimateCore.h"
#include "AnimateService.h"

#include "animate/publisher/wheelchair/AdobeWheelchair.h"
#include "core/string/string_converter.h"

#include <codecvt>
#include <limits.h>
#include <vector>

namespace FCM
{
	IFCMUnknown* FCMListPtr::operator[](FCM::U_Int32 index)
	{
		return this->m_Ptr->operator[](index);
	}

	PluginModule* PluginModule::m_instance = nullptr;

	PluginModule::PluginModule(PIFCMCallback pCallback, const Animate::ModuleInfo& module) :
		m_module(module),
		m_firstNode(0),
		falloc(nullptr),
		m_objectCounter(0)
	{
		PluginModule::m_instance = this;
		m_callback = pCallback;

		falloc = GetService<FCM::IFCMCalloc>(SRVCID_Core_Memory);
		console = GetService<Animate::Application::Service::IOutputConsoleService>(Animate::Application::Service::APP_OUTPUT_CONSOLE_SERVICE);
		appService = GetService<Animate::Application::Service::IApplicationService>(Animate::Application::Service::APP_SERVICE);

		auto application = GetService<Animate::Application::Service::IApplicationService>(Animate::Application::Service::APP_SERVICE);

		{
			Animate::AdobeWheelchair& wheelchair = Animate::AdobeWheelchair::Instance();

			FCM::U_Int32 version;
			application->GetVersion(version);
			wheelchair.RunWheelchair(version);
		}
	}

	FCM::U_Int32 PluginModule::IncrementAliveCount()
	{
		return ++m_objectCounter;
	}
	FCM::U_Int32 PluginModule::DecrementAliveCount()
	{
		assert(m_objectCounter > 0);
		return --m_objectCounter;
	}

	Result PluginModule::GetClassInfo(IFCMCalloc* pCalloc, FCM::PFCMClassInterfaceInfo* ppClassInfo)
	{
		FCM::U_Int32 pairCount = 0;
		for (PluginModule::ClassNode* pNode = m_firstNode; pNode != 0; pNode = pNode->next)
		{
			FCMInterfaceMap* pIntfIndex = pNode->m_ClassMap.pGetInterfaceTable();
			for (; pIntfIndex && (pIntfIndex->iid != FCMIID_NULL); ++pIntfIndex) {
				if (pIntfIndex->iid == ID_IFCMUnknown)
					continue;
				++pairCount;
			}
		}
		PFCMClassInterfaceInfo arrClsIntfinfo = *ppClassInfo = (FCMClassInterfaceInfo*)pCalloc->Alloc((sizeof(FCMClassInterfaceInfo) * (pairCount + 2)));

		if (!arrClsIntfinfo)
			return FCM_MEM_NOT_AVAILABLE;

		arrClsIntfinfo[0].classID = FCMCLSID_NULL;
		arrClsIntfinfo[0].classVersion = (FCM::U_Int32)(FCM_VERSION);
		arrClsIntfinfo[0].interfaceID = FCMIID_NULL;

		arrClsIntfinfo[pairCount + 1].classID = FCMCLSID_NULL;
		arrClsIntfinfo[pairCount + 1].classVersion = 0;
		arrClsIntfinfo[pairCount + 1].interfaceID = FCMIID_NULL;

		pairCount = 0;
		for (PluginModule::ClassNode* pNode = m_firstNode; pNode != 0; pNode = pNode->next)
		{
			FCMInterfaceMap* pIntfIndex = pNode->m_ClassMap.pGetInterfaceTable();
			for (; pIntfIndex && (pIntfIndex->iid != FCMIID_NULL); ++pIntfIndex)
			{
				if (pIntfIndex->iid == ID_IFCMUnknown)
					continue;
				arrClsIntfinfo[pairCount + 1].classID = pNode->m_ClassMap.clsid;
				arrClsIntfinfo[pairCount + 1].classVersion = pNode->m_ClassMap.classVersion;
				arrClsIntfinfo[pairCount + 1].interfaceID = pIntfIndex->iid;

				++pairCount;
			}
		}
		return FCM_SUCCESS;
	}

	Result PluginModule::GetClassObject(FCM::ConstRefFCMCLSID clsid, FCM::ConstRefFCMIID iid, PPVoid pAny)
	{
		using namespace FCM;
		Result res = FCM_GENERAL_ERROR;

		for (PluginModule::ClassNode* pNode = m_firstNode; pNode != 0; pNode = pNode->next)
		{
			if (pNode->m_ClassMap.clsid == clsid) {
				IFCMClassFactory* pf = pNode->m_ClassMap.pFactoryCreator(this);
				res = pf->QueryInterface(iid, pAny);
				pf->Release();
				return res;
			}
		}
		return FCM_CLS_NOT_DEFINED;
	}

	FCM::U_Int32 PluginModule::CanUnloadNow(void) const
	{
		return m_objectCounter;
	}

	void PluginModule::Finalize()
	{
		if (!falloc)
			return;

		ClassNode* current = m_firstNode;
		ClassNode* prev = 0;
		while (current)
		{
			prev = current;
			current = current->next;
			falloc->Free(prev);
		}

		//Release the ICalloc now. Don't delay till destructor
		falloc.Reset();
	}

	std::string PluginModule::LanguageCode() const {
		return falloc->GetString8(
			appService.m_Ptr,
			&Animate::Application::Service::IApplicationService::GetLanguageCode
		);
	}

#ifdef _WINDOWS
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

	EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#endif

#ifdef __APPLE__
    #include "CoreFoundation/CoreFoundation.h"
    #include <dlfcn.h>
    #include <sys/utsname.h>
    #include <sys/sysctl.h>
#endif

	std::filesystem::path PluginModule::CurrentPath(PluginModule::PathType type)
	{
		std::filesystem::path modulePath;

#if defined(_WINDOWS)
		char16_t* pathPtr = new char16_t[MAX_PATH];
		GetModuleFileName((HINSTANCE)&__ImageBase, (LPWSTR)pathPtr, MAX_PATH - 1);

		modulePath = std::filesystem::path(std::u16string((const char16_t*)pathPtr)).parent_path();
		delete[] pathPtr;
#elif defined(__APPLE__)
        Dl_info info;
        if (dladdr((void*)(CurrentPath), &info)) {
            modulePath = std::filesystem::path(info.dli_fname) / "../../../";
        } else {
            wk::Exception("Failed to get module path");
        }
#else
#error not implemented
#endif
		std::filesystem::path extensionPath(modulePath / "../");
		std::filesystem::path assetsPath(extensionPath / "resources");

		switch (type)
		{
		case PluginModule::PathType::Module:
			return modulePath;
		case PluginModule::PathType::Extension:
			return extensionPath;
		case PluginModule::PathType::Locale:
			return assetsPath / "locales";
		case PluginModule::PathType::Assets:
			return assetsPath;
		default:
			return modulePath;
		}
	}

	std::string PluginModule::SystemInfo()
	{
#if defined(_WINDOWS)
		std::stringstream result;

		HMODULE module = LoadLibrary(TEXT("ntdll.dll"));
		if (module) {
			typedef void (WINAPI* RtlGetVersion_FUNC) (OSVERSIONINFOEXW*);
			RtlGetVersion_FUNC func = (RtlGetVersion_FUNC)GetProcAddress(module, "RtlGetVersion");
			if (func == 0) {
				FreeLibrary(module);
				return "Unknown";
			}

			OSVERSIONINFOEXW info;
			ZeroMemory(&info, sizeof(info));

			info.dwOSVersionInfoSize = sizeof(info);
			func(&info);

			result << "Windows ";
			result << info.dwMajorVersion << "." << info.dwMinorVersion << "." << info.dwBuildNumber << " ";
		}
		else
		{
			return "Unknown";
		}

		FreeLibrary(module);

		return result.str();
#elif defined(__APPLE__)
        std::stringstream result;

        int mib[2] = {CTL_KERN, KERN_OSRELEASE};
        char osrelease[256] = {0};
        size_t size = sizeof(osrelease);
        if (sysctl(mib, 2, osrelease, &size, nullptr, 0) != 0) {
            return "Unknown";
        }

        struct utsname unameInfo;
        if (uname(&unameInfo) != 0) {
            return "Unknown";
        }

        result << "macOS " << unameInfo.release << " (" << unameInfo.sysname << ")";

        return result.str();
#else
#error not implemented
#endif
	}

	namespace Locale
	{
		std::u16string ToUtf16(const std::string& string) {
            return wk::StringConverter::ToUTF16(string);
		}

		std::string ToUtf8(const std::u16string& string) {
            return wk::StringConverter::ToUTF8(string);
		}
	}
}

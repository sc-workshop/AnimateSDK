#include "SymbolContext.h"

namespace Animate
{
	namespace Publisher
	{
		SymbolContext::SymbolContext(FCM::AutoPtr<DOM::ILibraryItem> item) : name(GetName(item)), type(GetType(item)), linkage_name(GetLinkage(item))
		{
			
		}

		SymbolContext::SymbolContext(const std::u16string& name, SymbolType type) : name(name), type(type)
		{
		}

		std::u16string SymbolContext::GetName(FCM::AutoPtr<DOM::ILibraryItem> symbol)
		{
			FCM::PluginModule& context = FCM::PluginModule::Instance();

			return context.falloc->GetString16(
				symbol.m_Ptr,
				&DOM::ILibraryItem::GetName
			);
		}

		SymbolContext::SymbolType SymbolContext::GetType(FCM::AutoPtr<DOM::ILibraryItem> symbol)
		{
			FCM::AutoPtr<FCM::IFCMDictionary> properties;
			symbol->GetProperties(properties.m_Ptr);
			if (!properties) return SymbolType::Unknown;

			std::string symbolTypeName = "";
			properties->Get(kLibProp_SymbolType_DictKey, symbolTypeName);

			SymbolType type = SymbolType::Unknown;
			if (symbolTypeName == "MovieClip")
			{
				type = SymbolType::MovieClip;
			}
			else if (symbolTypeName == "Graphic")
			{
				type = SymbolType::Graphic;
			}

			return type;
		}

		std::string SymbolContext::GetLinkage(FCM::AutoPtr<DOM::ILibraryItem> symbol)
		{
			FCM::AutoPtr<FCM::IFCMDictionary> dict;
			FCM::Result status = symbol->GetProperties(dict.m_Ptr);

			std::string result;
			if (!FCM_FAILURE_CODE(status) && dict != nullptr)
			{
				dict->Get(kLibProp_LinkageClass_DictKey, result);
			}

			return result;
		}
	}
}
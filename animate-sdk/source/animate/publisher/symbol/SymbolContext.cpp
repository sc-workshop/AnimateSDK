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

			FCM::U_Int32 valueLen;
			FCM::FCMDictRecTypeID propertyType;

			FCM::Result status = properties->GetInfo(kLibProp_SymbolType_DictKey, propertyType, valueLen);
			if (FCM_FAILURE_CODE(status) || propertyType != FCM::FCMDictRecTypeID::StringRep8)
			{
				return SymbolContext::SymbolType::Unknown;
			}

			const char* symbolType = (const char*)malloc(valueLen);
			properties->Get(kLibProp_SymbolType_DictKey, propertyType, (FCM::PVoid)symbolType, valueLen);

			SymbolType type = SymbolType::Unknown;
			if (symbolType)
			{
				if (strcmp(symbolType, "MovieClip") == 0)
				{
					type = SymbolType::MovieClip;
				}
				else if (strcmp(symbolType, "Graphic") == 0)
				{
					type = SymbolType::Graphic;
				}
			}

			free((void*)symbolType);

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
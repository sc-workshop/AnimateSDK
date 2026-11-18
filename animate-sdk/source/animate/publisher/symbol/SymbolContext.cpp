#include "SymbolContext.h"

#include <mutex>
#include <unordered_map>
#include <vector>

namespace Animate::Publisher {
    namespace {
        struct LibraryItemMetadata {
            std::u16string name;
            std::string linkage_name;
            SymbolContext::SymbolType type = SymbolContext::SymbolType::Unknown;
        };

        // Constructing a SymbolContext happens once per frame-element occurrence, so the same
        // library item is asked for its name, type and linkage thousands of times per publish.
        // Those are three separate host calls (and host calls are the serialised bottleneck),
        // while the answers never change during a publish. Memoise them per item.
        std::mutex& MetadataMutex() {
            static std::mutex mutex;
            return mutex;
        }

        std::unordered_map<const void*, LibraryItemMetadata>& Metadata() {
            static std::unordered_map<const void*, LibraryItemMetadata> metadata;
            return metadata;
        }

        SymbolContext::SymbolType ParseSymbolType(const std::string& value) {
            if (value == "MovieClip")
                return SymbolContext::SymbolType::MovieClip;
            if (value == "Graphic")
                return SymbolContext::SymbolType::Graphic;
            return SymbolContext::SymbolType::Unknown;
        }

        /**
         * @brief Reads a string entry out of a host dictionary into a buffer with headroom.
         *
         * The `Get` helper in IFCMDictionary.h sizes a `std::string` from `GetInfo` and lets the
         * host write straight into it. The host writes one byte more than `GetInfo` reports for
         * some entries, and when the string is small enough to live inside the string object
         * that extra byte lands on the /RTC1 stack guard of the neighbouring local (observed as
         * "Stack around the variable 'library_gate' was corrupted"). Copy through a heap buffer
         * with slack instead, so a host-side overrun cannot reach the stack frame, and handle a
         * zero length without the `valueLen - 1` underflow of the original helper.
         */
        std::string ReadDictionaryString(FCM::IFCMDictionary* properties, FCM::CStringRep8 key) {
            FCM::FCMDictRecTypeID type = FCM::FCMDictRecTypeID::Invalid;
            FCM::U_Int32 length = 0;
            if (FCM_FAILURE_CODE(properties->GetInfo(key, type, length)))
                return {};
            if (type != FCM::FCMDictRecTypeID::StringRep8 || length == 0)
                return {};

            std::vector<char> buffer((size_t) length + 16, '\0');
            FCM::U_Int32 written = length;
            if (FCM_FAILURE_CODE(properties->Get(key, type, (FCM::PVoid) buffer.data(), written)))
                return {};

            // The reported length includes the null terminator.
            size_t size = written > 0 ? (size_t) written - 1 : 0;
            if (size > buffer.size())
                size = buffer.size();
            return std::string(buffer.data(), size);
        }

        LibraryItemMetadata QueryMetadata(const FCM::AutoPtr<DOM::ILibraryItem>& item) {
            FCM::PluginModule& context = FCM::PluginModule::Instance();

            LibraryItemMetadata metadata;
            {
                // One gate for the whole query: the host is only entered once.
                FCM::PluginModule::HostGate library_gate(FCM::PluginModule::HostScope::Library);

                metadata.name = context.falloc->GetString16(item.m_Ptr, &DOM::ILibraryItem::GetName);

                FCM::AutoPtr<FCM::IFCMDictionary> properties;
                item->GetProperties(properties.m_Ptr);
                if (properties) {
                    metadata.type = ParseSymbolType(ReadDictionaryString(properties, kLibProp_SymbolType_DictKey));
                    metadata.linkage_name = ReadDictionaryString(properties, kLibProp_LinkageClass_DictKey);
                }
            }

            return metadata;
        }
    }

    SymbolContext::SymbolContext(const FCM::AutoPtr<DOM::ILibraryItem>& item) : library_item(item) {
        const void* key = item.m_Ptr;

        if (key) {
            std::unique_lock<std::mutex> lock(MetadataMutex());

            auto cached = Metadata().find(key);
            if (cached != Metadata().end()) {
                name = cached->second.name;
                linkage_name = cached->second.linkage_name;
                type = cached->second.type;
                return;
            }
        }

        const LibraryItemMetadata metadata = QueryMetadata(item);
        name = metadata.name;
        linkage_name = metadata.linkage_name;
        type = metadata.type;

        if (key) {
            std::lock_guard<std::mutex> lock(MetadataMutex());
            Metadata()[key] = metadata;
        }
    }

    SymbolContext::SymbolContext(const std::u16string& name, SymbolType type) :
        name(name), type(type) {
    }

    void SymbolContext::ClearCache() {
        std::lock_guard<std::mutex> lock(MetadataMutex());
        Metadata().clear();
    }

    std::u16string SymbolContext::GetName(FCM::AutoPtr<DOM::ILibraryItem> symbol) {
        FCM::PluginModule& context = FCM::PluginModule::Instance();
        FCM::PluginModule::HostGate library_gate(FCM::PluginModule::HostScope::Library);

        return context.falloc->GetString16(symbol.m_Ptr, &DOM::ILibraryItem::GetName);
    }

    SymbolContext::SymbolType SymbolContext::GetType(FCM::AutoPtr<DOM::ILibraryItem> symbol) {
        FCM::PluginModule::HostGate library_gate(FCM::PluginModule::HostScope::Library);

        FCM::AutoPtr<FCM::IFCMDictionary> properties;
        symbol->GetProperties(properties.m_Ptr);
        if (!properties)
            return SymbolType::Unknown;

        std::string symbolTypeName = "";
        properties->Get(kLibProp_SymbolType_DictKey, symbolTypeName);

        return ParseSymbolType(symbolTypeName);
    }

    bool SymbolContext::operator==(const SymbolContext& other) const {
        return name == other.name && linkage_name == other.linkage_name && type == other.type && 
            looping.GetMode() == other.looping.GetMode() && looping.GetStartFrame() == other.looping.GetStartFrame() && looping.GetEndFrame() == other.looping.GetEndFrame();
    }

    std::string SymbolContext::GetLinkage(FCM::AutoPtr<DOM::ILibraryItem> symbol) {
        FCM::PluginModule::HostGate library_gate(FCM::PluginModule::HostScope::Library);

        FCM::AutoPtr<FCM::IFCMDictionary> dict;
        FCM::Result status = symbol->GetProperties(dict.m_Ptr);

        std::string result;
        if (!FCM_FAILURE_CODE(status) && dict != nullptr) {
            dict->Get(kLibProp_LinkageClass_DictKey, result);
        }

        return result;
    }
}

#pragma once

#include "AnimateDOM.h"
#include "animate/publisher/symbol/SlicingContext.h"
#include "core/hashing/ncrypto/xxhash.h"

#include <memory>
#include <stdint.h>
#include <string>

namespace Animate::Publisher {
    class SymbolContext {
    public:
        enum class SymbolType : uint8_t {
            Unknown = 0,
            MovieClip,
            Graphic
        };

    public:
        SymbolContext(FCM::AutoPtr<DOM::ILibraryItem> item);
        SymbolContext(const std::u16string& name, SymbolType type);

    public:
        const std::u16string name;
        const std::string linkage_name;
        const SymbolType type = SymbolType::Unknown;
        uint32_t current_frame_index = 0;
        FCM::AutoPtr<DOM::ILibraryItem> library_item;

        SlicingContext slicing;

    public:
        static std::u16string GetName(FCM::AutoPtr<DOM::ILibraryItem> symbol);
        static std::string GetLinkage(FCM::AutoPtr<DOM::ILibraryItem> symbol);
        static SymbolType GetType(FCM::AutoPtr<DOM::ILibraryItem> symbol);

    public:
        bool operator==(const SymbolContext& other) const;
    };
}

namespace std {
    template <>
    struct hash<Animate::Publisher::SymbolContext> {
        std::size_t operator()(const Animate::Publisher::SymbolContext& context) const noexcept {
            wk::hash::XxHash hasher;
            hasher.update(context.name);
            hasher.update(context.linkage_name);
            hasher.update(context.type);

            return hasher.digest();
        }
    };
}
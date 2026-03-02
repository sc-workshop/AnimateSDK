#pragma once

#include "AnimateDOM.h"
#include "animate/publisher/symbol/LoopingContext.h"
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
        // Library item name like 'folder/folder2/item'
        std::u16string name;

        // Linkage name or name which is will be used to export item with
        std::string linkage_name;

        // Library item symbol type
        SymbolType type = SymbolType::Unknown;

        // Current frame index while processing in LayerBuilder
        // Useful for exception handling
        uint32_t current_frame_index = 0;

        // Native wrapper for library item
        FCM::AutoPtr<DOM::ILibraryItem> library_item;

        // 9-Slicing properties
        SlicingContext slicing;

        // Graphic MovieClip looping properties
        LoopingContext looping;

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

            hasher.update(context.looping.GetMode());
            hasher.update(context.looping.GetStartFrame());
            hasher.update(context.looping.GetEndFrame());

            return hasher.digest();
        }
    };
}
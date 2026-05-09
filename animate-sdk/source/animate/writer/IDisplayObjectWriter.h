#pragma once

#include "AnimateDOM.h"
#include "animate/publisher/ResourceReference.h"

#include <functional>

namespace Animate::Publisher {
    struct Filter {
        /**
         * @brief Boolean value which is set to true when filter is enabled
         * for the object in context, else set to false.
         */
        bool enabled = true;

        /**
         * @brief Value of the blur-X property for the object.
         */
        double blurX = 0.0;

        /**
         * @brief Value of the blur-Y property for the object.
         */
        double blurY = 0.0;

        /**
         * @brief Value of the strength property for the object in context.
         */
        int strength = 0;

        /**
         * @brief Quality type of the filter(High, Low, or Medium).
         */
        DOM::Utils::FilterQualityType quality = DOM::Utils::FilterQualityType::FILTER_QUALITY_LOW;
    };

    struct GlowFilter : public Filter {
        /**
         * @brief Color of the shadow applied to the object(in RGBA form).
         */
        DOM::Utils::COLOR color;

        /**
         * @brief Boolean variable which is set to true when innerShadow property is enabled.
         */
        bool inner_shadow = false;

        /**
         * @brief Boolean variable which is set to true when Knock out property is enabled.
         */
        bool knockout = false;
    };

    struct DropShadowFilter : public Filter {
        /**
         * @brief Color of the shadow applied to the object(in RGBA form).
         */
        DOM::Utils::COLOR color;

        /**
         * @brief Angle at which the filter effects are applied (in Radians).
         */
        double angle = 0.0;

        /**
         * @brief Distance at which the filter effects are applied.
         */
        double distance = 0.0;

        /**
         * @brief Boolean variable which is set to true when innerShadow property is enabled.
         */
        bool inner_shadow = false;

        /**
         * @brief Boolean variable which is set to true when Knock out property is enabled.
         */
        bool knockout = false;
    };

    struct DisplayObjectWriterHasher {
        static inline std::size_t counter = 0;
        size_t operator()(const std::size_t& key) const { return key; }
    };

    enum class WriterType {
        Graphic = 0,
        MovieClip,
        TextField
    };

    class IDisplayObjectWriter {
    public:
        IDisplayObjectWriter(SymbolContext& context) :
            m_symbol(context) {};
        virtual ~IDisplayObjectWriter() = default;

    public:
        virtual void SetGlowFilter(const GlowFilter&) {};
        virtual void SetDropShadowFilter(const DropShadowFilter&) {};

    public:
        virtual WriterType Type() const = 0;

        std::size_t HashCode() const {
            if (!m_hash_code) {
                m_hash_code = GenerateHash();
                if (!m_hash_code)
                    m_hash_code = ++DisplayObjectWriterHasher::counter;
            }

            return m_hash_code;
        }

    protected:
        virtual std::size_t GenerateHash() const = 0;

    public:
        /**
         * @brief Hook that called before finalizing and object hash calculation.
         * Its purpose is to let the object know that it must complete all its actions now.
         */
        virtual void PreFinalize() {};

        /**
         * @brief Writer must finalize object and add it to its own resource palette here
         * @param reference Reference of object in library
         * @param required  If True then writer must return positive status, else writer can skip object writing and return False
         * @param new_symbol True when symbol is new in library, otherwise it is used only to notify about object id
         * @return True if object was written
         */
        virtual bool Finalize(ResourceReference reference, bool required, bool new_symbol) = 0;

    protected:
        SymbolContext& m_symbol;
        mutable std::size_t m_hash_code = 0;
    };
}
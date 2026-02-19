#pragma once

#include "AnimateCore.h"
#include "AnimateDOM.h"
#include "IDisplayObjectWriter.h"
#include "animate/publisher/ResourceReference.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Animate::Publisher {
    class SharedMovieclipWriter : public IDisplayObjectWriter {
    public:
        SharedMovieclipWriter(SymbolContext& context) :
            IDisplayObjectWriter(context) {};
        virtual ~SharedMovieclipWriter() = default;

    protected:
        uint32_t m_position = 0;

    public:
        virtual void Next() {
            m_position++;
            m_symbol.current_frame_index++;
        };

        virtual void InitializeTimeline(uint32_t fps, uint32_t frameCount) = 0;

        virtual void SetLabel(const std::u16string& label) = 0;

        virtual void AddFrameElement(ResourceReference ref,
                                     FCM::BlendMode blending,
                                     const std::u16string& name,
                                     std::optional<DOM::Utils::MATRIX2D> matrix,
                                     std::optional<DOM::Utils::COLOR_MATRIX> color) = 0;
    };
}
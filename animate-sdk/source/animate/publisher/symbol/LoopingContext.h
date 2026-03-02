#pragma once

#include "AnimateDOM.h"

namespace Animate::Publisher {
    class LoopingContext {
    public:
        using LoopMode = DOM::FrameElement::AnimationLoopMode;

    public:
        LoopingContext(LoopMode mode = LoopMode::ANIMATION_LOOP, uint32_t start = 0, uint32_t end = 0xFFFFFFFF);
        LoopingContext(FCM::AutoPtr<DOM::FrameElement::IGraphic> graphic);

    public:
        LoopMode GetMode() const { return m_mode; }
        uint32_t GetStartFrame() const { return m_start_frame; }
        uint32_t GetEndFrame() const { return m_end_frame; }

    private:
        LoopMode m_mode = LoopMode::ANIMATION_LOOP;
        uint32_t m_start_frame = 0;
        uint32_t m_end_frame = 0xFFFFFFFF;
    };
}
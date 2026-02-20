#include "FrameIterator.h"

namespace Animate::Publisher {
    FrameIterator::FrameIterator(LoopingContext& context, uint32_t total_duration) {
        m_start = context.GetStartFrame();
        m_end = context.GetEndFrame();

        if (m_end == 0xFFFFFFFF)
            m_end = total_duration;

        switch (context.GetMode()) {
            case LoopingContext::LoopMode::ANIMATION_LOOP:
                m_end = total_duration;
                break;
            case LoopingContext::LoopMode::ANIMATION_PLAY_ONCE:
                m_once = true;
                break;
            case LoopingContext::LoopMode::ANIMATION_PLAY_REVERSE:
                m_reverse = true;
                break;
            case LoopingContext::LoopMode::ANIMATION_PLAY_REVERSE_ONCE:
                m_reverse = true;
                m_once = true;
                break;
            case LoopingContext::LoopMode::ANIMATION_SINGLE_FRAME:
                m_end = m_start + 1;
                break;
        }

        m_current = m_start;
    }

    FrameIterator& FrameIterator::operator++() {
        m_current++;
        return *this;
    }

    FrameIterator::operator uint32_t() {
        return m_current;
    }

    bool FrameIterator::Active() const {
        if (m_reverse)
            return m_current > m_end;
        return m_end > m_current;
    }

    uint32_t FrameIterator::Duration() const {
        return m_end - m_start;
    }
}
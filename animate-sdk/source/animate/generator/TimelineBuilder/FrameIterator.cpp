#include "FrameIterator.h"

namespace Animate::Publisher {
    FrameIterator::FrameIterator(LoopingContext& context, uint32_t total_duration) {
        m_start = std::min(context.GetStartFrame(), total_duration - 1);
        m_end = context.GetEndFrame();

        switch (context.GetMode()) {
            case LoopingContext::LoopMode::ANIMATION_LOOP:
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

        m_total_duration = total_duration;
        if (!HasEndFrame() && m_once) {
            m_end = total_duration;
        }

        // Should normalize end frame since it can be used internally to indicate desired duration
        else if (HasEndFrame())
            m_end = std::min(m_end, total_duration);

        if (m_start >= m_end)
            m_end = m_start + 1;
    }

    uint32_t FrameIterator::CurrentFrameIndex() const {
        return m_current;
    }

    FrameIterator& FrameIterator::operator++() {
        m_current++;
        return *this;
    }

    FrameIterator::operator uint32_t() {
        return CurrentFrame();
    }

    bool FrameIterator::Static() const {
        if (!m_once)
            return false;

        return CurrentFrame() >= EndFrame();
    }

    bool FrameIterator::Active() const {
        uint32_t duration = Duration();

        if (m_reverse)
            return m_current > duration;

        return duration > m_current;
    }

    uint32_t FrameIterator::Duration() const {
        // If end frame is null, then MovieClip should remain the same number of frames, just start from other frame
        if (!HasEndFrame())
            return m_total_duration;

        // Cut number of frames for other cases
        return EndFrame() - StartFrame();
    }

    uint32_t FrameIterator::StartFrame() const {
        return m_start;
    }

    bool FrameIterator::HasEndFrame() const {
        return m_end != 0xFFFFFFFF;
    }

    uint32_t FrameIterator::EndFrame() const {
        if (!HasEndFrame())
            return m_total_duration;

        return m_end;
    }

    uint32_t FrameIterator::CurrentFrame() const {
        //if (!HasEndFrame())
        //    return ((m_start + m_current) % m_total_duration + m_total_duration) % m_total_duration;

        return m_start + m_current;
    }
}
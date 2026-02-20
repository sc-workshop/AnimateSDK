#pragma once

#include "animate/publisher/symbol/LoopingContext.h"

namespace Animate::Publisher {
    class FrameIterator {
    public:
        FrameIterator() {}
        FrameIterator(LoopingContext& context, uint32_t total_duration);

    public:
        bool Active() const;
        uint32_t Duration() const;

        FrameIterator& operator++();
        operator uint32_t();

    private:
        uint32_t m_current = 0;
        uint32_t m_duration = 0;
        uint32_t m_start = 0;
        uint32_t m_end = 0;

        bool m_reverse = false;
        bool m_once = false;
    };
}
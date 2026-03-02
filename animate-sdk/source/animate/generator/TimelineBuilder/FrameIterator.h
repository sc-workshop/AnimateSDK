#pragma once

#include "animate/publisher/symbol/LoopingContext.h"

namespace Animate::Publisher {
    class FrameIterator {
    public:
        FrameIterator() {}
        FrameIterator(LoopingContext& context, uint32_t total_duration);

    public:
        /**
         * @brief 
         * @return True when symbol should loop just once and FrameBuilder should release single frame copy of element 
         */
        bool Static() const;

        /**
         * @brief 
         * @return True when symbol should iterate further for exporting 
         */
        bool Active() const;

        /**
         * @brief 
         * @return Total number of frames to export 
         */
        uint32_t Duration() const;

        /**
         * @brief 
         * @return Start frame index of iteration 
         */
        uint32_t StartFrame() const;

        /**
         * @brief 
         * @return True when properties has user-defined end frame index 
         */
        bool HasEndFrame() const;

        /**
         * @brief 
         * @return End frame index of iteration
         */
        uint32_t EndFrame() const;

        /**
         * @brief 
         * @return Current frame index on timeline
         */
        uint32_t CurrentFrame() const;

        /**
         * @brief 
         * @return Current frame index 
         */
        uint32_t CurrentFrameIndex() const;

        FrameIterator& operator++();
        operator uint32_t();

    private:
        uint32_t m_current = 0;
        uint32_t m_total_duration = 0;
        uint32_t m_start = 0;
        uint32_t m_end = 0;

        bool m_reverse = false;
        bool m_once = false;
    };
}
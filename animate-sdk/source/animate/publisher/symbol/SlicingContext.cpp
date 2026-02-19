#include "SlicingContext.h"

#include "animate/publisher/wheelchair/AdobeWheelchair.h"

namespace Animate::Publisher {
    SlicingContext::SlicingContext() {
    }

    SlicingContext::SlicingContext(FCM::AutoPtr<DOM::ITimeline1> timeline) {
        AdobeWheelchair& wheelchair = AdobeWheelchair::Instance();

        uint8_t* page = (uint8_t*) timeline->GetDocPage();
        m_enabled = *(uint8_t*) (page + wheelchair.CPicPage_Is9SliceEnabled) >= 1;

        if (m_enabled) {
            uint8_t* grid = page + wheelchair.CPicPage_9SliceGuides;

            uint8_t offset = 0;
            auto readValue = [&]() {
                // Some ugly uh methods to get values
                const size_t valueOffset = offset * sizeof(int32_t);
                const int32_t normalized = *(int32_t*) (grid + valueOffset);

                double scaled = normalized / 20.0;
                double rounded = std::round(scaled * 1000.0) / 1000.0;
                offset++;

                return static_cast<float>(rounded);
            };

            m_guides.topLeft.y = readValue();
            m_guides.bottomRight.y = readValue();

            m_guides.topLeft.x = readValue();
            m_guides.bottomRight.x = readValue();
        }
    }
}

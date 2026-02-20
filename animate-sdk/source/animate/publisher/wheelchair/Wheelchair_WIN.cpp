#include "AdobeWheelchair.h"
#if defined(_WINDOWS)

namespace Animate {
    void AdobeWheelchair::RunWheelchair(FCM::U_Int32 version) {
        size_t major = ((version >> 24) & 0xFF);

        // Animate 2024
        // Most likely not working
        if (major == 24) {
            CPicGraphic_stageFrameEndOffset = 224;
            CPicPage_Is9SliceEnabled = 1744;
            CPicPage_9SliceGuides = 1748;
            m_initialized = true;
            return;
        }

        // Animate 2021+
        if (major >= 21) {
            CPicGraphic_stageFrameEndOffset = 224;
            CPicPage_Is9SliceEnabled = 1624;
            CPicPage_9SliceGuides = 1628;
            CPicObj_IsButton = 544;
            CPicSprite_GetName = 864;

            m_initialized = true;
            return;
        }

        m_initialized = false;
    }
}
#endif

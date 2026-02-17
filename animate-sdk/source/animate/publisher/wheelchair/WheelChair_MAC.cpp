#include "AdobeWheelchair.h"
#if defined(__APPLE__)

namespace Animate
{
	void AdobeWheelchair::RunWheelchair(FCM::U_Int32 version)
	{
		size_t major = ((version >> 24) & 0xFF);

		#if defined(__aarch64__)
        if (major == 24)
        {
            CPicPage_Is9SliceEnabled = 382;
            CPicPage_9SliceGuides = 1532;

            CPicObj_IsButton = 552;
            CPicSprite_GetName = 912;

            m_initialized = true;
            return;
        }
        
		#else
		// Animate 2024
		if (major == 24)
		{
			CPicPage_Is9SliceEnabled = 1528;
			CPicPage_9SliceGuides = 1532;

			CPicObj_IsButton = 552;
			CPicSprite_GetName = 912;

			m_initialized = true;
			return;
		}

		if (major >= 21) {
			CPicPage_Is9SliceEnabled = 1408;
			CPicPage_9SliceGuides = 1412;

			CPicObj_IsButton = 552;
			CPicSprite_GetName = 888;

			m_initialized = true;
			return;
		}
		#endif

		m_initialized = false;
	}
}
#endif

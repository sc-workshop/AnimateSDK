#include "AdobeWheelchair.h"
#if defined(__APPLE__)

namespace Animate
{
	void AdobeWheelchair::RunWheelchair(FCM::U_Int32 version)
	{
		size_t major = ((version >> 24) & 0xFF);

		// Animate 2024
		if (major == 24)
		{
			CPicPage_Is9SliceEnabled = 1528;
			CPicPage_9SliceGuides = 584;

			CPicObj_IsButton = 552;
			CPicSprite_GetName = 912;

			m_initialized = true;
			return;
		}

		if (major >= 21) {
			CPicPage_Is9SliceEnabled = 1408;
			CPicPage_9SliceGuides = 528;

			CPicObj_IsButton = 552;
			CPicSprite_GetName = 888;

			m_initialized = true;
			return;
		}

		m_initialized = false;
	}
}
#endif

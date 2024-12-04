#include "AdobeWheelchair.h"

namespace Animate
{
	void AdobeWheelchair::RunWheelchair(FCM::U_Int32 version)
	{
		size_t major = ((version >> 24) & 0xFF);

		// Animate 2024
		if (major == 24)
		{
			CPicPage_Is9SliceEnabled = 436;
			CPicPage_9SliceGuides = 1748;
		}

		// Animate 2023 and lower
		else if (major <= 23)
		{
			CPicPage_Is9SliceEnabled = 1624;
			CPicPage_9SliceGuides = 1628;
		}

		else
		{
			m_initialized = false;
		}
	}
}
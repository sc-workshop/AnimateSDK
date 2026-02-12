#include "AdobeWheelchair.h"
#if defined(__APPLE__)

namespace Animate
{
	void AdobeWheelchair::RunWheelchair(FCM::U_Int32)
	{
		m_initialized = false;
	}
}
#endif

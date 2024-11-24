#pragma once

#include <vector>
#include "AnimateDOM.h"

// !!!!!!!!!!!!!!!!!!!!!!!!! WARNING !!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!! DANGER ZONE !!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!  Some magic numbers for 9slice Scaling Data !!!!!!
//   Here is present some offsets for CDocumentPage structure
// !!!!!!! that we can get from ITimeliine1 interface !!!!!!!!
// !!!!!!!!! and which is not officially documented !!!!!!!!!!
// !!!!!!!!!!!!!!!!!! (many thanks to IDA) !!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!! Windows:
// !!!!!!!!! Animate: 22.x, 23.x
// !!!!! GetScale9: 1624 bytes
// !!!!! GetScale9Rect: 1628 bytes

namespace Animate
{
	namespace Publisher
	{
		class SlicingContext
		{
		public:
			SlicingContext();
			SlicingContext(FCM::AutoPtr<DOM::ITimeline1> timeline);

		public:
			bool IsEnabled() const { return m_enabled; }
			const DOM::Utils::RECT& Guides() const { return m_guides; }

		private:
			bool m_enabled = false;
			DOM::Utils::RECT m_guides = { {0.0f, 0.0f}, {0.0f, 0.0f} };
		};
	}
}
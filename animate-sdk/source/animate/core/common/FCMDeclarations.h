#pragma once
#include "FCMMacros.h"
#include "FCMTypes.h"

namespace FCM
{
	FORWARD_DECLARE_INTERFACE(IFCMCallback);
	FORWARD_DECLARE_INTERFACE(IFCMCalloc);
	FORWARD_DECLARE_INTERFACE(IFCMClassFactory);
	FORWARD_DECLARE_INTERFACE(IFCMDictionary);
	FORWARD_DECLARE_INTERFACE(IFCMList);
	FORWARD_DECLARE_INTERFACE(IFCMNoAggregationUnknown);
	FORWARD_DECLARE_INTERFACE(IFCMNotificationClient);
	FORWARD_DECLARE_INTERFACE(IFCMNotificationService);
	FORWARD_DECLARE_INTERFACE(IFCMPair);
	FORWARD_DECLARE_INTERFACE(IFCMPluginDictionary);
	FORWARD_DECLARE_INTERFACE(IFCMUnknown);
}

#include "animate/core/IFCMUnknown.h"
namespace FCM
{
	template<FCM::ConstFCMIID& T, typename CLS>
	class ChildrenVirtualClass : public CLS {
	public:
		inline static FCMIID GetIID() { return T; };
	};

	template<FCM::ConstFCMIID& T>
	class VirtualClass : public ChildrenVirtualClass<T, IFCMUnknown> {
	};
}
#pragma once

#include "AnimateCore.h"

#include "animate/app/Publisher/IPublisher.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace Animate::Publisher
{
	using namespace FCM;
	class GenericPublisher final : public IPublisher, public FCM::FCMObjectBase
	{
	public:
		BEGIN_INTERFACE_MAP(GenericPublisher)
			INTERFACE_ENTRY(IPublisher)
		END_INTERFACE_MAP

	public:
		FCM::Result _FCMCALL Publish(DOM::PIFLADocument document, const FCM::PIFCMDictionary publishSettings, const FCM::PIFCMDictionary config);

		FCM::Result _FCMCALL Publish(
			DOM::PIFLADocument document,
			DOM::PITimeline pTimeline,
			const Exporter::Service::RANGE& frameRange,
			const FCM::PIFCMDictionary publishSettings,
			const FCM::PIFCMDictionary config
		) {
			return FCM_SERVICE_NOT_FOUND;
		}

		FCM::Result _FCMCALL ClearCache() { return FCM_SERVICE_NOT_FOUND; };
	};

	FCM::Result RegisterPublisher(FCM::PIFCMDictionary plugins, FCM::FCMCLSID docId);
}
#pragma once

#include "AnimateDOM.h"

namespace Animate::Publisher
{
	class GenericPublisherConfig
	{
	public:
		GenericPublisherConfig(DOM::PIFLADocument document, const FCM::PIFCMDictionary settings, const FCM::PIFCMDictionary publishSettings) : activeDocument(document)
		{
			FromDict(publishSettings);

			// TODO: parse settings dict?
		}

		virtual void FromDict(const FCM::PIFCMDictionary dict)
		{
		}

	public:
		const DOM::PIFLADocument activeDocument;
	};
}
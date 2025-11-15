#pragma once

#include "AnimateDOM.h"

namespace Animate::Publisher
{
	class GenericPublisherConfig
	{
	public:
		virtual void SetDocument(DOM::PIFLADocument document, const FCM::PIFCMDictionary /*settings*/, const FCM::PIFCMDictionary /*publishSettings*/) {
			activeDocument = document;
		}

		virtual bool FromDict(const FCM::PIFCMDictionary /*dict*/)
		{
			return true;
		};

		virtual void Reset() {
			activeDocument = nullptr;
		}

	public:
		DOM::PIFLADocument activeDocument;
	};
}
#pragma once

#include "AnimateDOM.h"

namespace Animate::Publisher
{
	class GenericPublisherConfig
	{
	public:
		GenericPublisherConfig(DOM::PIFLADocument document, const FCM::PIFCMDictionary /*settings*/, const FCM::PIFCMDictionary /*publishSettings*/) : activeDocument(document)
		{
			// TODO: parse settings dict?
		}

		virtual void FromDict(const FCM::PIFCMDictionary /*dict*/)
		{
		};

	public:
		DOM::PIFLADocument activeDocument;
	};
}
#pragma once

#include "AnimateDOM.h"

namespace Animate::Publisher
{
	class GenericPublisherConfig
	{
	public:
		GenericPublisherConfig(DOM::PIFLADocument document, const FCM::PIFCMDictionary dict) : activeDocument(document)
		{
			FromDict(dict);
		}

		virtual void FromDict(const FCM::PIFCMDictionary dict)
		{
		}

	public:
		const DOM::PIFLADocument activeDocument;
	};
}
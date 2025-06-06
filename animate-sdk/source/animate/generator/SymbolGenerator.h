#pragma once

#include <vector>

#include "AnimateCore.h"
#include "AnimateWriter.h"

#include "TimelineBuilder/LayerBuilder.h"
#include "animate/publisher/symbol/SymbolContext.h"

namespace Animate::Publisher
{
	class ResourcePublisher;

	class SymbolGenerator {
		ResourcePublisher& m_resources;

	public:
		static void GetLayerBuilder(FCM::FCMListPtr& layers, ResourcePublisher& resources, SymbolContext& symbol, std::vector<LayerBuilder>& result);

		SymbolGenerator(ResourcePublisher& resources) : m_resources(resources) {}

		IDisplayObjectWriter* Generate(SymbolContext& symbol, FCM::AutoPtr<DOM::ITimeline1> timeline, bool required);
	};
}
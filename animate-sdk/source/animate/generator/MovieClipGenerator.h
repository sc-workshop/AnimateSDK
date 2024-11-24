#pragma once

#include <vector>

#include "AnimateCore.h"
#include "AnimateWriter.h"

#include "TimelineBuilder/LayerBuilder.h"
#include "animate/publisher/symbol/SymbolContext.h"

namespace Animate::Publisher
{
	class ResourcePublisher;

	class MovieClipGeneator {
		ResourcePublisher& m_resources;

	public:
		static void GetLayerBuilder(FCM::FCMListPtr& layers, ResourcePublisher& resources, SymbolContext& symbol, std::vector<LayerBuilder>& result);

		MovieClipGeneator(ResourcePublisher& resources) : m_resources(resources) {}

		void Generate(SharedMovieclipWriter& writer, SymbolContext& symbol, FCM::AutoPtr<DOM::ITimeline1> timeline);
	};
}
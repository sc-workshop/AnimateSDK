#pragma once

#include "AnimateCore.h"
#include "AnimateWriter.h"
#include "TimelineBuilder/LayerBuilder.h"
#include "animate/publisher/symbol/SymbolContext.h"

#include <vector>

namespace Animate::Publisher {
    class ResourcePublisher;

    class SymbolGenerator {
        ResourcePublisher& m_resources;

    public:
        static void GetLayerBuilder(
            SymbolContext& symbol, LayerBuilderContext& context, FCM::FCMListPtr& layers, ResourcePublisher& resources, std::vector<LayerBuilder>& result);

        SymbolGenerator(ResourcePublisher& resources) :
            m_resources(resources) {}

        wk::Ref<IDisplayObjectWriter> Generate(SymbolContext& symbol, FCM::AutoPtr<DOM::ITimeline1> timeline, bool required);
    };
}
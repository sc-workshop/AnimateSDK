#include "SymbolGenerator.h"

#include "animate/publisher/ResourcePublisher.h"
#include "animate/publisher/symbol/SlicingContext.h"

namespace Animate::Publisher {
    void SymbolGenerator::GetLayerBuilder(
        SymbolContext& symbol, LayerBuilderContext& context, FCM::FCMListPtr& layers, ResourcePublisher& resources, std::vector<LayerBuilder>& result) {
        uint32_t layerCount = 0;
        layers->Count(layerCount);
        result.reserve(result.capacity() + layerCount);

        for (uint32_t i = 0; layerCount > i; i++) {
            FCM::AutoPtr<DOM::ILayer2> layer = layers[i];
            if (!layer) {
                continue;
            }

            FCM::AutoPtr<FCM::IFCMUnknown> unknownLayer;
            layer->GetLayerType(unknownLayer.m_Ptr);

            FCM::AutoPtr<DOM::Layer::ILayerNormal> normalLayer = unknownLayer;
            FCM::AutoPtr<DOM::Layer::ILayerGuide> guideLayer = unknownLayer;
            FCM::AutoPtr<DOM::Layer::ILayerFolder> folderLayer = unknownLayer;

            if (folderLayer) {
                FCM::FCMListPtr folderLayers;
                folderLayer->GetChildren(folderLayers.m_Ptr);

                SymbolGenerator::GetLayerBuilder(symbol, context, folderLayers, resources, result);
                continue;
            } else if (guideLayer) {
                FCM::FCMListPtr guideChildren;
                guideLayer->GetChildren(guideChildren.m_Ptr);

                SymbolGenerator::GetLayerBuilder(symbol, context, guideChildren, resources, result);
                continue;
            } else if (normalLayer) {
                uint32_t duration = 0;
                normalLayer->GetTotalDuration(duration);
                if (duration <= 0)
                    continue;

                result.emplace_back(context, layer, duration, resources, symbol);
            }
        }
    };

    wk::Ref<IDisplayObjectWriter> SymbolGenerator::Generate(SymbolContext& symbol, FCM::AutoPtr<DOM::ITimeline1> timeline, bool required) {
        // Parsing layers data
        uint32_t duration = 0;
        timeline->GetMaxFrameCount(duration);

        SlicingContext slice_scaling = SlicingContext(timeline);
        if (slice_scaling.IsEnabled()) {
            symbol.slicing = slice_scaling;
        }

        FCM::FCMListPtr layersList;
        timeline->GetLayers(layersList.m_Ptr);

        // Creating build context and creating builder wrapper
        LayerBuilderContext context;
        std::vector<LayerBuilder> layers;
        SymbolGenerator::GetLayerBuilder(symbol, context, layersList, m_resources, layers);

        bool isStatic = (!required && symbol.linkage_name.empty()) && !symbol.slicing.IsEnabled();
        if (isStatic) {
            for (LayerBuilder& layer : layers) {
                if (!layer.IsStatic()) {
                    isStatic = false;
                    break;
                }
            }
        }

        // Trying to guess if we can use that symbol as a static shape
        if (isStatic) {
            wk::Ref<SharedShapeWriter> shape = m_resources.m_writer.AddShape(symbol);
            const auto group = LayerBuilder::ProcessStaticLayers(layers);
            if (group.has_value()) {
                shape->AddGroup(symbol, group.value());
            }

            return shape;
        } else {
            wk::Ref<SharedMovieclipWriter> movieclip = m_resources.m_writer.AddMovieclip(symbol);
            movieclip->InitializeTimeline(m_resources.document_fps, duration);
            LayerBuilder::ProcessLayers(symbol, context, layers, *movieclip, duration);

            return movieclip;
        }
    }
}
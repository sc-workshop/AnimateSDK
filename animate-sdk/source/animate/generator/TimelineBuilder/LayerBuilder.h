#pragma once

#include "AnimateDOM.h"
#include "AnimateWriter.h"
#include "FrameBuilder.h"
#include "animate/publisher/symbol/SymbolContext.h"

#include <cstdint>
#include <vector>

namespace Animate::Publisher {
    class ResourcePublisher;
    class LayerBuilder;

    struct LayerBuilderContext {
        bool frameUpdated = true;
    };

    struct MaskedLayerContext {
        // Context
        LayerBuilderContext context;

        // Array of children builders
        std::vector<LayerBuilder> layers;
    };

    class LayerBuilder {
    private:
        SymbolContext& m_symbol;

        // Total layer duration
        uint32_t m_duration = 0;

        // Current layer position on timeline
        uint32_t m_position = 0;

        // Array of native keyframes
        FCM::FCMListPtr m_keyframes;

        // Total count of layer keyframes
        uint32_t m_keyframeCount = 0;

        // Current keyframe index
        uint32_t m_keyframeIndex = 0;

        // Current native layer
        FCM::AutoPtr<DOM::ILayer2> m_layer;

        // Resource publisher from current document context
        ResourcePublisher& m_resources;

        // Symbol building context
        LayerBuilderContext& m_context;

        void UpdateFrame(SymbolContext& symbol);
        void AddModifier(SharedMovieclipWriter& writer, MaskedLayerState type);

    public:
        LayerBuilder(LayerBuilderContext& context, FCM::AutoPtr<DOM::ILayer2> layer, uint32_t duration, ResourcePublisher& resources, SymbolContext& info);

        void operator()(SharedMovieclipWriter& writer);

        operator bool() const { return m_duration > m_position; }

    public:
        void Next();

        bool IsHoldStatic() const { return !frameBuilder.StaticElements().Empty(); };

        bool ShouldReleaseStatic(const LayerBuilder& next_layer) const;

        void InheritStatic(const LayerBuilder& last_layer) { frameBuilder.InheritStatic(last_layer.frameBuilder); }

        void ReleaseStatic();

        bool IsMaskLayer() const { return (bool) maskContext; }

        bool IsStatic() const;

    public:
        // Context for masked items should be seperated to invidual scope
        wk::Ref<MaskedLayerContext> maskContext;

        FrameBuilder frameBuilder;

    public:
        static void ProcessLayerFrame(LayerBuilderContext& build_context,
                                      std::vector<LayerBuilder>& layers,
                                      SharedMovieclipWriter& writer,
                                      size_t layer_index,
                                      size_t last_layer_index,
                                      bool has_next_layer);

        static void ProcessLayers(SymbolContext& context, LayerBuilderContext& build_context, std::vector<LayerBuilder>& layers, SharedMovieclipWriter& writer);

        static void ProcessLayers(
            SymbolContext& context, LayerBuilderContext& build_context, std::vector<LayerBuilder>& layers, SharedMovieclipWriter& writer, uint32_t range);

        static const std::optional<StaticElementsGroup> ProcessStaticLayers(std::vector<LayerBuilder>& layers);
    };

}
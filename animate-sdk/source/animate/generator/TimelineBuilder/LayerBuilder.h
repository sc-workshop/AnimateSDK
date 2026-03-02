#pragma once

#include "AnimateDOM.h"
#include "AnimateWriter.h"
#include "FrameBuilder.h"
#include "FrameIterator.h"
#include "animate/publisher/symbol/SymbolContext.h"

#include <cstdint>
#include <vector>

namespace Animate::Publisher {
    class ResourcePublisher;
    class LayerBuilder;

    struct LayerBuilderContext {
        // True when one of layers has updated static elements or updated keyframe
        bool frameUpdated = true;

        // Timeline position handler
        FrameIterator iterator;
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

        /**
         * @brief Updates FrameBuilder`s frame to specific frame
         * @param target_frame Timeline frame index
         */
        void UpdateFrameTo(uint32_t target_frame);

        /**
         * @brief Updates FrameBuilder`s state
         * @param offset Frame offset in keyframe space
         */
        void UpdateFrame(uint32_t offset = 0);

        /**
         * @brief Updates masked layer state for MovieClip writer
         * @param writer target writer
         * @param type Masked layer state
         */
        void AddModifier(SharedMovieclipWriter& writer, MaskedLayerState type);

    public:
        LayerBuilder(LayerBuilderContext& context, FCM::AutoPtr<DOM::ILayer2> layer, uint32_t duration, ResourcePublisher& resources, SymbolContext& info);

        void operator()(SharedMovieclipWriter& writer);

        operator bool() const { return m_duration > m_context.iterator; }

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

        static void BuildLayers(SymbolContext& context, LayerBuilderContext& build_context, std::vector<LayerBuilder>& layers, SharedMovieclipWriter& writer);

        static const std::optional<StaticElementsGroup> BuildStaticLayers(std::vector<LayerBuilder>& layers);
    };

}
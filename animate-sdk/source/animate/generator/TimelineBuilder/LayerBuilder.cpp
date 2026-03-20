#include "LayerBuilder.h"

#include "animate/publisher/ResourcePublisher.h"

namespace Animate::Publisher {
    LayerBuilder::LayerBuilder(
        LayerBuilderContext& context, FCM::AutoPtr<DOM::ILayer2> layer, uint32_t duration, ResourcePublisher& resources, SymbolContext& symbol) :
        m_context(context),
        m_symbol(symbol),
        m_duration(duration),
        m_layer(layer),
        m_resources(resources),
        frameBuilder(symbol, context, resources) {
        FCM::AutoPtr<FCM::IFCMUnknown> unknownLayer;
        layer->GetLayerType(unknownLayer.m_Ptr);
        FCM::AutoPtr<DOM::Layer::ILayerNormal> normal_layer = unknownLayer;

        normal_layer->GetKeyFrames(m_keyframes.m_Ptr);
        m_keyframes->Count(m_keyframeCount);

        // Should get right keyframe index if start frame is non-zero
        if (context.iterator != 0) {
            UpdateFrameTo(context.iterator);
        } else {
            UpdateFrame();
        }

        FCM::AutoPtr<DOM::Layer::ILayerMask> maskLayer = unknownLayer;
        if (maskLayer) {
            FCM::FCMListPtr maskedLayers;
            maskLayer->GetChildren(maskedLayers.m_Ptr);

            maskContext = wk::CreateRef<MaskedLayerContext>();

            // Copy local iterator to masked layers context
            maskContext->context.iterator = context.iterator;

            SymbolGenerator::GetLayerBuilder(m_symbol, maskContext->context, maskedLayers, m_resources, maskContext->layers);
        }
    }

    void LayerBuilder::UpdateFrameTo(uint32_t target_frame) {
        uint32_t total_duration = 0;
        for (uint32_t i = 0; m_keyframeCount > i; i++) {
            FCM::AutoPtr<DOM::IFrame> frame = m_keyframes[i];
            uint32_t frame_duration = 0;
            frame->GetDuration(frame_duration);

            if (total_duration + frame_duration > target_frame) {
                m_keyframeIndex = i;
                UpdateFrame(target_frame - total_duration);
                break;
            }

            total_duration += frame_duration;
        }
    }

    void LayerBuilder::UpdateFrame(uint32_t offset) {
        FCM::AutoPtr<DOM::IFrame> frame = m_keyframes[m_keyframeIndex];
        frameBuilder.Update(m_layer, frame, offset);
    }

    void LayerBuilder::AddModifier(SharedMovieclipWriter& writer, MaskedLayerState type) {
        ResourceReference reference = m_resources.AddModifier(type);

        writer.AddFrameElement(reference, FCM::BlendMode::NORMAL, u"", std::nullopt, std::nullopt);
    }

    void LayerBuilder::Next() {
        // Check if frame builder is valid ahead of time
        bool isValidBuilder = (bool) frameBuilder;

        // Update frame builder
        frameBuilder.Next();

        if (m_context.iterator.Active()) {
            // Handling looping propeties
            // Check for current frame index mismatch
            if (m_context.iterator != m_symbol.current_frame_index + 1) {
                // If frame differs from what we expect here, we should update keyframe index as well
                m_context.frameUpdated = true;
                UpdateFrameTo(m_context.iterator);
                return;
            }

            // Handling cases when current frame not valid anymore but layer is still has more keyframes to reading
            if (m_keyframeCount != 0 && !frameBuilder) {
                if (m_duration > m_context.iterator) {
                    m_context.frameUpdated = true;

                    m_keyframeIndex++;
                    UpdateFrame();
                } else if (isValidBuilder) {
                    // Frame builder become invalid which means that frame was updated
                    m_context.frameUpdated = true;
                }
            }
        }
    }

    void LayerBuilder::operator()(SharedMovieclipWriter& writer) {
        frameBuilder(writer);
    }

    bool LayerBuilder::ShouldReleaseStatic(const LayerBuilder& next_layer) const {
        // Cant batch with masked layers
        if (next_layer.IsMaskLayer())
            return true;

        // Cant batch when layer is animated and may have different keyframes
        if (next_layer.frameBuilder.IsAnimated())
            return true;

        // Cant batch when layers frames has different properties
        if (frameBuilder != next_layer.frameBuilder)
            return true;

        // Cant batch when frame builder contains invalid for batching elements
        if (next_layer.frameBuilder.StaticElementsState() == FrameBuilder::StaticElementsState::Invalid)
            return true;

        // Success
        return false;
    }

    void LayerBuilder::ReleaseStatic() {
        frameBuilder.ReleaseStatic(std::u16string(u""));
    }

    bool LayerBuilder::IsStatic() const {
        if (m_keyframeCount > 1 && m_context.iterator.Duration() > 1)
            return false;

        if (IsMaskLayer())
            return false;

        return frameBuilder.IsStatic();
    }

    void LayerBuilder::ProcessLayerFrame(LayerBuilderContext& context,
                                         std::vector<LayerBuilder>& layers,
                                         SharedMovieclipWriter& writer,
                                         size_t layer_index,
                                         size_t next_layer_index,
                                         bool is_last_layer) {
        LayerBuilder& layer = layers[layer_index];
        if (layer) {
            if (!is_last_layer) {
                LayerBuilder& next_layer = layers[next_layer_index];
                if (layer.ShouldReleaseStatic(next_layer)) {
                    layer.ReleaseStatic();
                } else if (context.frameUpdated) {
                    next_layer.InheritStatic(layer);
                }
            }

            if (layer.IsHoldStatic()) {
                // Release static only in case if frame on some of layer builders is actually updated
                if (is_last_layer && context.frameUpdated) {
                    layer.ReleaseStatic();
                }
            }

            layer.frameBuilder.ApplyName(writer);
            layer(writer);
        }
    }

    void LayerBuilder::ProcessLayers(SymbolContext& context,
                                     LayerBuilderContext& build_context,
                                     std::vector<LayerBuilder>& layers,
                                     SharedMovieclipWriter& writer) {
        size_t layer_index = layers.size();
        for (size_t i = 0; layers.size() > i; i++) {
            size_t current_layer_index = --layer_index;
            size_t next_layer_index = layer_index - 1;

            LayerBuilder& current_layer = layers[current_layer_index];
            if (current_layer) {
                bool is_masked_frame = current_layer.IsMaskLayer() && current_layer.frameBuilder.FlushMask();
                bool last_layer = current_layer_index == 0;

                // Trying to find next layer index
                if (!last_layer) {
                    while (true) {
                        LayerBuilder& next_layer = layers[next_layer_index];

                        if (next_layer)
                            break;

                        if (next_layer_index == 0) {
                            last_layer = true;
                            break;
                        }

                        next_layer_index--;
                    }
                }

                // Notifying about mask layers
                if (is_masked_frame)
                    current_layer.AddModifier(writer, MaskedLayerState::MASK_LAYER);

                LayerBuilder::ProcessLayerFrame(build_context, layers, writer, current_layer_index, next_layer_index, last_layer);

                if (is_masked_frame) {
                    // Processing masked layer childrens
                    current_layer.AddModifier(writer, MaskedLayerState::MASKED_LAYERS);
                    LayerBuilder::ProcessLayers(context, current_layer.maskContext->context, current_layer.maskContext->layers, writer);
                    current_layer.AddModifier(writer, MaskedLayerState::MASKED_LAYERS_END);
                }
            } else {
                if (current_layer.IsMaskLayer() && !current_layer.maskContext->layers.empty()) {
                    LayerBuilder::ProcessLayers(context, current_layer.maskContext->context, current_layer.maskContext->layers, writer);
                }
            }
        }
    }

    void LayerBuilder::BuildLayers(SymbolContext& symbol,
                                   LayerBuilderContext& build_context,
                                   std::vector<LayerBuilder>& layers,
                                   SharedMovieclipWriter& writer) {
        while (build_context.iterator.Active()) {
            LayerBuilder::ProcessLayers(symbol, build_context, layers, writer);

            // Update timeline context state
            for (auto& layer : layers) {
                if (layer.IsMaskLayer())
                    ++layer.maskContext->context.iterator;
            }
            ++build_context.iterator;

            // Update layers state
            build_context.frameUpdated = false;
            for (LayerBuilder& layer : layers) {
                if (layer) {
                    layer.Next();
                }

                if (layer.IsMaskLayer()) {
                    for (LayerBuilder& maskedLayer : layer.maskContext->layers) {
                        maskedLayer.Next();
                    }
                }
            }
            writer.Next();
        }
    }

    const std::optional<StaticElementsGroup> LayerBuilder::BuildStaticLayers(std::vector<LayerBuilder>& layers) {
        size_t layer_index = layers.size();
        for (size_t i = 0; layers.size() > i; i++) {
            size_t current_layer_index = --layer_index;
            size_t next_layer_index = layer_index - 1;
            bool lastLayer = current_layer_index == 0;

            LayerBuilder& current_layer = layers[current_layer_index];
            if (!lastLayer) {
                LayerBuilder& next_layer = layers[next_layer_index];
                next_layer.InheritStatic(current_layer);
            } else {
                return current_layer.frameBuilder.StaticElements();
            }
        }

        return std::nullopt;
    }
}
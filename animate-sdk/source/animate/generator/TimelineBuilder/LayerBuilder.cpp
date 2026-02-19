#include "LayerBuilder.h"

#include "animate/publisher/ResourcePublisher.h"

namespace Animate::Publisher {
    void LayerBuilder::UpdateFrame(SymbolContext& symbol) {
        FCM::AutoPtr<DOM::IFrame> frame = m_keyframes[m_keyframeIndex];
        frameBuilder.Update(symbol, m_layer, frame);
    }

    void LayerBuilder::AddModifier(SharedMovieclipWriter& writer, MaskedLayerState type) {
        ResourceReference reference = m_resources.AddModifier(type);

        writer.AddFrameElement(reference, FCM::BlendMode::NORMAL, u"", std::nullopt, std::nullopt);
    }

    LayerBuilder::LayerBuilder(LayerBuilderContext& context,
                               FCM::AutoPtr<DOM::ILayer2> layer,
                               uint32_t duration,
                               ResourcePublisher& resources,
                               SymbolContext& symbol) :
        m_context(context),
        m_symbol(symbol),
        m_duration(duration),
        m_layer(layer),
        m_resources(resources),
        frameBuilder(resources) {
        FCM::AutoPtr<FCM::IFCMUnknown> unknownLayer;
        layer->GetLayerType(unknownLayer.m_Ptr);
        FCM::AutoPtr<DOM::Layer::ILayerNormal> normal_layer = unknownLayer;

        normal_layer->GetKeyFrames(m_keyframes.m_Ptr);
        m_keyframes->Count(m_keyframeCount);

        UpdateFrame(m_symbol);

        FCM::AutoPtr<DOM::Layer::ILayerMask> maskLayer = unknownLayer;
        if (maskLayer) {
            FCM::FCMListPtr maskedLayers;
            maskLayer->GetChildren(maskedLayers.m_Ptr);

            maskContext = wk::CreateRef<MaskedLayerContext>();
            SymbolGenerator::GetLayerBuilder(m_symbol,
                                             maskContext->context,
                                             maskedLayers,
                                             m_resources,
                                             maskContext->layers);
        }
    }

    void LayerBuilder::Next() {
        bool activeFrame = (bool) frameBuilder;

        frameBuilder.Next();
        m_position++;

        // if current frame not valid anymore but layer is still has more keyframes to reading
        if (m_keyframeCount != 0) {
            if (!frameBuilder && m_duration > m_position) {
                m_keyframeIndex++;
                m_context.frameUpdated = true;
                UpdateFrame(m_symbol);
            } else if (activeFrame && !frameBuilder) {
                m_context.frameUpdated = true;
            }
        }
    }

    void LayerBuilder::operator()(SharedMovieclipWriter& writer) {
        frameBuilder(m_symbol, writer);
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
        frameBuilder.ReleaseStatic(m_symbol, std::u16string(u""));
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
        // First iteration with the most of logic
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

                LayerBuilder::ProcessLayerFrame(build_context,
                                                layers,
                                                writer,
                                                current_layer_index,
                                                next_layer_index,
                                                last_layer);

                if (is_masked_frame) {
                    // Processing masked layer childrens
                    current_layer.AddModifier(writer, MaskedLayerState::MASKED_LAYERS);
                    LayerBuilder::ProcessLayers(context,
                                                current_layer.maskContext->context,
                                                current_layer.maskContext->layers,
                                                writer);
                    current_layer.AddModifier(writer, MaskedLayerState::MASKED_LAYERS_END);
                }
            } else {
                if (current_layer.IsMaskLayer() && !current_layer.maskContext->layers.empty()) {
                    LayerBuilder::ProcessLayers(context,
                                                current_layer.maskContext->context,
                                                current_layer.maskContext->layers,
                                                writer);
                }
            }
        }

        // Final iteration to update builder states
        build_context.frameUpdated = false;
        for (LayerBuilder& layer : layers) {
            if (layer) {
                layer.Next();
            }
        }
    }

    void LayerBuilder::ProcessLayers(SymbolContext& symbol,
                                     LayerBuilderContext& build_context,
                                     std::vector<LayerBuilder>& layers,
                                     SharedMovieclipWriter& writer,
                                     uint32_t range) {
        for (uint32_t t = 0; range > t; t++) {
            LayerBuilder::ProcessLayers(symbol, build_context, layers, writer);
            writer.Next();
        }
    }

    const std::optional<StaticElementsGroup> LayerBuilder::ProcessStaticLayers(std::vector<LayerBuilder>& layers) {
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

    bool LayerBuilder::IsStatic() const {
        if (m_keyframeCount > 1)
            return false;

        if (IsMaskLayer())
            return false;

        return frameBuilder.IsStatic();
    }
}
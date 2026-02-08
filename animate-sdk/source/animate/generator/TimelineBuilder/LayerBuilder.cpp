#include "LayerBuilder.h"

#include "animate/publisher/ResourcePublisher.h"

namespace Animate::Publisher
{
	void LayerBuilder::UpdateFrame(SymbolContext& symbol) {
		FCM::AutoPtr<DOM::IFrame> frame = m_keyframes[m_keyframeIndex];
		frameBuilder.Update(symbol, m_layer, frame);
	}

	void LayerBuilder::AddModifier(
		SharedMovieclipWriter& writer,
		MaskedLayerState type
	) {
		ResourceReference reference = m_resources.AddModifier(type);

		writer.AddFrameElement(
			reference,
			FCM::BlendMode::NORMAL,
			u"",
			std::nullopt,
			std::nullopt
		);
	}

	LayerBuilder::LayerBuilder(
		FCM::AutoPtr<DOM::ILayer2> layer,
		uint32_t duration,
		ResourcePublisher& resources,
		SymbolContext& symbol
	) : m_symbol(symbol), m_duration(duration), m_layer(layer), m_resources(resources), frameBuilder(resources)
	{
		FCM::AutoPtr<FCM::IFCMUnknown> unknownLayer;
		layer->GetLayerType(unknownLayer.m_Ptr);
		FCM::AutoPtr<DOM::Layer::ILayerNormal> normal_layer = unknownLayer;

		normal_layer->GetKeyFrames(m_keyframes.m_Ptr);
		m_keyframes->Count(m_keyframeCount);

		UpdateFrame(m_symbol);

		FCM::AutoPtr<DOM::Layer::ILayerMask> maskLayer = unknownLayer;
		if (maskLayer) {
			FCM::FCMListPtr layers;
			maskLayer->GetChildren(layers.m_Ptr);

			m_mask_layer = true;
			SymbolGenerator::GetLayerBuilder(layers, m_resources, m_symbol, maskedLayers);
		}
	}

	void LayerBuilder::Next() {
		frameBuilder.Next();
		m_position++;

		// if current frame not valid anymore but layer is still has more keyframes to reading
		if (m_keyframeCount != 0) {
			if (!frameBuilder && m_duration > m_position) {
				m_keyframeIndex++;
				UpdateFrame(m_symbol);
			}
		}
	}

	void LayerBuilder::operator()(SharedMovieclipWriter& writer) {
		frameBuilder(m_symbol, writer);
	}

	bool LayerBuilder::ShouldReleaseStatic(const LayerBuilder& next_layer) const
	{
		// Cant batch with masked layers
		if (next_layer.IsMaskLayer()) return true;

		// Cant batch when layer is animated and may have different keyframes
		if (next_layer.frameBuilder.IsAnimated()) return true;

		// Cant batch when duration is different
		if (frameBuilder.Duration() != next_layer.frameBuilder.Duration()) return true;

		// Cant batch when frame builders positions is different
		if (frameBuilder.Position() != next_layer.frameBuilder.Position()) return true;

		// Cant batch when layers frames has different properties
		if (frameBuilder != next_layer.frameBuilder) return true;

		// Cant batch when there is no static elements or layer`s frame builder cant afford elements inherit
		if (next_layer.frameBuilder.StaticElementsState() != FrameBuilder::StaticElementsState::Valid) return true;

		// Success
		return false;
	}

	void LayerBuilder::ReleaseStatic()
	{
		frameBuilder.ReleaseStatic(m_symbol, std::u16string(u""));
	}

	void LayerBuilder::ProcessLayerFrame(
		std::vector<LayerBuilder>& layers, SharedMovieclipWriter& writer,
		size_t layer_index, size_t next_layer_index,
		bool is_end
	)
	{
		LayerBuilder& layer = layers[layer_index];
		if (layer) {
			if (!is_end)
			{
				LayerBuilder& next_layer = layers[next_layer_index];
				if (layer.ShouldReleaseStatic(next_layer)) {
					layer.ReleaseStatic();
				}
				else
				{
					next_layer.InheritStatic(layer);
				}
			}

			if ((is_end || layer.IsMaskLayer()) && layer.IsHoldStatic())
			{
				layer.ReleaseStatic();
			}

			layer.frameBuilder.ApplyName(writer);
			layer(writer);
		}
	}

	void LayerBuilder::ProcessLayers(SymbolContext& context, std::vector<LayerBuilder>& layers, SharedMovieclipWriter& writer)
	{
		// First iteration to preprocess filled elements
		size_t layer_index = layers.size();
		for (size_t i = 0; layers.size() > i; i++) {
			size_t current_layer_index = --layer_index;
			size_t next_layer_index = layer_index - 1;

			LayerBuilder& current_layer = layers[current_layer_index];
			if (current_layer)
			{
				bool is_masked_frame = current_layer.IsMaskLayer() && current_layer.frameBuilder.FlushMask();
				if (is_masked_frame) {
					current_layer.AddModifier(writer, MaskedLayerState::MASK_LAYER);
				}

				LayerBuilder::ProcessLayerFrame(
					layers, writer,
					current_layer_index, next_layer_index,
					current_layer_index == 0
				);

				if (is_masked_frame)
				{
					current_layer.AddModifier(writer, MaskedLayerState::MASKED_LAYERS);
					LayerBuilder::ProcessLayers(context, current_layer.maskedLayers, writer);
					current_layer.AddModifier(writer, MaskedLayerState::MASKED_LAYERS_END);
				}
				else
				{
					LayerBuilder::ProcessLayers(context, current_layer.maskedLayers, writer);
				}
			}
			else
			{
				if (!current_layer.maskedLayers.empty())
				{
					LayerBuilder::ProcessLayers(context, current_layer.maskedLayers, writer);
				}
			}
		}

		// Final iteration to  change layers current positions
		for (LayerBuilder& layer : layers)
		{
			if (layer)
			{
				layer.Next();
			}
		}
	}

	void LayerBuilder::ProcessLayers(SymbolContext& symbol, std::vector<LayerBuilder>& layers, SharedMovieclipWriter& writer, uint32_t range)
	{
		for (uint32_t t = 0; range > t; t++) {
			LayerBuilder::ProcessLayers(symbol, layers, writer);
			writer.Next();
		}
	}

	const std::optional<StaticElementsGroup> LayerBuilder::ProcessStaticLayers(std::vector<LayerBuilder>& layers)
	{
		size_t layer_index = layers.size();
		for (size_t i = 0; layers.size() > i; i++) {
			size_t current_layer_index = --layer_index;
			size_t next_layer_index = layer_index - 1;
			bool lastLayer = current_layer_index == 0;

			LayerBuilder& current_layer = layers[current_layer_index];
			if (!lastLayer)
			{
				LayerBuilder& next_layer = layers[next_layer_index];
				next_layer.InheritStatic(current_layer);
			}
			else
			{
				return current_layer.frameBuilder.StaticElements();
			}
		}

		return std::nullopt;
	}

	bool LayerBuilder::IsStatic() const
	{
		if (m_keyframeCount > 1) return false;
		if (m_mask_layer) return false;

		return frameBuilder.IsStatic();
	}
}
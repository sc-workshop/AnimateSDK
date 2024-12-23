#pragma once

#include <cstdint>
#include <vector>

#include "AnimateWriter.h"
#include "AnimateDOM.h"

#include "FrameBuilder.h"
#include "animate/publisher/symbol/SymbolContext.h"

namespace Animate::Publisher
{
	class ResourcePublisher;

	class LayerBuilder {
	private:
		SymbolContext& m_symbol;

		uint32_t m_duration = 0;
		uint32_t m_position = 0;

		FCM::FCMListPtr m_keyframes;
		uint32_t m_keyframeCount = 0;
		uint32_t m_keyframeIndex = 0;

		FCM::AutoPtr<DOM::Layer::ILayerNormal> m_layer;
		ResourcePublisher& m_resources;

		bool m_mask_layer = false;

		void UpdateFrame(SymbolContext& symbol);

		void AddModifier(
			SharedMovieclipWriter& writer,
			MaskedLayerState type
		);

	public:
		LayerBuilder(FCM::AutoPtr<DOM::Layer::ILayerNormal> layer, uint32_t duration, ResourcePublisher& resources, SymbolContext& info);

		void operator()(SharedMovieclipWriter& writer);

		operator bool() const
		{
			return m_duration > m_position;
		}

	public:
		void Next();

		bool CanReleaseFilledElements() const { return !frameBuilder.FilledElements().empty(); };

		bool ShouldReleaseFilledElements(const LayerBuilder& next_layer);

		void InheritFilledElements(const LayerBuilder& last_layer)
		{
			frameBuilder.InheritFilledElements(last_layer.frameBuilder);
		}

		void ReleaseFilledElements();

		bool IsMaskLayer() const {
			return m_mask_layer;
		}

	public:
		std::vector<LayerBuilder> maskedLayers;
		FrameBuilder frameBuilder;

	public:
		static void ProcessLayerFrame(
			std::vector<LayerBuilder>& layers,
			SharedMovieclipWriter& writer,
			size_t layer_index, size_t last_layer_index,
			bool is_begin, bool is_end
		);

		static void ProcessLayers(SymbolContext& context, std::vector<LayerBuilder>& layers, SharedMovieclipWriter& writer);
		static void ProcessLayers(SymbolContext& context, std::vector<LayerBuilder>& layers, SharedMovieclipWriter& writer, uint32_t range);
	};

}
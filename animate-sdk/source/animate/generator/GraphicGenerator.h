#pragma once

#include "AnimateDOM.h"
#include "AnimateWriter.h"

#include "animate/publisher/symbol/SymbolContext.h"

namespace Animate::Publisher
{
	class ResourcePublisher;

	class GraphicGenerator {
		ResourcePublisher& m_resources;

		void GenerateLayerElements(
			SymbolContext& symbol,
			SharedShapeWriter& writer,
			FCM::FCMListPtr elements
		);

		void GenerateLayerShapes(
			SymbolContext& symbol,
			SharedShapeWriter& writer,
			FCM::AutoPtr<DOM::Layer::ILayerNormal> layer
		);

		void GenerateLayer(
			SymbolContext& symbol,
			SharedShapeWriter& writer,
			FCM::AutoPtr<DOM::ILayer2> layer
		);

		void GenerateLayerList(
			SymbolContext& symbol,
			SharedShapeWriter& writer,
			FCM::FCMListPtr layers
		);

		// Validate Stuff

		static bool ValidateLayerFrameElements(FCM::FCMListPtr frameElements);

		static bool ValidateLayerItems(
			FCM::AutoPtr<DOM::Layer::ILayerNormal> layer
		);

		static bool ValidateLayer(
			FCM::AutoPtr<DOM::ILayer2> layer
		);

		static bool ValidateLayerList(
			FCM::FCMListPtr layers
		);

	public:
		GraphicGenerator(ResourcePublisher& resources);;

		~GraphicGenerator();

		void Generate(SymbolContext& symbol, SharedShapeWriter& writer, DOM::ITimeline* timeline);

		static bool Validate(DOM::ITimeline* timeline);
	};
}
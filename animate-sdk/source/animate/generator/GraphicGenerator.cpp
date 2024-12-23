#include "GraphicGenerator.h"
#include "animate/publisher/ResourcePublisher.h"

namespace Animate::Publisher
{
	GraphicGenerator::GraphicGenerator(ResourcePublisher& resources) :
		m_resources(resources) {
	};

	GraphicGenerator::~GraphicGenerator() {
	};

	void GraphicGenerator::GenerateLayerElements(
		SymbolContext& symbol,
		SharedShapeWriter& writer,
		FCM::FCMListPtr frameElements
	) {
		FCM::PluginModule& context = FCM::PluginModule::Instance();

		uint32_t frameElementsCount = 0;
		frameElements->Count(frameElementsCount);

		uint32_t frameElementIndex = frameElementsCount;
		for (uint32_t i = 0; frameElementsCount > i; i++) {
			FCM::AutoPtr<DOM::FrameElement::IFrameDisplayElement> frameElement = frameElements[--frameElementIndex];

			DOM::Utils::MATRIX2D transformation;
			frameElement->GetMatrix(transformation);

			FCM::AutoPtr<DOM::FrameElement::IInstance> instance = frameElement;
			FCM::AutoPtr<DOM::FrameElement::IShape> shape = frameElement;
			FCM::AutoPtr<DOM::FrameElement::IGroup> group = frameElement;

			// Bitmap
			if (instance) {
				FCM::AutoPtr<DOM::ILibraryItem> item;
				instance->GetLibraryItem(item.m_Ptr);

				FCM::StringRep16 itemNamePtr;
				item->GetName(itemNamePtr);
				std::u16string itemName = (const char16_t*)itemNamePtr;
				context.falloc->Free(itemNamePtr);

				FCM::AutoPtr<DOM::LibraryItem::IMediaItem> media = item;

				if (media)
				{
					FCM::AutoPtr<FCM::IFCMUnknown> unknownMedia;
					media->GetMediaInfo(unknownMedia.m_Ptr);

					FCM::AutoPtr<DOM::MediaInfo::IBitmapInfo> bitmap = unknownMedia;

					if (bitmap) {
						SpriteElement element(media, bitmap);

						writer.AddGraphic(element, transformation);
					}
				}
			}

			// Fills / Stroke
			else if (shape)
			{
				FilledElement filledShape(symbol, shape, transformation);
				writer.AddFilledElement(filledShape);
			}

			// Group of elements
			else if (group)
			{
				FCM::FCMListPtr groupElements;
				group->GetMembers(groupElements.m_Ptr);

				GenerateLayerElements(symbol, writer, groupElements);
			}
		}
	}

	void GraphicGenerator::GenerateLayerShapes(
		SymbolContext& symbol,
		SharedShapeWriter& writer,
		FCM::AutoPtr<DOM::Layer::ILayerNormal> layer
	) {
		FCM::FCMListPtr keyframes;
		layer->GetKeyFrames(keyframes.m_Ptr);

		uint32_t keyframesCount = 0;
		keyframes->Count(keyframesCount);

		FCM::AutoPtr<DOM::IFrame> keyframe = keyframes[0];

		FCM::FCMListPtr frameElements;
		keyframe->GetFrameElements(frameElements.m_Ptr);

		GenerateLayerElements(symbol, writer, frameElements);
	}

	void GraphicGenerator::GenerateLayer(
		SymbolContext& symbol,
		SharedShapeWriter& writer,
		FCM::AutoPtr<DOM::ILayer2> layer
	) {
		FCM::AutoPtr<FCM::IFCMUnknown> unknownLayer;
		layer->GetLayerType(unknownLayer.m_Ptr);

		FCM::AutoPtr<DOM::Layer::ILayerNormal> normalLayer = unknownLayer;
		FCM::AutoPtr<DOM::Layer::ILayerMask> IsMaskLayer = unknownLayer;
		FCM::AutoPtr<DOM::Layer::ILayerGuide> guideLayer = unknownLayer;
		FCM::AutoPtr<DOM::Layer::ILayerFolder> folderLayer = unknownLayer;

		if (folderLayer) {
			FCM::FCMListPtr folderLayers;
			folderLayer->GetChildren(folderLayers.m_Ptr);
			GenerateLayerList(symbol, writer, folderLayers);
			return;
		}
		else if (guideLayer) {
			FCM::FCMListPtr childrens;
			guideLayer->GetChildren(childrens.m_Ptr);
			GenerateLayerList(symbol, writer, childrens);
			return;
		}
		else if (IsMaskLayer) {
			// TODO: masks ?

			GenerateLayer(symbol, writer, normalLayer);

			FCM::FCMListPtr maskedLayers;
			IsMaskLayer->GetChildren(maskedLayers.m_Ptr);

			GenerateLayerList(symbol, writer, maskedLayers);
			return;
		}
		else if (normalLayer) {
			GenerateLayerShapes(symbol, writer, normalLayer);
			return;
		}
	}

	void GraphicGenerator::GenerateLayerList(
		SymbolContext& symbol,
		SharedShapeWriter& writer,
		FCM::FCMListPtr layers
	) {
		uint32_t layerCount = 0;
		layers->Count(layerCount);

		uint32_t i = layerCount;
		for (uint32_t layerIndex = 0; layerCount > layerIndex; layerIndex++) {
			FCM::AutoPtr<DOM::ILayer2> layer = layers[--i];

			GenerateLayer(symbol, writer, layer);
		};
	}

	void GraphicGenerator::Generate(SymbolContext& symbol, SharedShapeWriter& writer, DOM::ITimeline* timeline) {
		FCM::FCMListPtr layers;
		timeline->GetLayers(layers.m_Ptr);

		GenerateLayerList(symbol, writer, layers);
	}

	// Validate stuff

	bool GraphicGenerator::ValidateLayerFrameElements(FCM::FCMListPtr frameElements) {
		uint32_t frameElementsCount = 0;
		frameElements->Count(frameElementsCount);

		for (uint32_t i = 0; frameElementsCount > i; i++) {
			FCM::AutoPtr<DOM::FrameElement::IInstance> instance = frameElements[i];
			FCM::AutoPtr<DOM::FrameElement::IShape> shape = frameElements[i];
			FCM::AutoPtr<DOM::FrameElement::IGroup> group = frameElements[i];

			if (group) {
				FCM::FCMListPtr groupElements;
				group->GetMembers(groupElements.m_Ptr);

				if (!ValidateLayerFrameElements(groupElements)) {
					return false;
				}
			}
			else if (instance) {
				FCM::AutoPtr<DOM::ILibraryItem> item;
				instance->GetLibraryItem(item.m_Ptr);

				FCM::AutoPtr<DOM::LibraryItem::IMediaItem> media = item;

				if (!media) {
					return false;
				}
			}
			else if (shape) {
				continue;
			}
			else {
				return false;
			}
		}

		return true;
	}

	bool GraphicGenerator::ValidateLayerItems(
		FCM::AutoPtr<DOM::Layer::ILayerNormal> layer
	) {
		FCM::FCMListPtr keyframes;
		layer->GetKeyFrames(keyframes.m_Ptr);

		uint32_t keyframesCount = 0;
		keyframes->Count(keyframesCount);

		if (keyframesCount != 1) {
			return false;
		}

		FCM::AutoPtr<DOM::IFrame> keyframe = keyframes[0];

		FCM::FCMListPtr frameElements;
		keyframe->GetFrameElements(frameElements.m_Ptr);

		return ValidateLayerFrameElements(frameElements);
	}

	bool GraphicGenerator::ValidateLayer(
		FCM::AutoPtr<DOM::ILayer2> layer
	) {
		FCM::AutoPtr<FCM::IFCMUnknown> unknownLayer;
		layer->GetLayerType(unknownLayer.m_Ptr);

		FCM::AutoPtr<DOM::Layer::ILayerNormal> normalLayer = unknownLayer;
		FCM::AutoPtr<DOM::Layer::ILayerMask> IsMaskLayer = unknownLayer;
		FCM::AutoPtr<DOM::Layer::ILayerGuide> guideLayer = unknownLayer;
		FCM::AutoPtr<DOM::Layer::ILayerFolder> folderLayer = unknownLayer;

		if (folderLayer) {
			FCM::FCMListPtr folderLayers;
			folderLayer->GetChildren(folderLayers.m_Ptr);

			return ValidateLayerList(folderLayers);
		}
		else if (guideLayer) {
			return true;
		}
		else if (IsMaskLayer) {
			FCM::FCMListPtr maskedLayers;
			IsMaskLayer->GetChildren(maskedLayers.m_Ptr);

			return ValidateLayerList(maskedLayers);
		}
		else if (normalLayer) {
			return ValidateLayerItems(normalLayer);
		}

		return false;
	}

	bool GraphicGenerator::ValidateLayerList(
		FCM::FCMListPtr layers
	) {
		uint32_t layerCount = 0;
		layers->Count(layerCount);

		for (uint32_t i = 0; layerCount > i; i++) {
			FCM::AutoPtr<DOM::ILayer2> layer = layers[i];

			if (!ValidateLayer(layer)) {
				return false;
			}
		};

		return true;
	}

	bool GraphicGenerator::Validate(DOM::ITimeline* timeline) {
		FCM::FCMListPtr layers;
		timeline->GetLayers(layers.m_Ptr);

		uint32_t timelineDuration;
		timeline->GetMaxFrameCount(timelineDuration);

		if (timelineDuration > 1) {
			return false;
		}

		return ValidateLayerList(layers);
	}
}
#include "FrameBuilder.h"

#include "animate/publisher/ResourcePublisher.h"

namespace Animate::Publisher
{
	std::string FrameBuilder::GetInstanceName(FCM::AutoPtr<DOM::FrameElement::ISymbolInstance> symbol_instance)
	{
		// TODO:

		return std::string("");
	}

	void FrameBuilder::Reset()
	{
		m_duration = 0;
		m_frame_position = 0;
		m_label.clear();
		m_elements.clear();
		m_static_elements.Clear();

		m_base_tween = nullptr;
		m_matrix_tweener = nullptr;
		m_color_tweener = nullptr;
		m_shape_tweener = nullptr;
	}

	void FrameBuilder::Update(SymbolContext& symbol, FCM::AutoPtr<DOM::IFrame> frame) {
		FCM::PluginModule& context = FCM::PluginModule::Instance();
		Reset();

		// Frame Label
		{
			DOM::KeyFrameLabelType labelType = DOM::KeyFrameLabelType::KEY_FRAME_LABEL_NONE;
			frame->GetLabelType(labelType);
			if (labelType == DOM::KeyFrameLabelType::KEY_FRAME_LABEL_NAME) {
				m_label = context.falloc->GetString16(
					frame.m_Ptr,
					&DOM::IFrame::GetLabel
				);
			}
		}

		// Frame Duration
		frame->GetDuration(m_duration);

		// Animation Tween
		{
			frame->GetTween(m_base_tween.m_Ptr);
			if (m_base_tween) {
				FCM::PIFCMDictionary tweenerDict;
				m_base_tween->GetTweenedProperties(tweenerDict);

				FCM::FCMGUID matrixGuid;
				FCM::FCMGUID colorGuid;
				FCM::FCMGUID shapeGuid;

				auto checkTweener = [&tweenerDict](FCM::StringRep8 key, FCM::FCMGUID& result)
				{
					FCM::U_Int32 valueLen;
					FCM::FCMDictRecTypeID type;

					FCM::Result res = tweenerDict->GetInfo(key, type, valueLen);
					if (FCM_FAILURE_CODE(res))
					{
						return false;
					}

					res = tweenerDict->Get(key, type, (FCM::PVoid)&result, valueLen);
					if (FCM_FAILURE_CODE(res))
					{
						return false;
					}

					return true;
				};

				bool hasMatrixTweener = checkTweener(DOM::Service::Tween::kDOMGeometricProperty, matrixGuid);
				bool hasColorTweener = checkTweener(DOM::Service::Tween::kDOMColorProperty, colorGuid);
				bool hasShapeTweener = checkTweener(DOM::Service::Tween::kDOMShapeProperty, shapeGuid);

				FCM::AutoPtr<DOM::Service::Tween::ITweenerService> TweenerService = context.GetService<DOM::Service::Tween::ITweenerService>(DOM::Service::Tween::TWEENER_SERVICE);

				FCM::AutoPtr<FCM::IFCMUnknown> unknownTweener;
				if (hasMatrixTweener) {
					TweenerService->GetTweener(matrixGuid, nullptr, unknownTweener.m_Ptr);
					m_matrix_tweener = unknownTweener;
				}
				if (hasColorTweener) {
					TweenerService->GetTweener(colorGuid, nullptr, unknownTweener.m_Ptr);
					m_color_tweener = unknownTweener;
				}
				if (hasShapeTweener) {
					TweenerService->GetTweener(shapeGuid, nullptr, unknownTweener.m_Ptr);
					m_shape_tweener = unknownTweener;
				}
			}
		}

		// Frame Elements
		{
			FCM::FCMListPtr frameElements;
			frame->GetFrameElementsByType(DOM::FrameElement::IID_IFRAME_DISPLAY_ELEMENT, frameElements.m_Ptr);

			DeclareFrameElements(symbol, frameElements, std::nullopt, false);
		}

		m_rigging_frame = frame;
	}

	void FrameBuilder::ReleaseFrameElement(SymbolContext& symbol, SharedMovieclipWriter& writer, size_t index)
	{
		FrameBuilderElement& element = m_elements[index];

		FCM::AutoPtr<DOM::ILayer> rigging_layer = nullptr;
		std::optional<Matrix_t> matrix = element.matrix;
		std::optional<Color_t> color = element.color;

		if (matrix.has_value()) {
			if (m_matrix_tweener) {
				DOM::Utils::MATRIX2D transform_matrix;
				m_matrix_tweener->GetGeometricTransform(m_base_tween, m_frame_position, transform_matrix);

				*matrix = *matrix * transform_matrix;
			}
		}
		else if (m_matrix_tweener) {
			matrix = DOM::Utils::MATRIX2D();
			m_matrix_tweener->GetGeometricTransform(m_base_tween, m_frame_position, *matrix);
		}

		if (m_rigging_frame)
		{
			m_rigging_frame->GetRigParent(m_frame_position, rigging_layer.m_Ptr);
		}

		if (rigging_layer)
		{
			DOM::Utils::RIG_PROPERTIES props;
			m_rigging_frame->GetRigProperties(m_timeline_position, props);
			matrix = *matrix * props.matrix;
		}

		if (m_color_tweener) {
			color = DOM::Utils::COLOR_MATRIX();
			m_color_tweener->GetColorMatrix(m_base_tween, m_frame_position, *color);
		}

		if (m_shape_tweener) {
			FCM::AutoPtr<DOM::FrameElement::IShape> filledShape = nullptr;
			m_shape_tweener->GetShape(m_base_tween, m_frame_position, filledShape.m_Ptr);

			// TODO: this place may have many bugs and issues
			m_static_state = FrameBuilder::StaticElementsState::Valid;
			m_static_elements.AddFilledElement(symbol, filledShape);

			return;
		}

		writer.AddFrameElement(
			element.id,
			element.blend_mode,
			element.name,
			matrix,
			color
		);
	}

	void FrameBuilder::operator()(SymbolContext& symbol, SharedMovieclipWriter& writer) {
		uint32_t i = (uint32_t)m_elements.size();
		for (uint32_t elementIndex = 0; m_elements.size() > elementIndex; elementIndex++) {
			i--;

			ReleaseFrameElement(symbol, writer, i);
		}
	}

	void FrameBuilder::DeclareFrameElements(SymbolContext& symbol, FCM::FCMListPtr frameElements, std::optional<Matrix_t> base_transform, bool reverse) {
		uint32_t frameElementsCount = 0;
		frameElements->Count(frameElementsCount);

		if (reverse)
		{
			uint32_t i = frameElementsCount - 1;
			for (uint32_t t = 0; frameElementsCount > t; t++) {
				DeclareFrameElement(symbol, frameElements[i--], base_transform);
			}
		}
		else
		{
			for (uint32_t i = 0; frameElementsCount > i; i++) {
				DeclareFrameElement(symbol, frameElements[i], base_transform);
			}
		}
	}

	void FrameBuilder::DeclareFrameElement(
		SymbolContext& symbol,
		FCM::AutoPtr<DOM::FrameElement::IFrameDisplayElement> frameElement,
		std::optional<Matrix_t> base_transform
	)
	{
		FCM::PluginModule& context = FCM::PluginModule::Instance();

		// Symbol info
		FrameBuilderElement element;

		// Transform
		Matrix_t matrix;
		frameElement->GetMatrix(matrix);

		if (base_transform)
		{
			matrix += *base_transform;
		}

		std::optional<Color_t> color = std::nullopt;

		// Game "guess who i am"
		FCM::AutoPtr<DOM::FrameElement::IInstance> libraryElement = frameElement;
		FCM::AutoPtr<DOM::IFilterable> filterableElement = frameElement;
		FCM::AutoPtr<DOM::FrameElement::IClassicText> textfieldElement = frameElement;

		FCM::AutoPtr<DOM::FrameElement::IMovieClip> movieClipElement = frameElement;
		FCM::AutoPtr<DOM::FrameElement::ISymbolInstance> symbolItem = frameElement;
		FCM::AutoPtr<DOM::FrameElement::IShape> filledShapeItem = frameElement;
		FCM::AutoPtr<DOM::FrameElement::IGroup> groupedElemenets = frameElement;

		std::optional<FCM::FCMListPtr> filters;
		if (filterableElement)
		{
			filters = FCM::FCMListPtr();
			filterableElement->GetGraphicFilters(filters->m_Ptr);
		}
		

		auto is_required = [&element]()
		{
			return !element.name.empty();
		};

		auto release_static = [&](const std::u16string& name)
			{
				if (m_static_state == FrameBuilder::StaticElementsState::Invalid)
				{
					ReleaseStatic(symbol, name);
					return true;
				}

				return false;
			};

		// Symbol
		if (libraryElement) {
			FCM::AutoPtr<DOM::ILibraryItem> libraryItem;
			libraryElement->GetLibraryItem(libraryItem.m_Ptr);
			FCM::AutoPtr<DOM::LibraryItem::IMediaItem> mediaItem = libraryItem;

			SymbolContext librarySymbol(libraryItem);

			if (symbolItem) {
				color = Color_t();
				symbolItem->GetColorMatrix(*color);
				//element.name = Localization::ToUtf16(FrameBuilder::GetInstanceName(symbolItem));
			}

			if (movieClipElement) {
				// Instance name
				element.name = context.falloc->GetString16(
					movieClipElement.m_Ptr,
					&DOM::FrameElement::IMovieClip::GetName
				);

				movieClipElement->GetBlendMode(element.blend_mode);
			}

			if (mediaItem)
			{
				FCM::AutoPtr<FCM::IFCMUnknown> mediaInfo;
				mediaItem->GetMediaInfo(mediaInfo.m_Ptr);

				FCM::AutoPtr<DOM::MediaInfo::IBitmapInfo> bitmapMedia = mediaInfo;
				m_static_elements.AddElement<BitmapElement>(symbol, mediaItem, matrix);
				if (!release_static(element.name))
				{
					m_static_state = StaticElementsState::Valid;
				}
					
				return;
			}

			m_static_state = FrameBuilder::StaticElementsState::Invalid;
			element.id = m_resources.AddLibraryItem(librarySymbol, libraryItem, is_required());
		}

		// Textfield
		else if (textfieldElement) {
			m_static_state = FrameBuilder::StaticElementsState::Invalid;

			element.id = m_resources.AddTextField(symbol, textfieldElement, filters);

			if (element.id != 0xFFFF)
			{
				FCM::AutoPtr<DOM::FrameElement::ITextBehaviour> textfieldElementBehaviour;
				textfieldElement->GetTextBehaviour(textfieldElementBehaviour.m_Ptr);

				FCM::AutoPtr<DOM::FrameElement::IModifiableTextBehaviour> modifiableTextfieldBehaviour = textfieldElementBehaviour;
				if (modifiableTextfieldBehaviour) {
					element.name = context.falloc->GetString16(
						modifiableTextfieldBehaviour.m_Ptr,
						&DOM::FrameElement::IModifiableTextBehaviour::GetInstanceName
					);
				}
			}
		}

		// Fills / Stroke
		else if (filledShapeItem) {
			m_static_elements.AddFilledElement(symbol, filledShapeItem, matrix);

			if (!release_static(element.name))
			{
				m_static_state = StaticElementsState::Valid;
			}
			return;
		}

		// Groups
		else if (groupedElemenets) {
			FCM::FCMListPtr groupElements;
			groupedElemenets->GetMembers(groupElements.m_Ptr);

			DeclareFrameElements(symbol, groupElements, matrix);
			return;
		}

		else {
			// TODO: make it more detailed
			context.Trace("Unknown resource in library. Make sure symbols don't contain unsupported elements.");
			return;
		}

		// Just in case if keyframe has both element types
		release_static(u"");

		if (element.id == UINT16_MAX) {
			return;
		}

		element.matrix = matrix;
		element.color = color;
		m_elements.push_back(element);
		m_keyframe_static_state = StaticElementsState::Invalid;
	}

	void FrameBuilder::ReleaseStatic(SymbolContext& symbol, const std::u16string& name)
	{
		if (m_static_elements.Empty()) return;

		uint16_t element_id = m_resources.AddGroup(symbol, m_static_elements);
		if (element_id == UINT16_MAX)
		{
			return;
		}

		FrameBuilderElement& element = m_elements.emplace_back();
		element.id = element_id;
		element.name = name;

		m_static_elements.Clear();
		m_static_state = StaticElementsState::None;
	}

	void FrameBuilder::InheritStatic(const FrameBuilder& frame)
	{
		m_static_elements += frame.StaticElements();
	}

	bool FrameBuilder::IsStatic() const
	{
		if (m_keyframe_static_state != StaticElementsState::Valid) return false;

		return true;
	}
}
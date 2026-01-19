#include "FrameBuilder.h"

#include "animate/publisher/ResourcePublisher.h"
#include "animate/publisher/wheelchair/AdobeWheelchair.h"

namespace Animate::Publisher
{
	std::u16string FrameBuilder::GetInstanceName(FCM::AutoPtr<DOM::FrameElement::ISymbolInstance> item)
	{
		AdobeWheelchair& wheelchair = AdobeWheelchair::Instance();
		if (!wheelchair.CPicObj_IsButton || !wheelchair.CPicSprite_GetName) return u"";
		uint16_t* instanceName = 0;

		std::uintptr_t page;
		{
			std::uintptr_t wrapper = (std::uintptr_t)item.m_Ptr;
			std::uintptr_t nativeWrapper = wrapper + 24;
			using GetObjectFN = void(__fastcall*)(std::uintptr_t, std::uintptr_t&);
			GetObjectFN getObject = *(GetObjectFN*)(*(std::uintptr_t*)(nativeWrapper)+32);
			getObject(nativeWrapper, page);
		}

		bool isButton;
		{
			using CPicObjIsButtonFN = bool(__fastcall*)(std::uintptr_t);
			CPicObjIsButtonFN CPicObj_IsButton = *(CPicObjIsButtonFN*)(*(std::uintptr_t*)(page)+wheelchair.CPicObj_IsButton);
			isButton = CPicObj_IsButton(page);
		}

		if (isButton) {
			throw FCM::FCMPluginException(FCM::FCMPluginException::Reason::SERVICE_FAIL);
		}
		else {
			// CPicSprite::GetName

			std::uintptr_t propertyInstance = page + wheelchair.CPicSprite_GetName;

			// CPropertyInstance::GetName

			std::uintptr_t instanceNameString = propertyInstance + 24;

			// Checking for SBO in CString aka std::basic_string<wchar_t,std::char_traits<wchar_t>,dvacore::allocator::STLAllocator<wchar_t>>
			size_t instanceNameLength = *(size_t*)(instanceNameString + 16);

			// Is local allocated
			if (instanceNameLength < 8) {
				instanceName = (uint16_t*)instanceNameString;
			}

			// Else take from heap
			else {
				instanceName = *(uint16_t**)instanceNameString;
			}
		}

		return std::u16string((const char16_t*)instanceName);
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

	void FrameBuilder::Update(SymbolContext& symbol, FCM::AutoPtr<DOM::ILayer2> layer, FCM::AutoPtr<DOM::IFrame> frame) {
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

				auto checkTweener = [&tweenerDict](FCM::String8Key key, FCM::FCMGUID& result)
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
			m_frame_layer = layer;
			FCM::FCMListPtr frameElements;
			frame->GetFrameElementsByType(DOM::FrameElement::IID_IFRAME_DISPLAY_ELEMENT, frameElements.m_Ptr);

			DeclareFrameElements(symbol, frameElements, std::nullopt, false);
		}

		m_rigging_frame = frame;
	}

	void FrameBuilder::ReleaseFrameElement(SymbolContext& symbol, SharedMovieclipWriter& writer, FrameBuilderElement& element)
	{
		if (m_frame_position > element.duration) return;

		FCM::AutoPtr<DOM::ILayer> rigging_layer = nullptr;
		FCM::BlendMode blend = element.blend_mode;
		std::optional<Matrix_t> matrix = element.matrix;
		std::optional<Color_t> color = element.color;

		FCM::BlendMode frame_blend;
		Color_t frame_color;
		m_frame_layer->GetColorTransformAtFrame(m_timeline_position, frame_color);
		m_frame_layer->GetBlendModeAtFrame(m_timeline_position, frame_blend);

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


		if (frame_color.Identity())
		{
			if (m_color_tweener) {
				color = DOM::Utils::COLOR_MATRIX();
				m_color_tweener->GetColorMatrix(m_base_tween, m_frame_position, *color);
			}
		}
		else
		{
			color = frame_color;
		}

		if (frame_blend != FCM::BlendMode::NORMAL)
		{
			blend = frame_blend;
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
			element.reference,
			blend,
			element.name,
			matrix,
			color
		);
	}

	void FrameBuilder::operator()(SymbolContext& symbol, SharedMovieclipWriter& writer) {
		uint32_t i = (uint32_t)m_elements.size();
		for (uint32_t elementIndex = 0; m_elements.size() > elementIndex; elementIndex++) {
			i--;

			ReleaseFrameElement(symbol, writer, m_elements[i]);
		}
	}

	void FrameBuilder::DeclareFrameElements(SymbolContext& symbol, FCM::FCMListPtr frameElements, std::optional<Matrix_t> base_transform, bool reverse) {
		uint32_t frameElementsCount = 0;
		frameElements->Count(frameElementsCount);
		m_elements.reserve(frameElementsCount);

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
		using namespace Animate::DOM::FrameElement;
		using namespace FCM;

		PluginModule& context = PluginModule::Instance();

		// Symbol info
		FrameBuilderElement element;

		// Transform
		Matrix_t matrix;
		frameElement->GetMatrix(matrix);

		if (base_transform)
		{
			matrix += *base_transform;
		}

		std::optional<Color_t> color;

		// Game "guess who i am"
		AutoPtr<IInstance> libraryElement = frameElement;
		AutoPtr<DOM::IFilterable> filterableElement = frameElement;
		AutoPtr<IClassicText> textfieldElement = frameElement;

		AutoPtr<IMovieClip> movieClipElement = frameElement;
		AutoPtr<ISymbolInstance> symbolItem = frameElement;
		AutoPtr<IShape> filledShapeItem = frameElement;
		AutoPtr<IGraphic> graphicItem = frameElement;
		AutoPtr<IGroup> groupedElemenets = frameElement;

		std::optional<FCMListPtr> filters;
		if (filterableElement)
		{
			filters = FCMListPtr();
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
			AutoPtr<DOM::ILibraryItem> libraryItem;
			libraryElement->GetLibraryItem(libraryItem.m_Ptr);
			AutoPtr<DOM::LibraryItem::IMediaItem> mediaItem = libraryItem;

			SymbolContext librarySymbol(libraryItem);

			if (symbolItem) {
				color = Color_t();
				symbolItem->GetColorMatrix(*color);

				if (!movieClipElement)
					element.name = GetInstanceName(symbolItem);
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
				AutoPtr<IFCMUnknown> mediaInfo;
				mediaItem->GetMediaInfo(mediaInfo.m_Ptr);

				AutoPtr<DOM::MediaInfo::IBitmapInfo> bitmapMedia = mediaInfo;
				m_static_elements.AddElement<BitmapElement>(symbol, mediaItem, matrix);
				if (!release_static(element.name))
				{
					m_static_state = StaticElementsState::Valid;
				}
					
				return;
			}

			m_static_state = FrameBuilder::StaticElementsState::Invalid;
			element.reference = m_resources.AddLibraryItem(librarySymbol, libraryItem, is_required());
		}

		// Textfield
		else if (textfieldElement) {
			m_static_state = FrameBuilder::StaticElementsState::Invalid;

			element.reference = m_resources.AddTextField(symbol, textfieldElement, filters);
			if (element.reference)
			{
				AutoPtr<ITextBehaviour> textfieldElementBehaviour;
				textfieldElement->GetTextBehaviour(textfieldElementBehaviour.m_Ptr);

				AutoPtr<IModifiableTextBehaviour> modifiableTextfieldBehaviour = textfieldElementBehaviour;
				if (modifiableTextfieldBehaviour) {
					element.name = context.falloc->GetString16(
						modifiableTextfieldBehaviour.m_Ptr,
						&IModifiableTextBehaviour::GetInstanceName
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
			FCMListPtr groupElements;
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

		if (!element.reference) {
			return;
		}

		element.matrix = matrix;
		element.color = color;

		// Looping params
		if (graphicItem)
		{
			AnimationLoopMode mode;
			graphicItem->GetLoopMode(mode);

			switch (mode)
			{
			case AnimationLoopMode::ANIMATION_PLAY_ONCE:
				if (libraryElement)
				{
					AutoPtr<DOM::ILibraryItem> libraryItem;
					libraryElement->GetLibraryItem(libraryItem.m_Ptr);
					AutoPtr<DOM::LibraryItem::ISymbolItem> symbolLibraryItem = libraryItem;
					if (symbolLibraryItem)
					{
						AutoPtr<DOM::ITimeline> itemTimeline;
						symbolLibraryItem->GetTimeLine(itemTimeline.m_Ptr);
						itemTimeline->GetMaxFrameCount(element.duration);
					}
				}
				break;

			case AnimationLoopMode::ANIMATION_LOOP:
			case AnimationLoopMode::ANIMATION_SINGLE_FRAME:
			default:
				break;
			}
		}

		m_elements.push_back(element);
		m_keyframe_static_state = StaticElementsState::Invalid;
	}

	void FrameBuilder::ReleaseStatic(SymbolContext& symbol, const std::u16string& name)
	{
		if (m_static_elements.Empty()) return;

		ResourceReference reference = m_resources.AddGroup(symbol, m_static_elements);
		if (!reference)
			return;

		FrameBuilderElement& element = m_elements.emplace_back();
		element.reference = reference;
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
		if (IsAnimated()) return false;

		return true;
	}

	bool FrameBuilder::IsAnimated() const
	{
		if (m_base_tween)
			return true;

		return false;
	}
}
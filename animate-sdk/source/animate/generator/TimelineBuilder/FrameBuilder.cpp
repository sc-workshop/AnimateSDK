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
			m_position = 0;
			m_label = u"";
			m_elements.clear();
			m_filled_elements.clear();

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
					FCM::StringRep16 frameNamePtr;
					frame->GetLabel(&frameNamePtr);
					m_label = (const char16_t*)frameNamePtr;
					context.falloc->Free(frameNamePtr);
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

				DeclareFrameElements(symbol, frameElements, std::nullopt, true);
			}
		}

		void FrameBuilder::ReleaseFrameElement(SymbolContext& symbol, SharedMovieclipWriter& writer, size_t index)
		{
			FrameBuilderElement& element = m_elements[index];

			std::optional<Matrix_t> matrix = element.matrix;
			std::optional<Color_t> color = element.color;

			if (matrix.has_value()) {
				if (m_matrix_tweener) {
					DOM::Utils::MATRIX2D transform_matrix;
					m_matrix_tweener->GetGeometricTransform(m_base_tween, m_position, transform_matrix);

					*matrix = *matrix * transform_matrix;
				}
			}
			else if (m_matrix_tweener) {
				matrix = DOM::Utils::MATRIX2D();
				m_matrix_tweener->GetGeometricTransform(m_base_tween, m_position, *matrix);
			}

			if (m_color_tweener) {
				color = DOM::Utils::COLOR_MATRIX();
				m_color_tweener->GetColorMatrix(m_base_tween, m_position, *color);
			}

			if (m_shape_tweener) {
				FCM::AutoPtr<DOM::FrameElement::IShape> filledShape = nullptr;
				m_shape_tweener->GetShape(m_base_tween, m_position, filledShape.m_Ptr);

				// TODO: this place may have many bugs and issues
				m_last_element = FrameBuilder::LastElementType::FilledElement;
				m_filled_elements.emplace_back(symbol, filledShape);

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

		// TODO: Implement FrameBuilderElement as basic class for FrameElements
		// And after that refactor GetIdentifer function to be called just in Release time
		// To avoid unused elements in resource palette
		void FrameBuilder::DeclareFrameElement(
			SymbolContext& symbol,
			FCM::AutoPtr<DOM::FrameElement::IFrameDisplayElement> frameElement,
			std::optional<Matrix_t> base_transform
		)
		{
			FCM::PluginModule& context = FCM::PluginModule::Instance();

			// Symbol info

			// TODO move to seperate class
			FrameBuilderElement element;

			// Transform
			Matrix_t matrix;
			frameElement->GetMatrix(matrix);

			if (base_transform)
			{
				matrix.a *= base_transform->a;
				matrix.b += base_transform->b;
				matrix.c += base_transform->c;
				matrix.d *= base_transform->d;
				matrix.tx += base_transform->tx;
				matrix.ty += base_transform->ty;
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

			auto is_required = [&element]()
			{
				return !element.name.empty();
			};

			// Symbol
			if (libraryElement) {
				m_last_element = FrameBuilder::LastElementType::Symbol;

				FCM::AutoPtr<DOM::ILibraryItem> libraryItem;
				libraryElement->GetLibraryItem(libraryItem.m_Ptr);

				SymbolContext librarySymbol(libraryItem);

				if (symbolItem) {
					color = Color_t();
					symbolItem->GetColorMatrix(*color);
					//element.name = Localization::ToUtf16(FrameBuilder::GetInstanceName(symbolItem));
				}

				if (movieClipElement) {
					// Instance name
					FCM::StringRep16 instanceNamePtr;
					movieClipElement->GetName(&instanceNamePtr);
					element.name = (const char16_t*)instanceNamePtr;
					context.falloc->Free(instanceNamePtr);

					movieClipElement->GetBlendMode(element.blend_mode);
				}

				element.id = m_resources.GetIdentifer(librarySymbol.name);
				if (element.id == UINT16_MAX) {
					element.id = m_resources.AddLibraryItem(librarySymbol, libraryItem, is_required());
				}
			}

			// Textfield
			else if (textfieldElement) {
				m_last_element = FrameBuilder::LastElementType::TextField;

				TextElement textfield;

				{
					frameElement->GetObjectSpaceBounds(textfield.bound);

					FCM::StringRep16 text;
					textfieldElement->GetText(&text);
					textfield.text = std::u16string((const char16_t*)text);
					context.falloc->Free(text);

					textfield.renderingMode.structSize = sizeof(textfield.renderingMode);
					textfieldElement->GetAntiAliasModeProp(textfield.renderingMode);

					FCM::AutoPtr<DOM::FrameElement::ITextBehaviour> textfieldElementBehaviour;
					textfieldElement->GetTextBehaviour(textfieldElementBehaviour.m_Ptr);

					textfieldElementBehaviour->IsSelectable(textfield.isSelectable);

					// Instance name

					FCM::AutoPtr<DOM::FrameElement::IModifiableTextBehaviour> modifiableTextfieldBehaviour = textfieldElementBehaviour;
					if (modifiableTextfieldBehaviour) {
						FCM::StringRep16 instanceName;
						modifiableTextfieldBehaviour->GetInstanceName(&instanceName);
						element.name = (const char16_t*)instanceName;
						context.falloc->Free(instanceName);

						modifiableTextfieldBehaviour->GetLineMode(textfield.lineMode);
					}

					// Textfields properties

					FCM::FCMListPtr paragraphs;
					uint32_t paragraphsCount = 0;
					textfieldElement->GetParagraphs(paragraphs.m_Ptr);
					paragraphs->Count(paragraphsCount);

					if (paragraphsCount == 0)
					{
						return;
					};

					if (paragraphsCount > 1) { // TODO: Add paragraph vector
						// context.Trace("Warning. Some of TextField has multiple paragraph");
					}

					FCM::AutoPtr<DOM::FrameElement::IParagraph> paragraph = paragraphs[0];
					textfield.style.structSize = sizeof(textfield.style);
					paragraph->GetParagraphStyle(textfield.style);

					FCM::FCMListPtr textRuns;
					uint32_t textRunCount = 0;
					paragraph->GetTextRuns(textRuns.m_Ptr);
					textRuns->Count(textRunCount);

					if (textRunCount == 0) {
						// context.Trace("Failed to get TextRun from TextField. Check logs for details.");
						// context.logger->error("TextField from {} does not have TextRuns", Localization::ToUtf8(symbol.name));
						return;
					}

					if (textRunCount > 1) {
						// context.Trace("Warning. Some of TextField has multiple textRun");
					}

					FCM::AutoPtr<DOM::FrameElement::ITextRun> textRun = textRuns[0];
					FCM::AutoPtr<DOM::FrameElement::ITextStyle> textStyle;
					textRun->GetTextStyle(textStyle.m_Ptr);

					textStyle->GetFontColor(textfield.fontColor);
					textStyle->GetFontSize(textfield.fontSize);
					textStyle->IsAutoKernEnabled(textfield.autoKern);

					FCM::StringRep16 fontNamePtr;
					textStyle->GetFontName(&fontNamePtr);
					textfield.fontName = std::u16string((const char16_t*)fontNamePtr);
					context.falloc->Free(fontNamePtr);

					FCM::StringRep8 fontStylePtr;
					textStyle->GetFontStyle(&fontStylePtr);
					textfield.fontStyle = std::string((const char*)fontStylePtr);
					context.falloc->Free(fontStylePtr);
				}

				// Textfield filters
				if (filterableElement) {
					FCM::FCMListPtr filters;
					filterableElement->GetGraphicFilters(filters.m_Ptr);
					uint32_t filterCount = 0;
					filters->Count(filterCount);

					for (uint32_t f = 0; filterCount > f; f++) {
						// And again game "guess who i am"
						FCM::AutoPtr<DOM::GraphicFilter::IGlowFilter> glowFilter = filters[f];

						if (glowFilter) {
							textfield.isOutlined = true;
							glowFilter->GetShadowColor(textfield.outlineColor);
						}
					}
				}

				element.id = m_resources.GetIdentifer(textfield);
				if (element.id == UINT16_MAX) {
					element.id = m_resources.AddTextField(symbol, textfield);
				}
			}

			// Fills / Stroke
			else if (filledShapeItem) {
				m_filled_elements.emplace_back(symbol, filledShapeItem, matrix);

				if (m_last_element != FrameBuilder::LastElementType::None)
				{
					if (m_last_element != FrameBuilder::LastElementType::FilledElement)
					{
						ReleaseFilledElements(symbol, element.name);
					}
				}

				m_last_element = FrameBuilder::LastElementType::FilledElement;
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

			if (m_last_element != LastElementType::None)
			{
				// Just in case if keyframe has both element types
				if (m_last_element != LastElementType::FilledElement)
				{
					ReleaseFilledElements(symbol, std::u16string(u""));
				}
			}

			if (element.id == UINT16_MAX) {
				return;
			}

			element.matrix = matrix;
			element.color = color;
			m_elements.push_back(element);
		}

		void FrameBuilder::ReleaseFilledElements(SymbolContext& symbol, const std::u16string& name)
		{
			if (m_filled_elements.empty()) return;

			uint16_t element_id = m_resources.GetIdentifer(m_filled_elements);

			if (element_id == UINT16_MAX)
			{
				element_id = m_resources.AddFilledElement(symbol, m_filled_elements);
			}

			if (element_id == UINT16_MAX)
			{
				return;
			}

			FrameBuilderElement& element = m_elements.emplace_back();
			element.id = element_id;
			element.name = name;
		}

		void FrameBuilder::InheritFilledElements(const FrameBuilder& frame)
		{
			m_filled_elements.insert(
				m_filled_elements.begin(),
				frame.FilledElements().begin(), frame.FilledElements().end()
			);
		}
}
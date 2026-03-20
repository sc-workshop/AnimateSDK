#include "FrameBuilder.h"

#include "LayerBuilder.h"
#include "animate/publisher/ResourcePublisher.h"
#include "animate/publisher/wheelchair/AdobeWheelchair.h"

namespace Animate::Publisher {
    std::u16string FrameBuilder::GetInstanceName(FCM::AutoPtr<DOM::FrameElement::ISymbolInstance> item) {
        AdobeWheelchair& wheelchair = AdobeWheelchair::Instance();
        if (!wheelchair.CPicObj_IsButton || !wheelchair.CPicSprite_GetName)
            return u"";
        const char16_t* instanceName = 0;

        std::uintptr_t page;
        {
            std::uintptr_t wrapper = (std::uintptr_t) item.m_Ptr;
            std::uintptr_t nativeWrapper = wrapper + 24;
            using GetObjectFN = void (*)(std::uintptr_t, std::uintptr_t&);
            GetObjectFN getObject = *(GetObjectFN*) (*(std::uintptr_t*) (nativeWrapper) + 32);
            getObject(nativeWrapper, page);
        }

        bool isButton;
        {
            using CPicObjIsButtonFN = bool (*)(std::uintptr_t);
            CPicObjIsButtonFN CPicObj_IsButton = *(CPicObjIsButtonFN*) (*(std::uintptr_t*) (page) + wheelchair.CPicObj_IsButton);
            isButton = CPicObj_IsButton(page);
        }

        if (isButton) {
            throw FCM::FCMPluginException(FCM::FCMPluginException::Reason::SERVICE_FAIL);
        } else {
            // CPicSprite::GetName

            std::uintptr_t propertyInstance = page + wheelchair.CPicSprite_GetName;

            // CPropertyInstance::GetName

            // CString aka
            // std::basic_string<wchar_t,std::char_traits<wchar_t>,dvacore::allocator::STLAllocator<wchar_t>>*
            std::uintptr_t instanceNameString = propertyInstance + 24;

#if defined(_WINDOWS)
            // Checking for SBO
            size_t instanceNameLength = *(size_t*) (instanceNameString + 16);

            // Is local allocated
            if (instanceNameLength < 8) {
                instanceName = (const char16_t*) instanceNameString;
            }

            // Else take from heap
            else {
                instanceName = *(const char16_t**) instanceNameString;
            }

#elif defined(__APPLE__)
            uint8_t flags = *(uint8_t*) (instanceNameString);

            // size_t instanceNameLength = 0;
            if (flags & 1) // long string
            {
                const uint64_t* fields = reinterpret_cast<const uint64_t*>(instanceNameString);
                // instanceNameLength = (size_t)fields[1];                      // size field
                instanceName = reinterpret_cast<const char16_t*>(fields[2]); // data field
            } else                                                           // short string
            {
                // instanceNameLength = flags >> 1; // size stored in upper 7 bits
                instanceName = reinterpret_cast<const char16_t*>((uint8_t*) instanceNameString + 2);
            }
#endif
        }

        return std::u16string((const char16_t*) instanceName);
    }

    void FrameBuilder::Reset() {
        m_duration = 0;
        m_label.clear();
        m_elements.clear();
        m_static_elements.Clear();

        m_base_tween = nullptr;
        m_matrix_tweener = nullptr;
        m_color_tweener = nullptr;
        m_shape_tweener = nullptr;
    }

    void FrameBuilder::Update(FCM::AutoPtr<DOM::ILayer2> layer, FCM::AutoPtr<DOM::IFrame> frame, uint32_t offset) {
        FCM::PluginModule& context = FCM::PluginModule::Instance();
        Reset();

        m_position = offset;

        // Frame Label
        {
            DOM::KeyFrameLabelType labelType = DOM::KeyFrameLabelType::KEY_FRAME_LABEL_NONE;
            frame->GetLabelType(labelType);
            if (labelType == DOM::KeyFrameLabelType::KEY_FRAME_LABEL_NAME) {
                m_label = context.falloc->GetString16(frame.m_Ptr, &DOM::IFrame::GetLabel);
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

                auto checkTweener = [&tweenerDict](FCM::String8Key key, FCM::FCMGUID& result) {
                    FCM::U_Int32 valueLen;
                    FCM::FCMDictRecTypeID type;

                    FCM::Result res = tweenerDict->GetInfo(key, type, valueLen);
                    if (FCM_FAILURE_CODE(res)) {
                        return false;
                    }

                    res = tweenerDict->Get(key, type, (FCM::PVoid) &result, valueLen);
                    if (FCM_FAILURE_CODE(res)) {
                        return false;
                    }

                    return true;
                };

                bool hasMatrixTweener = checkTweener(DOM::Service::Tween::kDOMGeometricProperty, matrixGuid);
                bool hasColorTweener = checkTweener(DOM::Service::Tween::kDOMColorProperty, colorGuid);
                bool hasShapeTweener = checkTweener(DOM::Service::Tween::kDOMShapeProperty, shapeGuid);

                FCM::AutoPtr<DOM::Service::Tween::ITweenerService> TweenerService =
                    context.GetService<DOM::Service::Tween::ITweenerService>(DOM::Service::Tween::TWEENER_SERVICE);

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

        m_layer = layer;
        m_frame = frame;

        UpdateFrameElements(offset);
        UpdateShapeTweener();
    }

    void FrameBuilder::UpdateFrameElements(uint32_t offset) {
        m_frame_elements = FCM::FCMListPtr();
        m_frame->GetFrameElementsByType(DOM::FrameElement::IID_IFRAME_DISPLAY_ELEMENT, m_frame_elements.m_Ptr);

        DeclareFrameElements(m_frame_elements, std::nullopt, false, offset);
    }

    void FrameBuilder::UpdateShapeTweener() {
        if (!m_shape_tweener)
            return;

        m_static_elements.Clear();
        FCM::AutoPtr<DOM::FrameElement::IShape> filledShape = nullptr;
        m_shape_tweener->GetShape(m_base_tween, m_position, filledShape.m_Ptr);

        if (filledShape) {
            // Remove all previous elements
            m_elements.clear();

            // Update states and add filled shape to queue
            m_static_state = FrameBuilder::StaticElementsState::Valid;
            m_static_elements.AddFilledElement(m_symbol, filledShape);
            m_builder.frameUpdated = true;
        }
    }

    void FrameBuilder::ReleaseFrameElement(SharedMovieclipWriter& writer, FrameBuilderElement& element) {
        if (m_position > element.duration)
            return;

        FCM::AutoPtr<DOM::ILayer> rigging_layer = nullptr;
        FCM::BlendMode blend = element.blend_mode;
        std::optional<Matrix_t> matrix = element.matrix;
        std::optional<Color_t> color = element.color;

        FCM::BlendMode frame_blend;
        Color_t frame_color;
        m_layer->GetColorTransformAtFrame(m_builder.iterator, frame_color);
        m_layer->GetBlendModeAtFrame(m_builder.iterator, frame_blend);

        if (matrix.has_value()) {
            if (m_matrix_tweener) {
                DOM::Utils::MATRIX2D transform_matrix;
                m_matrix_tweener->GetGeometricTransform(m_base_tween, m_position, transform_matrix);

                *matrix = *matrix * transform_matrix;
            }
        } else if (m_matrix_tweener) {
            matrix = DOM::Utils::MATRIX2D();
            m_matrix_tweener->GetGeometricTransform(m_base_tween, m_position, *matrix);
        }

        if (m_frame) {
            m_frame->GetRigParent(m_position, rigging_layer.m_Ptr);
        }

        if (rigging_layer) {
            DOM::Utils::RIG_PROPERTIES props;
            uint32_t position = m_builder.iterator;
            m_frame->GetRigProperties(position, props);
            matrix = *matrix * props.matrix;
        }

        if (frame_color.Identity()) {
            if (m_color_tweener) {
                color = DOM::Utils::COLOR_MATRIX();
                m_color_tweener->GetColorMatrix(m_base_tween, m_position, *color);
            }
        } else {
            color = frame_color;
        }

        if (frame_blend != FCM::BlendMode::NORMAL) {
            blend = frame_blend;
        }

        if (element.iterator.has_value()) {
            ++(*element.iterator);
        }

        writer.AddFrameElement(element.reference, blend, element.name, matrix, color);
    }

    void FrameBuilder::operator()(SharedMovieclipWriter& writer) {
        uint32_t i = (uint32_t) m_elements.size();
        for (uint32_t elementIndex = 0; m_elements.size() > elementIndex; elementIndex++) {
            i--;

            FrameBuilderElement& element = m_elements[i];

            // Check if symbol has local iterator for looping properties
            if (element.iterator.has_value() && element.iterator->Static()) {
                // Create single frame copy of movieclip
                DeclareFrameElement(m_frame_elements[elementIndex], element, std::nullopt, true);

                // Reset iterator
                element.iterator = std::nullopt;
            }

            if (!element.reference)
                continue;

            // Release element to writer
            ReleaseFrameElement(writer, element);
        }
    }

    void FrameBuilder::Next() {
        m_position++;
        m_static_elements.Update();
        UpdateShapeTweener();
    }

    void FrameBuilder::InvalidateStaticState() {
        m_static_state = StaticElementsState::Invalid;
        m_static_elements.Clear();
    }

    void FrameBuilder::DeclareFrameElements(FCM::FCMListPtr frameElements, std::optional<Matrix_t> base_transform, bool reverse, uint32_t offset) {
        uint32_t frameElementsCount = 0;
        frameElements->Count(frameElementsCount);
        m_elements.reserve(frameElementsCount);

        auto addElement = [&](uint32_t index) {
            FrameBuilderElement element;
            DeclareFrameElement(frameElements[index], element, base_transform, false, offset);

            if (element.reference)
                m_elements.push_back(element);
        };

        if (reverse) {
            uint32_t i = frameElementsCount - 1;
            for (uint32_t t = 0; frameElementsCount > t; t++) {
                addElement(i--);
            }
        } else {
            for (uint32_t i = 0; frameElementsCount > i; i++) {
                addElement(i);
            }
        }
    }

    void FrameBuilder::DeclareFrameElement(FCM::AutoPtr<DOM::FrameElement::IFrameDisplayElement> frameElement,
                                           FrameBuilderElement& element,
                                           std::optional<Matrix_t> base_transform,
                                           bool singleFrame,
                                           uint32_t offset) {
        using namespace Animate::DOM::FrameElement;
        using namespace FCM;

        PluginModule& context = PluginModule::Instance();

        // Transform
        Matrix_t matrix;
        frameElement->GetMatrix(matrix);

        if (base_transform) {
            matrix += *base_transform;
        }

        std::optional<Color_t> color;

        // Game "guess who i am"
        AutoPtr<IInstance> libraryElement = frameElement;
        AutoPtr<DOM::IFilterable> filterableElement = frameElement;
        AutoPtr<IClassicText> textfieldElement = frameElement;

        AutoPtr<IMovieClip> movieClipElement = frameElement;
        AutoPtr<ISymbolInstance> symbolInstanceItem = frameElement;
        AutoPtr<IShape> filledShapeItem = frameElement;
        AutoPtr<IGraphic> graphicItem = frameElement;
        AutoPtr<IGroup> groupedElemenets = frameElement;

        std::optional<FCMListPtr> filters;
        if (filterableElement) {
            filters = FCMListPtr();
            filterableElement->GetGraphicFilters(filters->m_Ptr);
        }

        auto is_required = [&element]() { return !element.name.empty(); };

        auto release_static = [&](const std::u16string& name) {
            if (m_static_state == FrameBuilder::StaticElementsState::Invalid) {
                ReleaseStatic(name);
                InvalidateStaticState();
                return true;
            }

            return false;
        };

        // Symbol
        if (libraryElement) {
            AutoPtr<DOM::ILibraryItem> libraryItem;
            libraryElement->GetLibraryItem(libraryItem.m_Ptr);

            AutoPtr<DOM::LibraryItem::IMediaItem> mediaItem = libraryItem;
            AutoPtr<DOM::LibraryItem::ISymbolItem> symbolItem = libraryItem;

            SymbolContext librarySymbol(libraryItem);

            if (symbolInstanceItem) {
                color = Color_t();
                symbolInstanceItem->GetColorMatrix(*color);

                if (!movieClipElement)
                    element.name = GetInstanceName(symbolInstanceItem);
            }

            if (movieClipElement) {
                // Instance name
                element.name = context.falloc->GetString16(movieClipElement.m_Ptr, &DOM::FrameElement::IMovieClip::GetName);

                movieClipElement->GetBlendMode(element.blend_mode);
            }

            // Looping params
            LoopingContext::LoopMode loopMode = LoopingContext::LoopMode::ANIMATION_LOOP;
            bool hasLoopMode = false;
            bool singleFrameLoop = false;
            if (graphicItem) {
                librarySymbol.looping = LoopingContext(graphicItem);
                bool hasEndFrame = librarySymbol.looping.GetEndFrame() != 0xFFFFFFFF;
                loopMode = librarySymbol.looping.GetMode();
                singleFrameLoop = loopMode == LoopingContext::LoopMode::ANIMATION_SINGLE_FRAME;
                hasLoopMode = loopMode != LoopingContext::LoopMode::ANIMATION_LOOP;

                // Set our own end frame to export less frames if user has not already set this
                if (!hasEndFrame) {
                    librarySymbol.looping =
                        LoopingContext(librarySymbol.looping.GetMode(), librarySymbol.looping.GetStartFrame(), librarySymbol.looping.GetStartFrame() + m_duration);
                }
            }

            if (mediaItem) {
                AutoPtr<IFCMUnknown> mediaInfo;
                mediaItem->GetMediaInfo(mediaInfo.m_Ptr);

                AutoPtr<DOM::MediaInfo::IBitmapInfo> bitmapMedia = mediaInfo;
                m_static_elements.AddElement<BitmapElement>(m_symbol, mediaItem, matrix);
                if (!release_static(element.name)) {
                    m_static_state = StaticElementsState::Valid;
                }

                return;
            }

            // Post-processing looping properties
            if (element.iterator.has_value() && singleFrame) {
                // Set single frame looping if requested single frame copy
                librarySymbol.looping =
                    LoopingContext(LoopingContext::LoopMode::ANIMATION_PLAY_ONCE, element.iterator->CurrentFrame() - 1, element.iterator->CurrentFrame());

            } else if (hasLoopMode) {
                uint32_t symbolDuration = 0;

                FCM::AutoPtr<DOM::ITimeline> timeline;
                symbolItem->GetTimeLine(timeline.m_Ptr);

                timeline->GetMaxFrameCount(symbolDuration);

                element.iterator = FrameIterator(librarySymbol.looping, symbolDuration);
            }

            // Handling nested looping properties
            if (offset != 0) {
                if (!singleFrameLoop) {
                    librarySymbol.looping =
                        LoopingContext(librarySymbol.looping.GetMode(), librarySymbol.looping.GetStartFrame() + offset, librarySymbol.looping.GetEndFrame());
                }
            }

            InvalidateStaticState();
            element.reference = m_resources.AddLibraryItem(librarySymbol, libraryItem, is_required());
        }

        // Textfield
        else if (textfieldElement) {
            InvalidateStaticState();

            element.reference = m_resources.AddTextField(m_symbol, textfieldElement, filters);
            if (element.reference) {
                AutoPtr<ITextBehaviour> textfieldElementBehaviour;
                textfieldElement->GetTextBehaviour(textfieldElementBehaviour.m_Ptr);

                AutoPtr<IModifiableTextBehaviour> modifiableTextfieldBehaviour = textfieldElementBehaviour;
                if (modifiableTextfieldBehaviour) {
                    element.name = context.falloc->GetString16(modifiableTextfieldBehaviour.m_Ptr, &IModifiableTextBehaviour::GetInstanceName);
                }
            }
        }

        // Fills / Stroke
        else if (filledShapeItem) {
            m_static_elements.AddFilledElement(m_symbol, filledShapeItem, matrix);

            if (!release_static(element.name)) {
                m_static_state = StaticElementsState::Valid;
            }
            return;
        }

        // Groups
        else if (groupedElemenets) {
            FCMListPtr groupElements;
            groupedElemenets->GetMembers(groupElements.m_Ptr);

            DeclareFrameElements(groupElements, matrix, true);
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
        m_keyframe_static_state = StaticElementsState::Invalid;
    }

    bool FrameBuilder::operator==(const FrameBuilder& builder) const {
        Color_t color_a;
        m_layer->GetColorTransformAtFrame(m_builder.iterator, color_a);

        Color_t color_b;
        builder.m_layer->GetColorTransformAtFrame(m_builder.iterator, color_b);

        // Comparing color transforms of frames
        constexpr size_t colorMatrixSize = sizeof(color_a.colorArray) / sizeof(FCM::Float);
        for (size_t i = 0; colorMatrixSize > i; i++) {
            if (color_a.colorArray[i] != color_b.colorArray[i])
                return false;
        }

        // Comparing frame blend modes
        FCM::BlendMode blend_a;
        m_layer->GetBlendModeAtFrame(m_builder.iterator, blend_a);

        FCM::BlendMode blend_b;
        builder.m_layer->GetBlendModeAtFrame(m_builder.iterator, blend_b);
        if (blend_a != blend_b)
            return false;

        return true;
    }

    void FrameBuilder::ReleaseStatic(const std::u16string& name) {
        if (m_static_elements.Empty())
            return;

        ResourceReference reference = m_resources.AddGroup(m_symbol, m_static_elements);
        if (!reference)
            return;

        auto existing = std::find_if(m_elements.begin(), m_elements.end(), [&reference, &name](const FrameBuilderElement& element) {
            return element.reference == reference && element.name == name && element.isVector;
        });

        // Add only if there is no such element already
        if (existing == m_elements.end()) {
            FrameBuilderElement& element = m_elements.emplace_back();
            element.reference = reference;
            element.name = name;
            element.isVector = true;
        }
    }

    void FrameBuilder::InheritStatic(const FrameBuilder& frame) {
        m_static_elements += frame.StaticElements();

        // Remove all already inherited elements
        std::erase_if(m_elements, [](const FrameBuilderElement& element) { return element.isVector; });
    }

    bool FrameBuilder::IsStatic() const {
        if (m_keyframe_static_state != StaticElementsState::Valid)
            return false;

        if (IsAnimated())
            return false;

        return true;
    }

    bool FrameBuilder::IsAnimated() const {
        // Checking for transformation tween
        if (m_color_tweener || m_matrix_tweener)
            return true;

        return false;
    }
}

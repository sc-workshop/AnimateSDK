#pragma once

#include "AnimateDOM.h"
#include "AnimateService.h"
#include "AnimateWriter.h"
#include "FrameElements/FilledElement.h"
#include "FrameElements/StaticElementsGroup.h"
#include "FrameIterator.h"
#include "animate/publisher/symbol/LoopingContext.h"
#include "animate/publisher/symbol/SymbolContext.h"

namespace Animate::Publisher {
    class ResourcePublisher;
    struct LayerBuilderContext;

    using Matrix_t = DOM::Utils::MATRIX2D;
    using Color_t = DOM::Utils::COLOR_MATRIX;

    struct FrameBuilderElement {
        // Element id
        ResourceReference reference;

        // Local symbol iterator
        std::optional<FrameIterator> iterator;

        // Element blending
        FCM::BlendMode blend_mode = FCM::BlendMode::NORMAL;

        // Element instance name
        std::u16string name;

        // Optional basic transforms for each frame element
        std::optional<Matrix_t> matrix = std::nullopt;
        std::optional<Color_t> color = std::nullopt;

        // Count of playing frames
        uint32_t duration = std::numeric_limits<uint32_t>::max();

        // Is vector fill or stroke shape
        bool isVector : 1 = false;

        // Is single frame copy of symbol
        bool isStatic : 1 = false;
    };

    class FrameBuilder {
    public:
        enum class StaticElementsState {
            None = 0,
            Invalid,
            Valid,
        };

    public:
        // A biiig workaround with magic numbers to get instance name from Graphic items
        static std::u16string GetInstanceName(FCM::AutoPtr<DOM::FrameElement::ISymbolInstance> symbol);

    private:
        SymbolContext& m_symbol;
        LayerBuilderContext& m_builder;
        ResourcePublisher& m_resources;

        // Frame layer
        FCM::AutoPtr<DOM::ILayer2> m_layer;

        // Current keyframe duration
        uint32_t m_duration = 0;

        // Current keyframe position [0 - m_duration]
        uint32_t m_position = 0;

        // Frame label
        std::u16string m_label;

        // Frame elements
        std::vector<FrameBuilderElement> m_elements;

        // Tweeners
        FCM::AutoPtr<DOM::ITween> m_base_tween = nullptr;

        FCM::AutoPtr<DOM::Service::Tween::IGeometricTweener> m_matrix_tweener = nullptr;
        FCM::AutoPtr<DOM::Service::Tween::IColorTweener> m_color_tweener = nullptr;
        FCM::AutoPtr<DOM::Service::Tween::IShapeTweener> m_shape_tweener = nullptr;

        // Current frame
        // Mostly for child layers feature
        FCM::AutoPtr<DOM::IFrame1> m_frame = nullptr;

        // Array of raw frame elements wrappers
        FCM::FCMListPtr m_frame_elements;

        // Graphic Batching state for filled shapes and sprites
        StaticElementsState m_static_state = StaticElementsState::None;           // State of current frame
        StaticElementsState m_keyframe_static_state = StaticElementsState::Valid; // State of whole keyframe for early out
        StaticElementsGroup m_static_elements;                                    // Array of sprites and filled shapes gathered for batch process

    public:
        FrameBuilder(SymbolContext& symbol, LayerBuilderContext& builder, ResourcePublisher& resources) :
            m_resources(resources),
            m_symbol(symbol),
            m_builder(builder) {};

        void Update(FCM::AutoPtr<DOM::ILayer2> layer, FCM::AutoPtr<DOM::IFrame> frame, uint32_t offset = 0);

        void UpdateFrameElements(uint32_t offset = 0);

        void UpdateShapeTweener();

        void ReleaseFrameElement(SharedMovieclipWriter& writer, FrameBuilderElement& element);

        void operator()(SharedMovieclipWriter& writer);

        bool FlushMask() const { return !m_elements.empty() || !m_static_elements.Empty(); }

        void Next();

        uint32_t Duration() const { return m_duration; }

        uint32_t Position() const { return m_position; }

        StaticElementsState StaticElementsState() const { return m_static_state; }

        operator bool() const { return m_duration > m_position; }

        bool operator==(const FrameBuilder& builder) const;

        const StaticElementsGroup& StaticElements() const { return m_static_elements; }

        void ApplyName(SharedMovieclipWriter& writer) const {
            if (!m_label.empty()) {
                writer.SetLabel(m_label);
            }
        }

        void ReleaseStatic(const std::u16string& name);
        void InheritStatic(const FrameBuilder& frame);

        bool IsStatic() const;
        bool IsAnimated() const;

        void Reset();

    private:
        void InvalidateStaticState();

        void DeclareFrameElements(FCM::FCMListPtr frameElements,
                                  std::optional<Matrix_t> base_transform = std::nullopt,
                                  bool reverse = false,
                                  uint32_t offset = 0);

        void DeclareFrameElement(FCM::AutoPtr<DOM::FrameElement::IFrameDisplayElement> frameElement,
                                 FrameBuilderElement& element,
                                 std::optional<Matrix_t> base_transform = std::nullopt,
                                 bool singleFrame = false,
                                 uint32_t offset = 0);
    };
}
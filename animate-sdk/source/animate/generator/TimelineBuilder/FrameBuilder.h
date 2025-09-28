#pragma once

#include "AnimateDOM.h"
#include "AnimateWriter.h"
#include "AnimateService.h"

#include "FrameElements/FilledElement.h"
#include "FrameElements/StaticElementsGroup.h"

namespace Animate::Publisher
{
	class ResourcePublisher;

	using Matrix_t = DOM::Utils::MATRIX2D;
	using Color_t = DOM::Utils::COLOR_MATRIX;

	struct FrameBuilderElement
	{
	public:
		// Elements data
		uint16_t id = std::numeric_limits<uint16_t>::max();
		FCM::BlendMode blend_mode = FCM::BlendMode::NORMAL;
		std::u16string name;

		// Optional basic transforms for each frame element
		std::optional<Matrix_t> matrix = std::nullopt;
		std::optional<Color_t> color = std::nullopt;

		// Count of playing frames
		uint32_t duration = std::numeric_limits<uint32_t>::max();
	};

	class FrameBuilder {
	public:
		enum class StaticElementsState
		{
			None = 0,
			Invalid,
			Valid,
		};

	public:
		// A biiig workaround with magic numbers to get instance name from Graphic items
		static std::u16string GetInstanceName(FCM::AutoPtr<DOM::FrameElement::ISymbolInstance> symbol);

	private:
		ResourcePublisher& m_resources;

		// Basic frame data
		FCM::AutoPtr<DOM::ILayer2> m_frame_layer;
		uint32_t m_duration = 0;
		uint32_t m_frame_position = 0;
		uint32_t m_timeline_position = 0;
		std::u16string m_label;

		// Frame elements
		std::vector<FrameBuilderElement> m_elements;

		// Tweeners
		FCM::AutoPtr<DOM::ITween> m_base_tween = nullptr;

		FCM::AutoPtr<DOM::Service::Tween::IGeometricTweener> m_matrix_tweener = nullptr;
		FCM::AutoPtr<DOM::Service::Tween::IColorTweener> m_color_tweener = nullptr;
		FCM::AutoPtr<DOM::Service::Tween::IShapeTweener> m_shape_tweener = nullptr;

		// Rigging
		FCM::AutoPtr<DOM::IFrame1> m_rigging_frame = nullptr;

		// Filled elements "Static Batching" things
		StaticElementsState m_static_state = StaticElementsState::None;
		StaticElementsState m_keyframe_static_state = StaticElementsState::Valid;
		StaticElementsGroup m_static_elements;

	public:
		FrameBuilder(ResourcePublisher& resources) : m_resources(resources) { };

		void Update(SymbolContext& symbol, FCM::AutoPtr<DOM::ILayer2> layer, FCM::AutoPtr<DOM::IFrame> frame);

		void ReleaseFrameElement(SymbolContext& symbol, SharedMovieclipWriter& writer, FrameBuilderElement& element);

		void operator()(SymbolContext& symbol, SharedMovieclipWriter& writer);

		bool FlushMask() const
		{
			return !m_elements.empty() || !m_static_elements.Empty();
		}

		void Next()
		{
			m_frame_position++;
			m_timeline_position++;
		}

		uint32_t Duration() const
		{
			return m_duration;
		}

		uint32_t Position() const
		{
			return m_frame_position;
		}

		StaticElementsState StaticElementsState() const
		{
			return m_static_state;
		}

		operator bool() const
		{
			return m_duration > m_frame_position;
		}

		const StaticElementsGroup& StaticElements() const
		{
			return m_static_elements;
		}

		void ApplyName(SharedMovieclipWriter& writer) const
		{
			if (!m_label.empty()) {
				writer.SetLabel(m_label);
			}
		}

		void ReleaseStatic(SymbolContext& symbol, const std::u16string& name);
		void InheritStatic(const FrameBuilder& frame);

		bool IsStatic() const;

		void Reset();

	private:
		void DeclareFrameElements(
			SymbolContext& symbol,
			FCM::FCMListPtr frameElements,
			std::optional<Matrix_t> base_transform = std::nullopt,
			bool reverse = false
		);

		void DeclareFrameElement(
			SymbolContext& symbol,
			FCM::AutoPtr<DOM::FrameElement::IFrameDisplayElement> frameElement,
			std::optional<Matrix_t> base_transform = std::nullopt
		);
	};
}
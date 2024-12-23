#pragma once

#include "AnimateDOM.h"
#include "AnimateWriter.h"
#include "AnimateService.h"

#include "FrameElements/FilledElement.h"

namespace Animate::Publisher
{
	class ResourcePublisher;

	using Matrix_t = DOM::Utils::MATRIX2D;
	using Color_t = DOM::Utils::COLOR_MATRIX;

	struct FrameBuilderElement
	{
	public:
		// Elements data
		uint16_t id = UINT16_MAX;
		FCM::BlendMode blend_mode = FCM::BlendMode::NORMAL;
		std::u16string name;

		// Optional basic transforms for each frame element
		std::optional<Matrix_t> matrix = std::nullopt;
		std::optional<Color_t> color = std::nullopt;
	};

	class FrameBuilder {
	public:
		enum class LastElementType
		{
			None = 0,
			Symbol,
			TextField,
			FilledElement,
			SpriteElement
		};

	public:
		// A biiig workaround with magic numbers to get instance name from Graphic items
		// TODO: implement this workaround finally
		static std::string GetInstanceName(FCM::AutoPtr<DOM::FrameElement::ISymbolInstance> symbol);

	private:
		ResourcePublisher& m_resources;

		// Basic frame data
		uint32_t m_duration = 0;
		uint32_t m_position = 0;
		std::u16string m_label;

		// Frame elements
		std::vector<FrameBuilderElement> m_elements;

		// Tweeners
		FCM::AutoPtr<DOM::ITween> m_base_tween = nullptr;

		FCM::AutoPtr<DOM::Service::Tween::IGeometricTweener> m_matrix_tweener = nullptr;
		FCM::AutoPtr<DOM::Service::Tween::IColorTweener> m_color_tweener = nullptr;
		FCM::AutoPtr<DOM::Service::Tween::IShapeTweener> m_shape_tweener = nullptr;

		// Filled elements "Static Batching" things
		LastElementType m_last_element = LastElementType::None;
		std::vector<FilledElement> m_filled_elements;

	public:
		FrameBuilder(ResourcePublisher& resources) : m_resources(resources) { };

		void Update(SymbolContext& symbol, FCM::AutoPtr<DOM::IFrame> frame);

		void ReleaseFrameElement(SymbolContext& symbol, SharedMovieclipWriter& writer, size_t index);

		void operator()(SymbolContext& symbol, SharedMovieclipWriter& writer);

		bool FlushMask() const
		{
			return !m_elements.empty() || !m_filled_elements.empty();
		}

		void Next()
		{
			m_position++;
		}

		uint32_t Duration() const
		{
			return m_duration;
		}

		uint32_t Position() const
		{
			return m_position;
		}

		LastElementType LastElementType() const
		{
			return m_last_element;
		}

		operator bool() const
		{
			return m_duration > m_position;
		}

		const std::vector<FilledElement>& FilledElements() const
		{
			return m_filled_elements;
		}

		void ApplyName(SharedMovieclipWriter& writer) const
		{
			if (!m_label.empty()) {
				writer.SetLabel(m_label);
			}
		}

		void ReleaseFilledElements(SymbolContext& symbol, const std::u16string& name);
		void InheritFilledElements(const FrameBuilder& frame);

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
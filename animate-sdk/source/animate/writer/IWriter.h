#pragma once

#include "IGraphicWriter.h"
#include "IMovieclipWriter.h"
#include "ITextFieldWriter.h"

#include "core/memory/ref.h"

namespace Animate::Publisher
{
	class SymbolContext;

	enum class MaskedLayerState : uint8_t
	{
		MASK_LAYER = 0,
		MASKED_LAYERS,
		MASKED_LAYERS_END
	};

	class SharedWriter {
	public:
		virtual ~SharedWriter() = default;

	public:
		virtual wk::Ref<SharedMovieclipWriter> AddMovieclip(SymbolContext& symbol) = 0;
		virtual wk::Ref<SharedShapeWriter> AddShape(SymbolContext& symbol) = 0;
		virtual wk::Ref<SharedTextFieldWriter> AddTextField(SymbolContext& symbol) = 0;

		virtual void AddModifier(uint16_t id, MaskedLayerState type) = 0;

		virtual void Finalize() = 0;
	};
}
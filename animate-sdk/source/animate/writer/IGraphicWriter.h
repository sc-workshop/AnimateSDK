#pragma once

#include <memory>
#include <vector>

#include "AnimateCore.h"
#include "AnimateDOM.h"

#include "IDisplayObjectWriter.h"
#include "animate/generator/TimelineBuilder/FrameElements/SpriteElement.h"
#include "animate/generator/TimelineBuilder/FrameElements/FilledElement.h"

namespace Animate::Publisher
{
	class SharedShapeWriter : public IDisplayObjectWriter {
	public:
		SharedShapeWriter(SymbolContext& context) : IDisplayObjectWriter(context) {};
		virtual ~SharedShapeWriter() = default;

	public:
		virtual void AddGraphic(const SpriteElement& item, const DOM::Utils::MATRIX2D& matrix) = 0;

		virtual void AddFilledElement(const FilledElement& shape) = 0;

		virtual void AddSlicedElements(const std::vector<FilledElement>& elements, const DOM::Utils::RECT& guides) = 0;
	};
}
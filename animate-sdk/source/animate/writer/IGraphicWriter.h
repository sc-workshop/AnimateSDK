#pragma once

#include <memory>
#include <vector>

#include "AnimateCore.h"
#include "AnimateDOM.h"

#include "IDisplayObjectWriter.h"
#include "animate/generator/TimelineBuilder/FrameElements/SpriteElement.h"
#include "animate/generator/TimelineBuilder/FrameElements/FilledElement.h"
#include "animate/generator/TimelineBuilder/FrameElements/SliceElement.h"

namespace Animate::Publisher
{
	class SharedShapeWriter : public IDisplayObjectWriter {
	public:
		SharedShapeWriter(SymbolContext& context) : IDisplayObjectWriter(context) {};
		virtual ~SharedShapeWriter() = default;

	public:
		virtual void AddGraphic(const BitmapElement& item) = 0;

		virtual void AddFilledElement(const FilledElement& shape) = 0;

		virtual void AddSlicedElements(const Slice9Element& sliced) = 0;
	};
}
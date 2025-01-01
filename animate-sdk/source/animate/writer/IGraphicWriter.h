#pragma once

#include <memory>
#include <vector>

#include "AnimateCore.h"
#include "AnimateDOM.h"

#include "IDisplayObjectWriter.h"
#include "animate/generator/TimelineBuilder/FrameElements/StaticElementsGroup.h"
#include "animate/generator/TimelineBuilder/FrameElements/BitmapElement.h"
#include "animate/generator/TimelineBuilder/FrameElements/FilledElement.h"
#include "animate/generator/TimelineBuilder/FrameElements/Slice9Element.h"

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

	public:
		void AddGroup(SymbolContext& symbol, const StaticElementsGroup& group)
		{
			if (!group) return;

			if (symbol.slicing.IsEnabled())
			{
				DOM::Utils::MATRIX2D matrix;
				Slice9Element slice9(symbol, group, matrix, symbol.slicing.Guides());
				AddSlicedElements(slice9);
			}
			else
			{
				for (size_t i = 0; group.Size() > i; i++)
				{
					const StaticElement& element = group[i];

					if (element.IsSprite())
					{
						AddGraphic((const BitmapElement&)element);
					}
					else if (element.IsFilledArea())
					{
						AddFilledElement((const FilledElement&)element);
					}
				}
			}
		}
	};
}
#pragma once

#include <memory>
#include <vector>

#include "AnimateCore.h"
#include "AnimateDOM.h"

#include "animate/generator/TimelineBuilder/FrameElements/SpriteElement.h"
#include "animate/generator/TimelineBuilder/FrameElements/FilledElement.h"

namespace Animate::Publisher
{
	class SharedShapeWriter {
	public:
		virtual ~SharedShapeWriter() = default;

	public:
		virtual void AddGraphic(const SpriteElement& item, const DOM::Utils::MATRIX2D& matrix) = 0;

		virtual void AddFilledElement(const FilledElement& shape) = 0;

		virtual void AddSlicedElements(const std::vector<FilledElement>& elements, const DOM::Utils::RECT& guides) = 0;

		/// <summary>
		/// Writer must finalize object and add it to its own resource palette here
		/// </summary>
		/// <param name="id"> Identifier of object </param>
		/// <param name="required"> If True then writer must return positive status, else writer can skip object writing and return False </param>
		/// <returns> True if object was written </returns>
		virtual bool Finalize(uint16_t id, bool required) = 0;
	};
}
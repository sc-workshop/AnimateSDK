#pragma once

#include "AnimateDOM.h"
#include "FilledElement.h"

namespace Animate::Publisher
{
	class SymbolContext;

	class SliceElement
	{
	public:
		SliceElement(
			SymbolContext& symbol,
			FCM::AutoPtr<DOM::FrameElement::IShape> shape,
			DOM::Utils::MATRIX2D& matrix
		);

	public:
		DOM::Utils::MATRIX2D transform;
		FilledElement fill;
	};
}
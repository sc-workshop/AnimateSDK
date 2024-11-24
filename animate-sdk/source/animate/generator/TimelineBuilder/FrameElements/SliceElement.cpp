#include "SliceElement.h"

#include "animate/publisher/symbol/SymbolContext.h"

namespace Animate::Publisher
{
	SliceElement::SliceElement(SymbolContext& symbol, FCM::AutoPtr<DOM::FrameElement::IShape> shape, DOM::Utils::MATRIX2D& matrix) :
		transform(matrix), fill(symbol, shape)
	{
	}

}
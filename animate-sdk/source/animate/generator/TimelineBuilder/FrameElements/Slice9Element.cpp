#include "Slice9Element.h"

#include "animate/publisher/symbol/SymbolContext.h"

namespace Animate::Publisher
{
	Slice9Element::Slice9Element(
			SymbolContext& symbol,
			const std::vector<FilledElement>& elements,
			DOM::Utils::MATRIX2D& matrix,
			const Animate::DOM::Utils::RECT guides
	) : StaticElement(symbol), m_elements(elements), m_guides(guides)
	{
		m_matrix = matrix;
	}

}
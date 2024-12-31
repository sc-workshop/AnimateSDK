#include "StaticElement.h"

#include "AnimateGenerator.h"

namespace Animate::Publisher
{
	StaticElement::StaticElement(SymbolContext& context) : m_symbol(context)
	{

	}

	const SymbolContext& StaticElement::Symbol() const
	{
		return m_symbol;
	}
}
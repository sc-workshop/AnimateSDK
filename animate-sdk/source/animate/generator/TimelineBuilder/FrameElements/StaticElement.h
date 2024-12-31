#pragma once

#include "animate/publisher/symbol/SymbolContext.h"

namespace Animate::Publisher
{
	class SymbolContext;

	// Base class for basic frame elements
	class StaticElement
	{
	public:
		StaticElement(SymbolContext& context);
		virtual ~StaticElement() = default;

	public:
		virtual DOM::Utils::MATRIX2D Transformation() const
		{
			return m_matrix;
		}

	public:
		virtual bool IsSprite() const
		{
			return false;
		}

		virtual bool Is9Sliced() const
		{
			return false;
		}

		virtual bool IsFilledArea() const
		{
			return false;
		}

		virtual const SymbolContext& Symbol() const;

	protected:
		SymbolContext m_symbol;
		DOM::Utils::MATRIX2D m_matrix;
	};
}
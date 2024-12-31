#pragma once

#include "AnimateDOM.h"
#include "animate/generator/TimelineBuilder/FrameElements/StaticElementsGroup.h"
#include "FilledElement.h"

namespace Animate::Publisher
{
	class SymbolContext;

	class Slice9Element : public StaticElement
	{
	public:
		Slice9Element(
			SymbolContext& symbol,
			const StaticElementsGroup& group,
			DOM::Utils::MATRIX2D& matrix,
			const Animate::DOM::Utils::RECT guides
		);

		virtual bool Is9Sliced() const
		{
			return true;
		}

		Animate::DOM::Utils::RECT Guides() const
		{
			return m_guides;
		}

		const StaticElementsGroup& Elements() const
		{
			return m_elements;
		}

	private:
		const Animate::DOM::Utils::RECT m_guides;
		const StaticElementsGroup m_elements;
	};
}
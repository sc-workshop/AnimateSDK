#include "StaticElementsGroup.h"

#include "AnimatePublisher.h"
#include "FilledElement.h"

namespace Animate::Publisher
{
	StaticElementsGroup::StaticElementsGroup()
	{
		m_items = wk::CreateRef<Elements>();
	}

	StaticElement& StaticElementsGroup::operator[](size_t index) const
	{
		return *m_items->at(index);
	}

	StaticElementsGroup& StaticElementsGroup::operator +=(const StaticElementsGroup& group)
	{
		m_items->insert(
				m_items->begin(),
				group.m_items->begin(), group.m_items->end()
		);

		return *this;
	}

	size_t StaticElementsGroup::Size() const
	{
		return m_items->size();
	}

	bool StaticElementsGroup::Empty() const
	{
		return m_items->empty();
	}

	void StaticElementsGroup::Clear()
	{
		m_items->clear();
	}
}
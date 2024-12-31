#include "StaticElementsGroup.h"

#include "AnimatePublisher.h"
#include "FilledElement.h"

namespace Animate::Publisher
{
	StaticElement& StaticElementsGroup::operator[](size_t index) const
	{
		return *m_items[index];
	}

	StaticElementsGroup& StaticElementsGroup::operator +=(const StaticElementsGroup& group)
	{
		m_items.insert(
				m_items.begin(),
				group.m_items.begin(), group.m_items.end()
		);

		return *this;
	}

	size_t StaticElementsGroup::Size() const
	{
		return m_items.size();
	}

	bool StaticElementsGroup::Empty() const
	{
		return m_items.empty();
	}

	void StaticElementsGroup::Clear()
	{
		m_items.clear();
	}
}
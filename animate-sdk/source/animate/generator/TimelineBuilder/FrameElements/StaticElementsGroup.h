#pragma once

#include "AnimateDOM.h"
#include "core/memory/ref.h"
#include <vector>

#include "FrameElement.h"
#include "FilledElement.h"
#include "SpriteElement.h"
#include "SliceElement.h"

namespace Animate::Publisher
{
	class SymbolContext;

	class StaticElementsGroup
	{
	public:
		StaticElementsGroup() = default;

	public:
		template<typename T, typename ... Args>
		T& AddElement(Args&& ... args)
		{
			wk::Ref<T> item = wk::CreateRef<T>(std::forward<Args>(args)...);
			m_items.push_back(item);
			return *item;
		}

		template<typename ... Args>
		void AddFilledElement(Args&& ... args)
		{
			AddElement<FilledElement>(std::forward<Args>(args)...);
		}

		StaticElement& operator[](size_t index) const;
		StaticElementsGroup& operator +=(const StaticElementsGroup& group);

		size_t Size() const;
		bool Empty() const;
		void Clear();

	private:
		std::vector<wk::Ref<StaticElement>> m_items;
	};

}
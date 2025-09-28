#pragma once

#include "AnimateDOM.h"
#include "core/memory/ref.h"
#include <vector>

#include "StaticElement.h"
#include "FilledElement.h"

namespace Animate::Publisher
{
	class SymbolContext;

	class StaticElementsGroup
	{
	private:
		using Elements = std::vector<wk::Ref<StaticElement>>;

	public:
		StaticElementsGroup();

	public:
		template<typename T, typename ... Args>
		T& AddElement(Args&& ... args)
		{
			wk::Ref<T> item = wk::CreateRef<T>(std::forward<Args>(args)...);
			m_items->push_back(item);
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

		operator bool() const;

	private:
		wk::Ref<Elements> m_items;
	};

}
#include "StaticElementsGroup.h"

#include "AnimatePublisher.h"
#include "FilledElement.h"

namespace Animate::Publisher {
    StaticElementsGroup::StaticElementsGroup() {
        m_items = wk::CreateRef<Elements>();
        m_inherited = wk::CreateRef<Elements>();
    }

    StaticElement& StaticElementsGroup::operator[](size_t index) const {
        if (index >= m_inherited->size()) {
            return *m_items->at(index - m_inherited->size());
        }

        return *m_inherited->at(index);
    }

    StaticElementsGroup& StaticElementsGroup::operator+=(const StaticElementsGroup& group) {
        m_inherited->insert(m_inherited->begin(), group.m_items->begin(), group.m_items->end());
        m_inherited->insert(m_inherited->begin(), group.m_inherited->begin(), group.m_inherited->end());

        return *this;
    }

    size_t StaticElementsGroup::Size() const {
        return m_items->size() + m_inherited->size();
    }

    bool StaticElementsGroup::Empty() const {
        return m_items->empty() && m_inherited->empty();
    }

    void StaticElementsGroup::Clear() {
        m_items->clear();
        Update();
    }

    void StaticElementsGroup::Update() {
        m_inherited->clear();
    }

    StaticElementsGroup::operator bool() const {
        return m_items && m_inherited && !Empty();
    }
}
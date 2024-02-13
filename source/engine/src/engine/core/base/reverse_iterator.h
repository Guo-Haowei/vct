#pragma once

namespace vct {

template<typename T, typename Base>
class ReverseIterator {
    using ThisType = ReverseIterator<T, Base>;

public:
    explicit ReverseIterator(Base base) : m_base_iterator(--base) {}

    ThisType operator++(int) {
        ThisType tmp = *this;
        --m_base_iterator;
        return tmp;
    }

    ThisType& operator++() {
        --m_base_iterator;
        return *this;
    }

    T& operator*() const { return m_base_iterator.operator*(); }
    T* operator->() const { return m_base_iterator.operator->(); }

    bool operator==(const ThisType& rhs) const { return m_base_iterator == rhs.m_base_iterator; }
    bool operator!=(const ThisType& rhs) const { return m_base_iterator != rhs.m_base_iterator; }

private:
    Base m_base_iterator;
};

}  // namespace vct

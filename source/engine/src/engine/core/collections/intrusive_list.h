#pragma once

namespace vct {

template<typename T>
struct IntrusiveListNode {
    T data;
    IntrusiveListNode<T>* next;

    IntrusiveListNode() : next(nullptr) {}
    IntrusiveListNode(const T& data) : data(data), next(nullptr) {}
};

template<typename T>
class IntrusiveListIterator {
    using this_type = IntrusiveListIterator<T>;
    using node_type = IntrusiveListNode<T>;

public:
    IntrusiveListIterator(node_type* node) : m_node(node) {}

    this_type operator++(int) {
        this_type ret = *this;
        m_node = m_node->next;
        return ret;
    }

    this_type& operator++() {
        m_node = m_node->next;
        return *this;
    }

    T& operator*() const { return m_node->data; }

    T* operator->() const { return &(m_node->data); }

    bool operator==(const this_type& rhs) const { return m_node == rhs.m_node; }

    bool operator!=(const this_type& rhs) const { return m_node != rhs.m_node; }

private:
    node_type* m_node;
};

template<typename T>
class IntrusiveList {
public:
    using iter = IntrusiveListIterator<T>;
    using const_iter = IntrusiveListIterator<const T>;
    using Node = IntrusiveListNode<T>;

    iter begin() { return iter(m_head); }

    iter end() { return iter(nullptr); }

    const_iter begin() const { return const_iter(m_head); }

    const_iter end() const { return const_iter(nullptr); }

public:
    IntrusiveList() { reset(); }

    bool empty() const { return m_size == 0; }
    size_t size() const { return m_size; }

    void node_push_front(Node* node) {
        if (empty()) {
            m_tail = node;
        } else {
            node->next = m_head;
        }
        m_head = node;

        ++m_size;
    }

    void node_push_back(Node* node) {
        if (empty()) {
            m_head = node;
            m_tail = node;
        } else {
            m_tail->next = node;
            m_tail = node;
        }

        ++m_size;
    }

    Node* node_pop_front() {
        if (empty()) {
            return nullptr;
        }

        Node* ret = m_head;
        m_head = m_head->next;
        ret->next = nullptr;

        if (m_size == 1) {
            m_tail = nullptr;
        }
        --m_size;
        return ret;
    }

    bool node_remove(const Node* node) {
        Node* cursor = m_head;
        Node* prev = nullptr;
        while (cursor) {
            // remove cursor
            if (cursor == node) {
                // cursor is the tail of the list
                if (cursor->next == nullptr) {
                    m_tail = prev;
                }

                if (prev) {
                    prev->next = cursor->next;
                } else {
                    m_head = cursor->next;
                }
                cursor->next = nullptr;
                --m_size;
                return true;
            }

            prev = cursor;
            cursor = cursor->next;
        }

        return false;
    }

    Node* node_front() {
        if (empty()) {
            return nullptr;
        }

        return m_head;
    }

    Node* node_back() {
        if (empty()) {
            return nullptr;
        }

        return m_tail;
    }

    void reset() {
        m_head = nullptr;
        m_tail = nullptr;
        m_size = 0;
    }

protected:
    Node* m_head;
    Node* m_tail;
    size_t m_size;
};

}  // namespace vct

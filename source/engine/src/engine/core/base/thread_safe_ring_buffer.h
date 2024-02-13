#pragma once
#include "ring_buffer.h"

namespace vct {

// @TODO: change to atomic int implementation
template<typename T, size_t N>
struct ThreadSafeRingBuffer {
    RingBuffer<T, N> m_ring_buffer;
    std::mutex m_lock;

    bool pop_front(T& out_value) {
        std::lock_guard<std::mutex> lock(m_lock);
        if (m_ring_buffer.empty()) {
            return false;
        }

        out_value = m_ring_buffer.front();
        m_ring_buffer.pop_front();
        return true;
    }

    bool push_back(const T& value) {
        std::lock_guard<std::mutex> lock(m_lock);
        if (m_ring_buffer.size() == m_ring_buffer.capacity()) {
            return false;
        }

        m_ring_buffer.push_back(value);
        return true;
    }
};
}  // namespace vct

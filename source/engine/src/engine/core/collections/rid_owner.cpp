#include "rid_owner.h"

#include "core/io/print.h"

namespace vct {

RID RIDAllocatorBase::allocate_rid_internal() {
    if (m_alloc_count == m_alloc_max) {
        allocate_new_chunk_internal();
    }

    DEV_ASSERT(!m_free_list.empty());

    ElementBlock* block = m_free_list.node_pop_front();

    uint32_t validator = gen_id() & BITMASK_VALID;
    CRASH_COND_MSG(validator == BITMASK_VALID, "Overflow in RID validator");
    uint64_t id = validator;
    id <<= 32;
    id |= block->data.index;

    block->data.validator = validator;
    block->data.validator |= BITMASK_UNINITIALIZED;  // mark uninitialized bit

    // PRINT("index {}, validator: {:x}, id: {:x}\n", block->data.index, block->data.validator, id);

    ++m_alloc_count;
    return make_from_id(id);
}

void* RIDAllocatorBase::get_or_null_internal(const RID& rid, bool initialize) {
    if (rid.is_null()) {
        return nullptr;
    }

    uint64_t id = rid.get_id();
    uint32_t idx = id & BITMASK_VALID;

    ERR_FAIL_COND_V(idx >= m_alloc_max, nullptr);

    const uint32_t validator = id >> 32;
    const uint32_t idx_chunk = idx / m_elements_in_chunk;
    const uint32_t idx_element = idx % m_elements_in_chunk;

    ElementBlock* block = reinterpret_cast<ElementBlock*>(&m_chunks[idx_chunk][idx_element * m_element_size]);

    if (initialize) [[unlikely]] {
        if (!(block->data.validator & BITMASK_UNINITIALIZED)) [[unlikely]] {
            ERR_FAIL_V_MSG(nullptr, "Initializing already initialized RID");
        }

        if ((block->data.validator & BITMASK_VALID) != validator) [[unlikely]] {
            ERR_FAIL_V_MSG(nullptr, "Attempting to initialize the wrong RID");
        }

        block->data.validator &= BITMASK_VALID;  // initialized
    } else if (block->data.validator != validator) [[unlikely]] {
        if ((block->data.validator & BITMASK_UNINITIALIZED) && block->data.validator != BITMASK_INVALID) {
            ERR_FAIL_V_MSG(nullptr, "Attempting to use an uninitialized RID");
        }
        return nullptr;
    }

    return (block + 1);
}

void* RIDAllocatorBase::free_rid_internal(const RID& rid) {
    uint64_t id = rid.get_id();
    uint32_t idx = id & 0xFFFFFFFF;
    if (idx >= m_alloc_max) [[unlikely]] {
        ERR_FAIL_V(nullptr);
    }

    const uint32_t validator = id >> 32;
    const uint32_t idx_chunk = idx / m_elements_in_chunk;
    const uint32_t idx_element = idx % m_elements_in_chunk;

    ElementBlock* block = reinterpret_cast<ElementBlock*>(&m_chunks[idx_chunk][idx_element * m_element_size]);

    if (block->data.validator & BITMASK_UNINITIALIZED) [[unlikely]] {
        ERR_FAIL_V_MSG(nullptr, "Attempted to free an uninitialized or invalid RID");
    }

    if (block->data.validator != validator) [[unlikely]] {
        ERR_FAIL_V(nullptr);
    }

    block->data.validator = BITMASK_INVALID;  // go invalid

    --m_alloc_count;

    m_free_list.node_push_back(block);
    return block + 1;
}

void RIDAllocatorBase::allocate_new_chunk_internal() {
    const uint32_t chunk_count = m_alloc_max == 0 ? 0 : (m_alloc_max / m_elements_in_chunk);
    m_chunks = (char**)realloc(m_chunks, sizeof(char*) * (chunk_count + 1));
    m_chunks[chunk_count] = (char*)malloc(m_element_size * m_elements_in_chunk);

    // initialize
    for (uint32_t i = 0; i < m_elements_in_chunk; ++i) {
        ElementBlock* block = reinterpret_cast<ElementBlock*>(&m_chunks[chunk_count][i * m_element_size]);
        block->data.index = m_alloc_max + i;
        block->data.validator = BITMASK_INVALID;
        m_free_list.node_push_back(block);
    }
    DEV_ASSERT(m_free_list.size() == m_elements_in_chunk);

    m_alloc_max += m_elements_in_chunk;
}

void RIDAllocatorBase::free_chunks() {
    uint32_t chunk_count = m_alloc_max / m_elements_in_chunk;
    for (uint32_t i = 0; i < chunk_count; i++) {
        free(m_chunks[i]);
    }

    if (m_chunks) {
        free(m_chunks);
    }

    m_free_list.reset();
}

}  // namespace vct

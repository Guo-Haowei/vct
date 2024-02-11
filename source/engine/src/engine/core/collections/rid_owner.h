#pragma once
#include "rid.h"
// #include "safe_refcount.h"

#include "core/io/print.h"
#include "intrusive_list.h"
// #include "core/math/math.h"
#include "core/os/spin_lock.h"

namespace vct {

template<bool TREAD_SAFE>
class RIDAllocatorLock {
public:
    void lock() {}
    void unlock() {}
};

template<>
class RIDAllocatorLock<true> {
public:
    void lock() {
        m_lock.lock();
    }

    void unlock() {
        m_lock.unlock();
    }

private:
    SpinLock m_lock;
};

class RIDAllocatorBase {
public:
    virtual ~RIDAllocatorBase() = default;

    void set_description(std::string_view description) { m_description = description; }

protected:
    enum : uint32_t {
        BITMASK_INVALID = 0xFFFFFFFF,
        BITMASK_VALID = 0x7FFFFFFF,
        BITMASK_UNINITIALIZED = 0x80000000,
    };

    struct ElementBlockMeta {
        uint32_t validator;
        uint32_t index;
    };

    using ElementBlock = IntrusiveListNode<ElementBlockMeta>;
    static_assert(sizeof(ElementBlock) == 16);

    RIDAllocatorBase(uint32_t element_size, uint32_t elements_in_chunk)
        : m_element_size(element_size),
          m_elements_in_chunk(elements_in_chunk) {
    }

    [[nodiscard]] RID allocate_rid_internal();
    [[nodiscard]] void* get_or_null_internal(const RID& rid, bool initialize);
    [[nodiscard]] void* free_rid_internal(const RID& rid);

    void allocate_new_chunk_internal();
    void free_chunks();

protected:
    static RID make_from_id(uint64_t p_id) {
        RID rid;
        rid.m_id = p_id;
        return rid;
    }

    static RID gen_rid() {
        return make_from_id(gen_id());
    }

    static uint64_t gen_id() {
        return m_base_id.fetch_add(1) + 1;
    }

protected:
    const uint32_t m_element_size;
    const uint32_t m_elements_in_chunk;

    uint32_t m_alloc_count{ 0 };
    uint32_t m_alloc_max{ 0 };

    char** m_chunks{ nullptr };
    IntrusiveList<ElementBlockMeta> m_free_list;
    std::string_view m_description;

private:
    inline static std::atomic_uint64_t m_base_id{ 0 };
};

template<typename T, bool THREAD_SAFE = false>
class RIDAllocator : public RIDAllocatorBase {
    using Guard = LockGuard<RIDAllocatorLock<THREAD_SAFE>>;

public:
    RIDAllocator(uint32_t target_chunk_byte_size = 65536)
        : RIDAllocatorBase(
              math::align<uint32_t>(sizeof(T) + sizeof(RIDAllocatorBase::ElementBlock), 16),
              uint32_t(sizeof(T) > target_chunk_byte_size ? 1 : (target_chunk_byte_size / sizeof(T)))) {
    }

    ~RIDAllocator() {
        leak_detection();
        free_chunks();
    }

    template<typename U = T, typename... Args>
    [[nodiscard]] RID make_rid(Args&&... args) {
        Guard guard(m_lock);
        RID rid = allocate_rid_internal();
        U* ptr = reinterpret_cast<U*>(get_or_null_internal(rid, true));
        DEV_ASSERT(ptr);
        new (ptr) U{ std::forward<Args>(args)... };
        return rid;
    }

    void free_rid(const RID& rid) {
        Guard guard(m_lock);
        T* p = reinterpret_cast<T*>(free_rid_internal(rid));
        DEV_ASSERT(p);
        p->~T();
    }

    [[nodiscard]] T* get_or_null(const RID& rid, bool initialize = false) {
        Guard guard(m_lock);
        return reinterpret_cast<T*>(get_or_null_internal(rid, initialize));
    }

    uint32_t get_rid_count() const { return m_alloc_count; }

protected:
    void leak_detection() {
        if (m_alloc_count) {
            LOG_WARN("ERROR: {} RID allocations of type '{}' were leaked at exit.", m_alloc_count, m_description);

            for (uint32_t i = 0; i < m_alloc_max; i++) {
                const uint32_t chunk_idx = i / m_elements_in_chunk;
                const uint32_t element_idx = i % m_elements_in_chunk;
                ElementBlock* block = reinterpret_cast<ElementBlock*>(&m_chunks[chunk_idx][element_idx * m_element_size]);

                uint32_t validator = block->data.validator;
                if (validator & BITMASK_UNINITIALIZED) {
                    continue;  // uninitialized
                }
                if (validator != BITMASK_INVALID) {
                    auto ptr = reinterpret_cast<T*>(block + 1);
                    ptr->~T();
                }
            }
        }
    }

    RIDAllocatorLock<THREAD_SAFE> m_lock;
};
// public:
//     RID make_rid()
//     {
//         RID rid = _allocate_rid();
//         initialize_rid(rid);
//         return rid;
//     }
//     RID make_rid(const T &p_value)
//     {
//         RID rid = _allocate_rid();
//         initialize_rid(rid, p_value);
//         return rid;
//     }
//
//     // allocate but don't initialize, use initialize_rid afterwards
//     RID allocate_rid()
//     {
//         return _allocate_rid();
//     }

// void initialize_rid(RID p_rid)
//{
//    T *mem = get_or_null(p_rid, true);
//    ERR_FAIL_NULL(mem);
//    memnew_placement(mem, T);
//}
// void initialize_rid(RID p_rid, const T &p_value)
//{
//    T *mem = get_or_null(p_rid, true);
//    ERR_FAIL_NULL(mem);
//    memnew_placement(mem, T(p_value));
//}

// bool owns(const RID &p_rid) const
//{
//     if (THREAD_SAFE)
//     {
//         spin_lock.lock();
//     }

//    uint64_t id = p_rid.get_id();
//    uint32_t idx = uint32_t(id & 0xFFFFFFFF);
//    if (unlikely(idx >= max_alloc))
//    {
//        if (THREAD_SAFE)
//        {
//            spin_lock.unlock();
//        }
//        return false;
//    }

//    uint32_t idx_chunk = idx / elements_in_chunk;
//    uint32_t idx_element = idx % elements_in_chunk;

//    uint32_t validator = uint32_t(id >> 32);

//    bool owned = (validator != 0x7FFFFFFF) && (validator_chunks[idx_chunk][idx_element] & 0x7FFFFFFF) == validator;

//    if (THREAD_SAFE)
//    {
//        spin_lock.unlock();
//    }

//    return owned;
//}

// uint32_t get_rid_count() const
// {
//     return alloc_count;
// }
// void get_owned_list(List<RID> *p_owned) const
//{
//     if (THREAD_SAFE)
//     {
//         spin_lock.lock();
//     }
//     for (size_t i = 0; i < max_alloc; i++)
//     {
//         uint64_t validator = validator_chunks[i / elements_in_chunk][i % elements_in_chunk];
//         if (validator != 0xFFFFFFFF)
//         {
//             p_owned->push_back(_make_from_id((validator << 32) | i));
//         }
//     }
//     if (THREAD_SAFE)
//     {
//         spin_lock.unlock();
//     }
// }

// used for fast iteration in the elements or RIDs
// void fill_owned_buffer(RID *p_rid_buffer) const
//{
//    if (THREAD_SAFE)
//    {
//        spin_lock.lock();
//    }
//    uint32_t idx = 0;
//    for (size_t i = 0; i < max_alloc; i++)
//    {
//        uint64_t validator = validator_chunks[i / elements_in_chunk][i % elements_in_chunk];
//        if (validator != 0xFFFFFFFF)
//        {
//            p_rid_buffer[idx] = _make_from_id((validator << 32) | i);
//            idx++;
//        }
//    }
//    if (THREAD_SAFE)
//    {
//        spin_lock.unlock();
//    }
//}

}  // namespace vct
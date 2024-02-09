#pragma once
#include "core/io/archive.h"

namespace vct::ecs {

class Entity {
public:
    static constexpr size_t kInvalidIndex = ~0llu;
    static constexpr uint32_t kInvalidId = 0;

    explicit constexpr Entity() : m_id(kInvalidId) {}

    explicit constexpr Entity(uint32_t handle) : m_id(handle) {}

    ~Entity() = default;

    bool operator==(const Entity& rhs) const { return m_id == rhs.m_id; }

    bool operator!=(const Entity& rhs) const { return m_id != rhs.m_id; }

    bool is_valid() const { return m_id != kInvalidId; }

    void make_invalid() { m_id = kInvalidId; }

    constexpr uint32_t get_id() const { return m_id; }

    void serialize(Archive& archive);

    static Entity create();

    static const Entity kInvalid;

private:
    uint32_t m_id;
};

}  // namespace vct::ecs

namespace std {

template<>
struct hash<vct::ecs::Entity> {
    std::size_t operator()(const vct::ecs::Entity& entity) const { return std::hash<uint32_t>{}(entity.get_id()); }
};

template<>
struct less<vct::ecs::Entity> {
    constexpr bool operator()(const vct::ecs::Entity& lhs, const vct::ecs::Entity& rhs) const {
        return lhs.get_id() < rhs.get_id();
    }
};

template<>
struct equal_to<vct::ecs::Entity> {
    constexpr bool operator()(const vct::ecs::Entity& lhs, const vct::ecs::Entity& rhs) const {
        return lhs.get_id() == rhs.get_id();
    }
};

}  // namespace std
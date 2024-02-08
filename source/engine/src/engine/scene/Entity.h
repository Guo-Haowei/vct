#pragma once
#include "core/io/archive.h"

namespace vct::ecs {

class Entity {
public:
    static constexpr size_t INVALID_INDEX = ~0llu;
    static constexpr uint32_t INVALID_ID = 0;

    explicit constexpr Entity() : mID(INVALID_ID) {}

    explicit constexpr Entity(uint32_t handle) : mID(handle) {}

    ~Entity() = default;

    bool operator==(const Entity& rhs) const { return mID == rhs.mID; }

    bool operator!=(const Entity& rhs) const { return mID != rhs.mID; }

    bool IsValid() const { return mID != INVALID_ID; }

    void MakeInvalid() { mID = INVALID_ID; }

    constexpr uint32_t GetID() const { return mID; }

    void serialize(Archive& archive);

    static Entity create();

    static const Entity INVALID;

private:
    uint32_t mID;

    friend class EntityGenerator;
};

}  // namespace vct::ecs

namespace std {

template<>
struct hash<vct::ecs::Entity> {
    std::size_t operator()(const vct::ecs::Entity& entity) const { return std::hash<uint32_t>{}(entity.GetID()); }
};

template<>
struct less<vct::ecs::Entity> {
    constexpr bool operator()(const vct::ecs::Entity& lhs, const vct::ecs::Entity& rhs) const {
        return lhs.GetID() < rhs.GetID();
    }
};

template<>
struct equal_to<vct::ecs::Entity> {
    constexpr bool operator()(const vct::ecs::Entity& lhs, const vct::ecs::Entity& rhs) const {
        return lhs.GetID() == rhs.GetID();
    }
};

}  // namespace std

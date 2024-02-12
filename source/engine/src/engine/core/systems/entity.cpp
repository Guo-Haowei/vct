#include "entity.h"

namespace vct::ecs {

const Entity Entity::INVALID{};

void Entity::serialize(Archive& archive) {
    if (archive.is_write_mode()) {
        archive << m_id;
    } else {
        archive >> m_id;
    }
}

Entity Entity::create() {
    static std::atomic<uint32_t> s_next = 1;
    DEV_ASSERT(s_next.load() < ~uint32_t(0));
    Entity entity(s_next.fetch_add(1));
    return entity;
}

}  // namespace vct::ecs

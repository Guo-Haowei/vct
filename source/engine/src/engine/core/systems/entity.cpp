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
    CRASH_COND_MSG(s_id.load() == MAX_ID, "max number of entity allocated, did you forget to call set_seed()?");
    Entity entity(s_id.fetch_add(1));
    return entity;
}

uint32_t Entity::get_seed() {
    return s_id;
}

void Entity::set_seed(uint32_t seed) {
    s_id = seed;
}

}  // namespace vct::ecs

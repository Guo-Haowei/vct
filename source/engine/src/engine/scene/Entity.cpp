#include "Entity.h"

#include "core/io/archive.h"

namespace ecs {

const Entity Entity::INVALID{};

void Entity::Serialize(Archive& archive) {
    if (archive.is_write_mode()) {
        archive << mID;
    } else {
        archive >> mID;
    }
}

}  // namespace ecs

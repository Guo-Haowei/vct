#include "Entity.h"

#include "Archive.h"

namespace ecs {

const Entity Entity::INVALID{};

void Entity::Serialize(Archive& archive) {
    if (archive.IsWriteMode()) {
        archive << mID;
    } else {
        archive >> mID;
    }
}

}  // namespace ecs

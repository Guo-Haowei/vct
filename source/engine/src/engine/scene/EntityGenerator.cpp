#include "EntityGenerator.h"

#include "core/io/archive.h"

namespace ecs {

EntityGenerator::EntityGenerator() : mNext(Entity::INVALID_ID + 1) {}

Entity EntityGenerator::Create() { return Entity(mNext.fetch_add(1)); }

void EntityGenerator::Serialize(Archive& archive) {
    if (archive.is_write_mode()) {
        archive << mNext.load();
    } else {
        uint32_t value;
        archive >> value;
        mNext.store(value);
    }
}

}  // namespace ecs

#include "EntityGenerator.h"

#include "Core/Archive.h"

namespace ecs
{

EntityGenerator::EntityGenerator()
    : mNext(Entity::INVALID_ID + 1)
{
}

Entity EntityGenerator::Create()
{
    return Entity(mNext.fetch_add(1));
}

void EntityGenerator::Serialize(Archive& archive)
{
    if (archive.IsWriteMode())
    {
        archive << mNext.load();
    }
    else
    {
        uint32_t value;
        archive >> value;
        mNext.store(value);
    }
}

}  // namespace ecs

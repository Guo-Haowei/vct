#pragma once
#include "Entity.h"

namespace ecs {

class EntityGenerator {
    EntityGenerator(const EntityGenerator&) = delete;
    EntityGenerator& operator=(const EntityGenerator&) = delete;

public:
    EntityGenerator();

    Entity Create();

    void Serialize(Archive& archive);

private:
    std::atomic<uint32_t> mNext;
};

}  // namespace ecs

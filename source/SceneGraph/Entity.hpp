#pragma once
#include <list>
#include <memory>
#include <string>

#include "universal/core_math.h"

class Entity {
    Entity( Entity& ) = delete;
    Entity( Entity&& ) = delete;

public:
    using Ptr = std::shared_ptr<Entity>;

    Entity();
    virtual ~Entity();

protected:
    Ptr m_pParent;
    std::list<Ptr> m_children;
};

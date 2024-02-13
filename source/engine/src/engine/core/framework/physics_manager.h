#pragma once
#include "core/base/singleton.h"
#include "core/framework/event_queue.h"
#include "core/framework/module.h"

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btCollisionShape;

namespace vct {

class Scene;

class PhysicsManager : public Singleton<PhysicsManager>, public Module, public EventListener {
public:
    PhysicsManager() : Module("PhysicsManager") {}

    bool initialize() override;
    void finalize() override;

    void update(float dt);

    void event_received(std::shared_ptr<Event> event) override;

protected:
    void create_world(const Scene& scene);
    void clean_world();
    bool has_world() const { return m_collision_config != nullptr; }

    btDefaultCollisionConfiguration* m_collision_config = nullptr;
    btCollisionDispatcher* m_dispatcher = nullptr;
    btBroadphaseInterface* m_overlapping_pair_cache = nullptr;
    btSequentialImpulseConstraintSolver* m_solver = nullptr;
    btDiscreteDynamicsWorld* m_dynamic_world = nullptr;
    std::vector<btCollisionShape*> m_collision_shapes;
};

}  // namespace vct

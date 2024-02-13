#include "physics_manager.h"

#include "core/framework/scene_manager.h"
#include "scene/scene.h"

#pragma warning(push)
#pragma warning(disable : 4127)
#include "bullet3/btBulletDynamicsCommon.h"
#pragma warning(pop)

namespace vct {

bool PhysicsManager::initialize() {
    return true;
}

void PhysicsManager::finalize() {
    clean_world();
}

void PhysicsManager::event_received(std::shared_ptr<Event> event) {
    SceneChangeEvent* e = dynamic_cast<SceneChangeEvent*>(event.get());
    if (!e) {
        return;
    }

    const Scene& scene = *e->get_scene();
    // @TODO: fix
    create_world(scene);
}

void PhysicsManager::update(float dt) {
    Scene& scene = SceneManager::singleton().get_scene();

    if (has_world()) {
        m_dynamic_world->stepSimulation(dt, 10);

        for (int j = m_dynamic_world->getNumCollisionObjects() - 1; j >= 0; j--) {
            btCollisionObject* collision_object = m_dynamic_world->getCollisionObjectArray()[j];
            btRigidBody* rigid_body = btRigidBody::upcast(collision_object);
            btTransform transform;

            if (rigid_body && rigid_body->getMotionState()) {
                rigid_body->getMotionState()->getWorldTransform(transform);
            } else {
                transform = collision_object->getWorldTransform();
            }

            uint32_t handle = (uint32_t)(uintptr_t)collision_object->getUserPointer();
            ecs::Entity id{ handle };
            if (id.is_valid()) {
                TransformComponent& transform_component = *scene.get_component<TransformComponent>(id);
                const btVector3& origin = transform.getOrigin();
                const btQuaternion rotation = transform.getRotation();
                transform_component.set_translation(vec3(origin.getX(), origin.getY(), origin.getZ()));
                transform_component.set_rotation(vec4(rotation.getX(), rotation.getY(), rotation.getZ(), rotation.getW()));
            }
        }
    }
}

void PhysicsManager::create_world(const Scene& scene) {
    m_collision_config = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collision_config);
    m_overlapping_pair_cache = new btDbvtBroadphase();
    m_solver = new btSequentialImpulseConstraintSolver;
    m_dynamic_world = new btDiscreteDynamicsWorld(m_dispatcher, m_overlapping_pair_cache, m_solver, m_collision_config);

    m_dynamic_world->setGravity(btVector3(0, -10, 0));

    for (int i = 0; i < scene.get_count<RigidBodyComponent>(); ++i) {
        ecs::Entity id = scene.get_entity<RigidBodyComponent>(i);
        const RigidBodyComponent& rigid_body = scene.get_component_array<RigidBodyComponent>()[i];
        const TransformComponent* transform_component = scene.get_component<TransformComponent>(id);
        DEV_ASSERT(transform_component);
        if (!transform_component) {
            continue;
        }

        btCollisionShape* shape = nullptr;
        switch (rigid_body.shape) {
            case RigidBodyComponent::SHAPE_BOX: {
                const vec3& half = rigid_body.param.box.half_size;
                shape = new btBoxShape(btVector3(half.x, half.y, half.z));
                break;
            }
            default:
                CRASH_NOW_MSG("unknown rigidBody.shape");
                break;
        }

        m_collision_shapes.push_back(shape);

        const vec3& origin = transform_component->get_translation();
        const vec4& rotation = transform_component->get_rotation();
        btTransform transform;
        transform.setIdentity();
        transform.setOrigin(btVector3(origin.x, origin.y, origin.z));
        transform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));

        btScalar mass(rigid_body.mass);
        bool isDynamic = (mass != 0.f);  // rigidbody is dynamic if and only if mass is non zero, otherwise static

        btVector3 localInertia(0, 0, 0);
        if (isDynamic) {
            shape->calculateLocalInertia(mass, localInertia);
        }

        // using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
        btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
        btRigidBody* body = new btRigidBody(rbInfo);
        body->setUserPointer((void*)((size_t)id.get_id()));
        m_dynamic_world->addRigidBody(body);
    }
}

void PhysicsManager::clean_world() {
    if (has_world()) {
        // remove the rigidbodies from the dynamics world and delete them
        for (int i = m_dynamic_world->getNumCollisionObjects() - 1; i >= 0; i--) {
            btCollisionObject* obj = m_dynamic_world->getCollisionObjectArray()[i];
            btRigidBody* body = btRigidBody::upcast(obj);
            if (body && body->getMotionState()) {
                delete body->getMotionState();
            }
            m_dynamic_world->removeCollisionObject(obj);
            delete obj;
        }

        // delete collision shapes
        for (int j = 0; j < m_collision_shapes.size(); j++) {
            btCollisionShape* shape = m_collision_shapes[j];
            m_collision_shapes[j] = 0;
            delete shape;
        }

        // delete dynamics world
        delete m_dynamic_world;
        m_dynamic_world = nullptr;

        // delete m_solver
        delete m_solver;
        m_solver = nullptr;

        // delete broadphase
        delete m_overlapping_pair_cache;
        m_overlapping_pair_cache = nullptr;

        // delete m_dispatcher
        delete m_dispatcher;
        m_dispatcher = nullptr;

        delete m_collision_config;
        m_collision_config = nullptr;

        m_collision_shapes.clear();
    }
}
}  // namespace vct

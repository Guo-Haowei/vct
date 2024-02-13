#pragma once
#include "core/base/noncopyable.h"
#include "core/math/ray.h"
#include "core/systems/component_manager.h"
#include "scene_components.h"

namespace vct {

class Scene : public NonCopyable {
public:
    static constexpr const char* EXTENSION = ".scene";

    Scene() = default;

#pragma region WORLD_COMPONENTS_REGISTERY
#define REGISTER_COMPONENT(T, VER)                                                               \
private:                                                                                         \
    ecs::ComponentManager<T>& m_##T##s = m_component_lib.register_manager<T>("World::" #T, VER); \
                                                                                                 \
public:                                                                                          \
    template<>                                                                                   \
    inline const T* get_component<T>(const ecs::Entity& entity) const {                          \
        return m_##T##s.get_component(entity);                                                   \
    }                                                                                            \
    template<>                                                                                   \
    inline T* get_component<T>(const ecs::Entity& entity) {                                      \
        return m_##T##s.get_component(entity);                                                   \
    }                                                                                            \
    template<>                                                                                   \
    inline const T& get_component<T>(size_t index) const {                                       \
        return m_##T##s[index];                                                                  \
    }                                                                                            \
    template<>                                                                                   \
    inline T& get_component<T>(size_t index) {                                                   \
        return m_##T##s[index];                                                                  \
    }                                                                                            \
    template<>                                                                                   \
    inline std::vector<T>& get_component_array() {                                               \
        return m_##T##s.get_component_array();                                                   \
    }                                                                                            \
    template<>                                                                                   \
    inline const std::vector<T>& get_component_array() const {                                   \
        return m_##T##s.get_component_array();                                                   \
    }                                                                                            \
    template<>                                                                                   \
    inline bool contains<T>(const ecs::Entity& entity) const {                                   \
        return m_##T##s.contains(entity);                                                        \
    }                                                                                            \
    template<>                                                                                   \
    inline size_t get_index<T>(const ecs::Entity& entity) const {                                \
        return m_##T##s.get_index(entity);                                                       \
    }                                                                                            \
    template<>                                                                                   \
    inline size_t get_count<T>() const {                                                         \
        return m_##T##s.get_count();                                                             \
    }                                                                                            \
    template<>                                                                                   \
    inline ecs::Entity get_entity<T>(size_t index) const {                                       \
        return m_##T##s.get_entity(index);                                                       \
    }                                                                                            \
    template<>                                                                                   \
    T& create<T>(const ecs::Entity& entity) {                                                    \
        return m_##T##s.create(entity);                                                          \
    }                                                                                            \
    template<>                                                                                   \
    void serialize<T>(Archive & archive) {                                                       \
        return m_##T##s.serialize(archive);                                                      \
    }                                                                                            \
    enum { __DUMMY_ENUM_TO_FORCE_SEMI_COLON_##T }

#pragma endregion WORLD_COMPONENTS_REGISTERY
    template<typename T>
    const T* get_component(const ecs::Entity&) const {
        return nullptr;
    }
    template<typename T>
    T* get_component(const ecs::Entity&) {
        return nullptr;
    }
    template<typename T>
    const T& get_component(size_t) const {
        return *(reinterpret_cast<T*>(nullptr));
    }
    template<typename T>
    T& get_component(size_t) {
        return *(reinterpret_cast<T*>(nullptr));
    }
    template<typename T>
    std::vector<T>& get_component_array() {
        return *(reinterpret_cast<std::vector<T>*>(nullptr));
    }
    template<typename T>
    const std::vector<T>& get_component_array() const {
        return *(reinterpret_cast<std::vector<T>*>(nullptr));
    }
    template<typename T>
    bool contains(const ecs::Entity&) const {
        return false;
    }
    template<typename T>
    size_t get_index(const ecs::Entity&) const {
        return ecs::Entity::INVALID_INDEX;
    }
    template<typename T>
    size_t get_count() const {
        return 0;
    }
    template<typename T>
    ecs::Entity get_entity(size_t) const {
        return ecs::Entity::INVALID;
    }
    template<typename T>
    T& create(const ecs::Entity&) {
        return *(T*)(nullptr);
    }
    template<typename T>
    void serialize(Archive&) {
    }

    ecs::ComponentLibrary m_component_lib;

    REGISTER_COMPONENT(NameComponent, 0);
    REGISTER_COMPONENT(TransformComponent, 0);
    REGISTER_COMPONENT(HierarchyComponent, 0);
    REGISTER_COMPONENT(MaterialComponent, 0);
    REGISTER_COMPONENT(MeshComponent, 0);
    REGISTER_COMPONENT(ObjectComponent, 0);
    REGISTER_COMPONENT(CameraComponent, 0);
    REGISTER_COMPONENT(LightComponent, 0);
    REGISTER_COMPONENT(ArmatureComponent, 0);
    REGISTER_COMPONENT(AnimationComponent, 0);
    REGISTER_COMPONENT(RigidBodyComponent, 0);

    bool serialize(Archive& archive);

    // Non-serialized attributes
    void update(float deltaTime);

    void merge(Scene& other);

    ecs::Entity create_name_entity(const std::string& name);
    ecs::Entity create_transform_entity(const std::string& name);
    ecs::Entity create_object_entity(const std::string& name);
    ecs::Entity create_mesh_entity(const std::string& name);
    ecs::Entity create_material_entity(const std::string& name);

    ecs::Entity create_camera_entity(const std::string& name, float width, float height,
                                     float nearPlane = CameraComponent::DEFAULT_NEAR,
                                     float farPlane = CameraComponent::DEFAULT_FAR,
                                     Degree fovy = CameraComponent::DEFAULT_FOV);

    ecs::Entity create_pointlight_entity(const std::string& name, const vec3& position, const vec3& color = vec3(1),
                                         const float energy = 10.0f);

    ecs::Entity create_omnilight_entity(const std::string& name, const vec3& color = vec3(1),
                                        const float energy = 10.0f);

    ecs::Entity create_sphere_entity(const std::string& name, float radius = 0.5f, const mat4& transform = mat4(1.0f));

    ecs::Entity create_sphere_entity(const std::string& name, ecs::Entity material_id, float radius = 0.5f,
                                     const mat4& transform = mat4(1.0f));

    ecs::Entity create_cube_entity(const std::string& name, const vec3& scale = vec3(0.5f),
                                   const mat4& transform = mat4(1.0f));

    ecs::Entity create_cube_entity(const std::string& name, ecs::Entity material_id, const vec3& scale = vec3(0.5f),
                                   const mat4& transform = mat4(1.0f));

    void attach_component(ecs::Entity entity, ecs::Entity parent);

    void attach_component(ecs::Entity entity) { attach_component(entity, m_root); }

    void detach_component(ecs::Entity entity);

    // @TODO: fix
    void Component_DetachChildren(ecs::Entity parent);

    CameraComponent& get_main_camera();

    struct RayIntersectionResult {
        ecs::Entity entity;
    };

    RayIntersectionResult Intersects(Ray& ray);

    ecs::Entity m_root;
    float m_delta_time = 0.0f;

    // @TODO: refactor
    AABB m_bound;
    ecs::Entity m_selected;

private:
    void update_transformation(uint32_t index);
    void update_hierarchy(uint32_t index);
    void update_animation(uint32_t index);
    void update_armature(uint32_t index);
};

}  // namespace vct

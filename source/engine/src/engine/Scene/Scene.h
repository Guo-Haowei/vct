#pragma once
#include "SceneComponents.h"

#include "Core/Types.h"
#include "ComponentManager.h"
#include "EntityGenerator.h"

#include "Math/Ray.h"

namespace jobsystem
{
class Context;
}

struct Light
{
    vec3 direction;
    vec3 color;
};

class Scene
{
    DISABLE_COPY(Scene);

public:
    static constexpr const char* EXTENSION = ".scene";

    Scene() {}
    Scene(uint32_t revision) : mRevision(revision) {}

#pragma region WORLD_COMPONENTS_REGISTERY
#define REGISTER_COMPONENT(T, VER)                                   \
private:                                                             \
    ecs::ComponentManager<T>& m##T##s =                              \
        mComponentLibrary.Register<T>("World::" #T, VER);            \
                                                                     \
public:                                                              \
    template<>                                                       \
    inline const T* GetComponent<T>(const ecs::Entity& entity) const \
    {                                                                \
        return m##T##s.GetComponent(entity);                         \
    }                                                                \
    template<>                                                       \
    inline T* GetComponent<T>(const ecs::Entity& entity)             \
    {                                                                \
        return m##T##s.GetComponent(entity);                         \
    }                                                                \
    template<>                                                       \
    inline const T& GetComponent<T>(size_t index) const              \
    {                                                                \
        return m##T##s[index];                                       \
    }                                                                \
    template<>                                                       \
    inline T& GetComponent<T>(size_t index)                          \
    {                                                                \
        return m##T##s[index];                                       \
    }                                                                \
    template<>                                                       \
    inline std::vector<T>& GetComponentArray()                       \
    {                                                                \
        return m##T##s.GetComponentArray();                          \
    }                                                                \
    template<>                                                       \
    inline const std::vector<T>& GetComponentArray() const           \
    {                                                                \
        return m##T##s.GetComponentArray();                          \
    }                                                                \
    template<>                                                       \
    inline bool Contains<T>(const ecs::Entity& entity) const         \
    {                                                                \
        return m##T##s.Contains(entity);                             \
    }                                                                \
    template<>                                                       \
    inline size_t GetIndex<T>(const ecs::Entity& entity) const       \
    {                                                                \
        return m##T##s.GetIndex(entity);                             \
    }                                                                \
    template<>                                                       \
    inline size_t GetCount<T>() const                                \
    {                                                                \
        return m##T##s.GetCount();                                   \
    }                                                                \
    template<>                                                       \
    inline ecs::Entity GetEntity<T>(size_t index) const              \
    {                                                                \
        return m##T##s.GetEntity(index);                             \
    }                                                                \
    template<>                                                       \
    T& Create<T>(const ecs::Entity& entity)                          \
    {                                                                \
        return m##T##s.Create(entity);                               \
    }                                                                \
    enum                                                             \
    {                                                                \
        __DUMMY_ENUM_TO_FORCE_SEMI_COLON_##T                         \
    }

#pragma endregion WORLD_COMPONENTS_REGISTERY
    template<typename T>
    const T* GetComponent(const ecs::Entity&) const
    {
        return nullptr;
    }
    template<typename T>
    T* GetComponent(const ecs::Entity&)
    {
        return nullptr;
    }
    template<typename T>
    const T& GetComponent(size_t) const
    {
        return *(reinterpret_cast<T*>(nullptr));
    }
    template<typename T>
    T& GetComponent(size_t)
    {
        return *(reinterpret_cast<T*>(nullptr));
    }
    template<typename T>
    std::vector<T>& GetComponentArray()
    {
        return *(reinterpret_cast<std::vector<T>*>(nullptr));
    }
    template<typename T>
    const std::vector<T>& GetComponentArray() const
    {
        return *(reinterpret_cast<std::vector<T>*>(nullptr));
    }
    template<typename T>
    bool Contains(const ecs::Entity&) const
    {
        return false;
    }
    template<typename T>
    size_t GetIndex(const ecs::Entity&) const
    {
        return ecs::Entity::INVALID_INDEX;
    }
    template<typename T>
    size_t GetCount() const
    {
        return 0;
    }
    template<typename T>
    ecs::Entity GetEntity(size_t) const
    {
        return ecs::Entity::INVALID;
    }
    template<typename T>
    T& Create(const ecs::Entity&)
    {
        return *(T*)(nullptr);
    }

    ecs::ComponentLibrary mComponentLibrary;

    REGISTER_COMPONENT(TagComponent, 0);
    REGISTER_COMPONENT(TransformComponent, 0);
    REGISTER_COMPONENT(HierarchyComponent, 0);
    REGISTER_COMPONENT(MaterialComponent, 0);
    REGISTER_COMPONENT(MeshComponent, 0);
    REGISTER_COMPONENT(ObjectComponent, 0);
    REGISTER_COMPONENT(CameraComponent, 0);
    REGISTER_COMPONENT(LightComponent, 0);
    REGISTER_COMPONENT(ArmatureComponent, 0);
    REGISTER_COMPONENT(AnimationComponent, 0);
    REGISTER_COMPONENT(RigidBodyPhysicsComponent, 0);

    void Serialize(Archive& archive);

    // Non-serialized attributes
    void Update(float deltaTime);

    void Merge(Scene& other);

    ecs::Entity Entity_CreateName(const std::string& name);

    ecs::Entity Entity_CreateTransform(const std::string& name);

    ecs::Entity Entity_CreateObject(const std::string& name);

    ecs::Entity Entity_CreateMesh(const std::string& name);

    ecs::Entity Entity_CreateMaterial(const std::string& name);

    ecs::Entity Entity_CreateCamera(
        const std::string& name,
        float width,
        float height,
        float nearPlane = CameraComponent::DEFAULT_ZNEAR,
        float farPlane = CameraComponent::DEFAULT_ZFAR,
        float fovy = CameraComponent::DEFAULT_FOVY);

    ecs::Entity Entity_CreatePointLight(
        const std::string& name,
        const vec3& position,
        const vec3& color = vec3(1),
        const float energy = 10.0f);

    ecs::Entity Entity_CreateOmniLight(
        const std::string& name,
        const vec3& color = vec3(1),
        const float energy = 10.0f);

    ecs::Entity Entity_CreateSphere(
        const std::string& name,
        float radius = 0.5f,
        const mat4& transform = mat4(1.0f));

    ecs::Entity Entity_CreateSphere(
        const std::string& name,
        ecs::Entity materialID,
        float radius = 0.5f,
        const mat4& transform = mat4(1.0f));

    ecs::Entity Entity_CreateCube(
        const std::string& name,
        const vec3& scale = vec3(0.5f),
        const mat4& transform = mat4(1.0f));

    ecs::Entity Entity_CreateCube(
        const std::string& name,
        ecs::Entity materialID,
        const vec3& scale = vec3(0.5f),
        const mat4& transform = mat4(1.0f));

    void Component_Attach(ecs::Entity entity, ecs::Entity parent);

    void Component_Attach(ecs::Entity entity)
    {
        Component_Attach(entity, mRoot);
    }

    void Component_Detach(ecs::Entity entity);

    void Component_DetachChildren(ecs::Entity parent);

    void RunAnimationUpdateSystem(jobsystem::Context& ctx);
    void RunTransformUpdateSystem(jobsystem::Context& ctx);
    void RunHierarchyUpdateSystem(jobsystem::Context& ctx);
    void RunArmatureUpdateSystem(jobsystem::Context& ctx);

    void RunObjectUpdateSystem();
    void RunCameraUpdateSystem();
    void RunLightUpdateSystem();

    struct RayIntersectionResult
    {
        ecs::Entity entity;
    };

    RayIntersectionResult Intersects(Ray& ray);

    ecs::Entity mRoot;
    float mDeltaTime = 0.0f;

    uint32_t mRevision = 0;

    ecs::Entity CreateEntity()
    {
        return mGenerator.Create();
    }

    // @TODO: refactor
    Light light;
    AABB bound;
    ecs::Entity mSelected = ecs::Entity::INVALID;

private:
    ecs::EntityGenerator mGenerator;
};

// @TODO: refactor
void ModelImporter_AssimpImport(const std::string& file_path,
                                Scene& scene);

void ModelImporter_TinyGLTFImport(const std::string& file_path,
                                  Scene& scene);

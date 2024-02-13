#pragma once
#include "core/base/rid.h"
#include "core/math/aabb.h"
#include "core/math/degree.h"
#include "core/systems/entity.h"

namespace vct {

class Archive;
class Scene;

//--------------------------------------------------------------------------------------------------
// Name Component
//--------------------------------------------------------------------------------------------------
class NameComponent {
public:
    NameComponent() = default;

    NameComponent(const char* tag) { m_name = tag; }

    void set_name(const char* tag) { m_name = tag; }
    void set_name(const std::string& tag) { m_name = tag; }

    const std::string& get_name() const { return m_name; }
    std::string& get_name_ref() { return m_name; }

    void serialize(Archive& archive);

private:
    std::string m_name;
};

//--------------------------------------------------------------------------------------------------
// Transform Component
//--------------------------------------------------------------------------------------------------
class TransformComponent {
public:
    enum : uint32_t {
        NONE = 0,
        DIRTY = 1 << 0,
    };

    bool is_dirty() const { return m_flags & DIRTY; }
    void set_dirty(bool dirty = true) { dirty ? m_flags |= DIRTY : m_flags &= ~DIRTY; }

    const vec3& get_translation() const { return m_translation; }
    void set_translation(const vec3& t) { m_translation = t; }

    const vec4& get_rotation() const { return m_rotation; }
    void set_rotation(const vec4& r) { m_rotation = r; }

    const vec3& get_scale() const { return m_scale; }
    void set_scale(const vec3& s) { m_scale = s; }

    const mat4& get_world_matrix() const { return m_world_matrix; }

    void set_world_matrix(const mat4& matrix) { m_world_matrix = matrix; }

    mat4 get_local_matrix() const;

    void update_transform();
    void scale(const vec3& scale);
    void translate(const vec3& translation);
    void rotate(const vec3& euler);

    void set_local_transform(const mat4& matrix);
    void matrix_transform(const mat4& matrix);

    void update_transform_parented(const TransformComponent& parent);

    void serialize(Archive& archive);

private:
    uint32_t m_flags = DIRTY;

    vec3 m_scale = vec3(1);              // local scale
    vec3 m_translation = vec3(0);        // local translation
    vec4 m_rotation = vec4(0, 0, 0, 1);  // local rotation

    // Non-serialized attributes
    mat4 m_world_matrix = mat4(1);
};

//--------------------------------------------------------------------------------------------------
// Hierarchy Component
//--------------------------------------------------------------------------------------------------
class HierarchyComponent {
public:
    ecs::Entity GetParent() const { return m_parent_id; }

    void serialize(Archive& archive);

private:
    ecs::Entity m_parent_id;

    friend class Scene;
};

//--------------------------------------------------------------------------------------------------
// Camera Component
//--------------------------------------------------------------------------------------------------
class CameraComponent {
public:
    enum : uint32_t {
        NONE = 0,
        DIRTY = 1,
    };

    static constexpr float DEFAULT_NEAR = 0.1f;
    static constexpr float DEFAULT_FAR = 100.0f;
    static constexpr Degree DEFAULT_FOV{ 50.0f };

    void update();

    void set_dimension(float width, float height);

    bool is_dirty() const { return m_flags & DIRTY; }
    void set_dirty(bool dirty = true) { dirty ? m_flags |= DIRTY : m_flags &= ~DIRTY; }

    float get_near() const { return m_near; }
    float get_far() const { return m_far; }
    const mat4& get_view_matrix() const { return m_view_matrix; }
    const mat4& get_projection_matrix() const { return m_projection_matrix; }
    const mat4& get_projection_view_matrix() const { return m_projection_view_matrix; }

    const vec3& get_eye() const { return m_eye; }
    const vec3& get_center() const { return m_center; }

    void set_eye(const vec3& eye) {
        set_dirty();
        m_eye = eye;
    }
    void set_center(const vec3& center) {
        set_dirty();
        m_center = center;
    }

    void serialize(Archive& archive);

private:
    uint32_t m_flags = DIRTY;

    float m_near = DEFAULT_NEAR;
    float m_far = DEFAULT_FAR;
    Degree m_fovy{ DEFAULT_FOV };
    float m_width = 0.0f;
    float m_height = 0.0f;

    vec3 m_eye;
    vec3 m_center{ 0, 0, 0 };

    // Non-serlialized
    mat4 m_view_matrix;
    mat4 m_projection_matrix;
    mat4 m_projection_view_matrix;

    friend class Scene;
};

//--------------------------------------------------------------------------------------------------
// Mesh Component
//--------------------------------------------------------------------------------------------------
struct MeshComponent {
    enum : uint32_t {
        NONE = 0,
        RENDERABLE = 1 << 0,
        DOUBLE_SIDED = 1 << 1,
        DYNAMIC = 1 << 2,
    };

    uint32_t flags = RENDERABLE;

    struct VertexAttribute {
        enum NAME {
            POSITION = 0,
            NORMAL,
            TEXCOORD_0,
            TEXCOORD_1,
            TANGENT,
            JOINTS_0,
            WEIGHTS_0,
            COLOR_0,
            COUNT,
        } name;

        uint32_t offset_in_byte = 0;
        uint32_t size_in_byte = 0;
        uint32_t stride = 0;

        bool is_valid() const { return size_in_byte != 0; }
    };

    std::vector<uint32_t> indices;
    std::vector<vec3> positions;
    std::vector<vec3> normals;
    std::vector<vec3> tangents;
    std::vector<vec2> texcoords_0;
    std::vector<vec2> texcoords_1;
    std::vector<ivec4> joints_0;
    std::vector<vec4> weights_0;
    std::vector<vec3> color_0;

    struct MeshSubset {
        ecs::Entity material_id;
        uint32_t index_offset = 0;
        uint32_t index_count = 0;
        AABB local_bound;
    };
    std::vector<MeshSubset> subsets;

    ecs::Entity armature_id;

    // Non-serialized
    mutable RID gpu_resource;
    AABB local_bound;

    VertexAttribute attributes[VertexAttribute::COUNT];
    size_t vertex_buffer_size = 0;  // combine vertex buffer

    void create_render_data();
    std::vector<char> generate_combined_buffer() const;

    void serialize(Archive& archive);
};

//--------------------------------------------------------------------------------------------------
// Material Component
//--------------------------------------------------------------------------------------------------
struct MaterialComponent {
    // @TODO: refactor
    enum TextureSlot {
        TEXTURE_BASE,
        TEXTURE_NORMAL,
        TEXTURE_METALLIC_ROUGHNESS,
        TEXTURE_MAX,
    };

    struct TextureMap {
        std::string name;
    };
    TextureMap textures[TEXTURE_MAX];

    // @TODO: refactor
    float metallic = 0.0f;
    float roughness = 1.0f;
    vec4 base_color = vec4(1);

    // Non-serialized
    mutable RID gpu_resource;

    void serialize(Archive& archive);
};

//--------------------------------------------------------------------------------------------------
// Light Component
//--------------------------------------------------------------------------------------------------
struct LightComponent {
    enum Type {
        LIGHT_TYPE_NONE,
        LIGHT_TYPE_POINT,
        LIGHT_TYPE_OMNI,
        LIGHT_TYPE_MAX,
    };

    vec3 color = vec3(1);
    float energy = 10.0f;
    Type type = LIGHT_TYPE_NONE;

    void serialize(Archive& archive);
};

//--------------------------------------------------------------------------------------------------
// Object Compoinent
//--------------------------------------------------------------------------------------------------
struct ObjectComponent {
    enum FLAGS {
        NONE = 0,
        RENDERABLE = 1 << 0,
        CAST_SHADOW = 1 << 1,
    };

    uint32_t flags = RENDERABLE | CAST_SHADOW;

    /// mesh
    ecs::Entity mesh_id;

    void serialize(Archive& archive);
};

//--------------------------------------------------------------------------------------------------
// Animation Component
//--------------------------------------------------------------------------------------------------
struct AnimationComponent {
    enum : uint32_t {
        NONE = 0,
        PLAYING = 1 << 0,
        LOOPED = 1 << 1,
    };

    struct Channel {
        enum Path {
            PATH_TRANSLATION,
            PATH_ROTATION,
            PATH_SCALE,

            PATH_UNKNOWN,
        };

        Path path = PATH_UNKNOWN;
        ecs::Entity target_id;
        int sampler_index = -1;
    };
    struct Sampler {
        std::vector<float> keyframe_times;
        std::vector<float> keyframe_data;
    };

    bool is_playing() const { return flags & PLAYING; }
    bool is_looped() const { return flags & LOOPED; }
    float get_legnth() const { return end - start; }
    float is_end() const { return timer > end; }

    uint32_t flags = LOOPED;
    float start = 0;
    float end = 0;
    float timer = 0;
    float amount = 1;  // blend amount
    float speed = 1;

    std::vector<Channel> channels;
    std::vector<Sampler> samplers;

    void serialize(Archive& archive);
};

//--------------------------------------------------------------------------------------------------
// Armature Component
//--------------------------------------------------------------------------------------------------
struct ArmatureComponent {
    enum FLAGS {
        NONE = 0,
    };
    uint32_t flags = NONE;

    std::vector<ecs::Entity> bone_collection;
    std::vector<mat4> inverse_bind_matrices;

    // Non-Serialized
    std::vector<mat4> bone_transforms;

    void serialize(Archive& archive);
};

//--------------------------------------------------------------------------------------------------
// Rigid Body Component
//--------------------------------------------------------------------------------------------------
struct RigidBodyComponent {
    enum CollisionShape {
        SHAPE_UNKNOWN,
        SHAPE_SPHERE,
        SHAPE_BOX,
        SHAPE_MAX,
    };

    union Parameter {
        struct {
            vec3 half_size;
        } box;
        struct {
            float radius;
        } sphere;
    };

    float mass = 1.0f;
    CollisionShape shape;
    Parameter param;

    void serialize(Archive& archive);
};

}  // namespace vct
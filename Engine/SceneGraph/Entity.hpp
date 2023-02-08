#pragma once
#include <list>
#include <memory>
#include <string>
#include <vector>

#include "Core/GeomMath.hpp"

struct MeshComponent {
    enum Flag {
        None = 0,
        HAS_UV_FLAG = 1 << 0,
        HAS_NORMAL_FLAG = 1 << 1,
        HAS_TANGENT_FLAG = 1 << 2,
        HAS_BITANGENT_FLAG = 1 << 3,
    };

    uint32_t flags = None;
    std::string name;
    std::vector<vec3> positions;
    std::vector<vec2> uvs;
    std::vector<vec3> normals;
    std::vector<vec3> tangents;
    std::vector<vec3> bitangents;
    std::vector<uint32_t> indices;

    uint32_t materialIdx = static_cast<uint32_t>( -1 );

    mutable void* gpuResource = nullptr;
};

struct MaterialComponent {
    /// only support albedo color for now
    std::string albedoTexture;
    std::string metallicRoughnessTexture;
    std::string normalTexture;
    vec4 albedoColor{ 1 };
    float metallic{ 0.0f };
    float roughness{ 1.0f };
    float reflectPower{ 0.0f };

    mutable void* gpuResource = nullptr;
};

class Entity {
    Entity( Entity& ) = delete;
    Entity( Entity&& ) = delete;
    Entity( const char* name, uint32_t flag )
        : m_name( name ), m_flag( flag ) {}

public:
    enum {
        FLAG_NONE = 0,
        FLAG_GEOMETRY = 0x1,
    };

    virtual ~Entity() = default;

    void GetCalculatedTransform( mat4& out ) const;
    void AddChild( Entity* child );

protected:
public:
    std::string m_name;
    uint32_t m_flag;
    mat4 m_trans = mat4( 1 );

    std::list<Entity*> m_children;
    Entity* m_pParent{ nullptr };
    MeshComponent* m_mesh{ nullptr };
    MaterialComponent* m_material{ nullptr };

    friend class Scene;
};

#include "AssimpLoader.hpp"

#include <filesystem>

#include <assimp/pbrmaterial.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>

#include "Base/Asserts.h"
#include "Base/Logger.h"

#include "com_dvars.h"
#include "Manager/AssetLoader.hpp"

using std::string;
using std::vector;
namespace fs = std::filesystem;

void AssimpLoader::loadGltf( const char* path, Scene& scene, bool flipUVs )
{
    fs::path sysPath = fs::path( Dvar_GetString( fs_base ) ) / path;
    string fullpath = sysPath.string();

    LOG_DEBUG( "[assimp] loading model from '%s'", fullpath.c_str() );

    Assimp::Importer importer;

    unsigned int flag = aiProcess_CalcTangentSpace | aiProcess_Triangulate;
    flag |= flipUVs ? aiProcess_FlipUVs : 0;
    const aiScene* aiscene = importer.ReadFile( fullpath, flag );

    // check for errors
    if ( !aiscene || aiscene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiscene->mRootNode )  // if is Not Zero
    {
        LOG_ERROR( "[assimp] failed to load scene '%s'\n\tdetails: %s", fullpath.c_str(), importer.GetErrorString() );
    }

    const uint32_t numMeshes = aiscene->mNumMeshes;
    const uint32_t numMaterials = aiscene->mNumMaterials;

    LOG_DEBUG( "scene '%s' has %u meshes, %u materials", path, numMeshes, numMaterials );

    // set base path
    fs::path fullsyspath{ fullpath };
    string modelName = fullsyspath.filename().string();
    m_currentPath = fullsyspath.remove_filename().string();

    Entity* model = scene.RegisterEntity( modelName.c_str(), Entity::FLAG_NONE );
    scene.m_root->AddChild( model );

    size_t materialOffset = scene.m_materials.size();
    for ( uint32_t i = 0; i < numMaterials; ++i ) {
        const aiMaterial* aimat = aiscene->mMaterials[i];
        MaterialComponent* mat = processMaterial( aimat );
        scene.m_materials.emplace_back( std::shared_ptr<MaterialComponent>( mat ) );
    }

    for ( uint32_t i = 0; i < numMeshes; ++i ) {
        const aiMesh* aimesh = aiscene->mMeshes[i];
        std::shared_ptr<MeshComponent> mesh( processMesh( aimesh ) );
        const size_t index = materialOffset + mesh->materialIdx;

        AABB box;
        box.Expand( mesh->positions.data(), mesh->positions.size() );

        // slightly enlarge bounding box
        const vec3 offset = vec3( 0.01f );
        box.min -= offset;
        box.max += offset;

        Entity* entity = scene.RegisterEntity( aimesh->mName.C_Str(), Entity::FLAG_GEOMETRY );

        model->AddChild( entity );
        entity->m_material = scene.m_materials.at( index ).get();
        entity->m_mesh = mesh.get();

        scene.m_meshes.emplace_back( mesh );
        scene.m_aabb.Union( box );
    }

    // load images
    auto loadImage = [&scene]( const string& key ) {
        if ( key.empty() ) {
            return false;
        }
        auto image = std::make_shared<Image>();
        if ( !Image::Load( key, *image.get() ) ) {
            return false;
        }
        scene.m_images.insert( std::make_pair( key, image ) );
        return true;
    };

    for ( const auto& material : scene.m_materials ) {
        loadImage( material->albedoTexture );
        loadImage( material->normalTexture );
        loadImage( material->pbrTexture );
    }
}

MaterialComponent* AssimpLoader::processMaterial( const aiMaterial* aimaterial )
{
    MaterialComponent* mat = new MaterialComponent;

    /// albedo
    {
        aiString path;
        if ( aimaterial->GetTexture( AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE, &path ) == AI_SUCCESS ) {
            mat->albedoTexture = m_currentPath;
            mat->albedoTexture.append( path.C_Str() );
        }
        else if ( aimaterial->GetTexture( aiTextureType_DIFFUSE, 0, &path ) == AI_SUCCESS ) {
            mat->albedoTexture = m_currentPath;
            mat->albedoTexture.append( path.C_Str() );
        }
        else {
            LOG_WARN( "[scene] mesh does not have diffuse texture" );
        }
    }

    /// metallic roughness
    {
        aiString path;
        if ( aimaterial->GetTexture( AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &path ) == AI_SUCCESS ) {
            mat->pbrTexture = m_currentPath;
            mat->pbrTexture.append( path.C_Str() );
        }
    }

    /// normal texture
    {
        aiString path;
        if ( aimaterial->GetTexture( aiTextureType_NORMALS, 0, &path ) == AI_SUCCESS ) {
            mat->normalTexture = m_currentPath;
            mat->normalTexture.append( path.C_Str() );
        }
        else if ( aimaterial->GetTexture( aiTextureType_HEIGHT, 0, &path ) == AI_SUCCESS ) {
            mat->normalTexture = m_currentPath;
            mat->normalTexture.append( path.C_Str() );
        }
    }

    return mat;
}

MeshComponent* AssimpLoader::processMesh( const aiMesh* aimesh )
{
    ASSERT( aimesh->mNumVertices );

    MeshComponent* mesh = new MeshComponent;
    mesh->positions.reserve( aimesh->mNumVertices );
    mesh->normals.reserve( aimesh->mNumVertices );
    mesh->uvs.reserve( aimesh->mNumVertices );
    mesh->name = aimesh->mName.C_Str();

    bool hasUv = aimesh->mTextureCoords[0];

    mesh->flags = MeshComponent::HAS_UV_FLAG | MeshComponent::HAS_NORMAL_FLAG;

    for ( uint32_t i = 0; i < aimesh->mNumVertices; ++i ) {
        auto& position = aimesh->mVertices[i];
        mesh->positions.emplace_back( vec3( position.x, position.y, position.z ) );
        auto& normal = aimesh->mNormals[i];
        mesh->normals.emplace_back( vec3( normal.x, normal.y, normal.z ) );

        if ( hasUv ) {
            auto& uv = aimesh->mTextureCoords[0][i];
            mesh->uvs.emplace_back( vec2( uv.x, uv.y ) );
        }
        else {
            mesh->uvs.emplace_back( vec2( 0 ) );
        }
    }

    bool hasTangent = aimesh->mTangents != nullptr;
    if ( hasTangent ) {
        mesh->flags |= MeshComponent::HAS_BITANGENT_FLAG;
        mesh->tangents.reserve( aimesh->mNumVertices );
        mesh->bitangents.reserve( aimesh->mNumVertices );
        for ( uint32_t i = 0; i < aimesh->mNumVertices; ++i ) {
            auto& tangent = aimesh->mTangents[i];
            mesh->tangents.emplace_back( vec3( tangent.x, tangent.y, tangent.z ) );
            auto& bitangent = aimesh->mBitangents[i];
            mesh->bitangents.emplace_back( vec3( bitangent.x, bitangent.y, bitangent.z ) );
        }
    }

    mesh->indices.reserve( aimesh->mNumFaces * 3 );
    for ( uint32_t i = 0; i < aimesh->mNumFaces; ++i ) {
        aiFace& face = aimesh->mFaces[i];
        mesh->indices.emplace_back( face.mIndices[0] );
        mesh->indices.emplace_back( face.mIndices[1] );
        mesh->indices.emplace_back( face.mIndices[2] );
    }

    mesh->materialIdx = aimesh->mMaterialIndex;

    return mesh;
}
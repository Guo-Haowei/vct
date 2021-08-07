#include "SceneLoader.h"

#include <assert.h>
#include <assimp/pbrmaterial.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>

#include "common/com_filesystem.h"
#include "universal/core_assert.h"
#include "universal/print.h"

using std::string;
using std::vector;

namespace vct {

void SceneLoader::loadGltf( const char* path, Scene& scene, const mat4& transform, bool flipUVs )
{
    char fullpath[kMaxOSPath];
    Com_FsBuildPath( fullpath, kMaxOSPath, path, "data/models" );
    Com_Printf( "[assimp] loading model from '%s'", fullpath );

    Assimp::Importer importer;

    unsigned int flag = aiProcess_CalcTangentSpace | aiProcess_Triangulate;
    flag |= flipUVs ? aiProcess_FlipUVs : 0;
    const aiScene* aiscene = importer.ReadFile( fullpath, flag );

    // check for errors
    if ( !aiscene || aiscene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiscene->mRootNode )  // if is Not Zero
    {
        Com_PrintError( "[assimp] failed to load scene '%s'\n\tdetails: %s", fullpath, importer.GetErrorString() );
    }

    const uint32_t numMeshes    = aiscene->mNumMeshes;
    const uint32_t numMaterials = aiscene->mNumMaterials;

    Com_Printf( "scene '%s' has %u meshes, %u materials", path, numMeshes, numMaterials );

    // set base path
    m_currentPath = fullpath;
    auto found    = m_currentPath.find_last_of( "/\\" );
    m_currentPath = m_currentPath.substr( 0, found + 1 );

    size_t materialOffset = scene.materials.size();

    for ( uint32_t i = 0; i < numMaterials; ++i )
    {
        const aiMaterial* aimat = aiscene->mMaterials[i];
        Material* mat           = processMaterial( aimat );
        scene.materials.emplace_back( std::shared_ptr<Material>( mat ) );
    }

    GeometryNode node;
    // node.transform = transform;
    node.transform = mat4( 1 );

    for ( uint32_t i = 0; i < numMeshes; ++i )
    {
        const aiMesh* aimesh = aiscene->mMeshes[i];
        MeshComponent* mesh  = processMesh( aimesh );
        size_t index         = materialOffset + mesh->materialIdx;
        Material* mat        = scene.materials.at( index ).get();
        AABB box;

        for ( vec3& position : mesh->positions )
        {
            position = transform * vec4( position, 1.0 );
        }
        for ( vec3& normal : mesh->normals )
        {
            normal = mat3( transform ) * normal;
        }

        box.Expand( mesh->positions.data(), mesh->positions.size() );
        // box.ApplyMatrix( transform );
        node.geometries.push_back( { mesh, mat, box } );
        scene.meshes.emplace_back( std::shared_ptr<MeshComponent>( mesh ) );

        scene.boundingBox.Union( box );
    }

    scene.geometryNodes.push_back( node );
}

Material* SceneLoader::processMaterial( const aiMaterial* aimaterial )
{
    Material* mat = new Material;

    /// albedo
    {
        aiString path;
        if ( aimaterial->GetTexture( AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE, &path ) == AI_SUCCESS )
        {
            mat->albedoTexture = m_currentPath;
            mat->albedoTexture.append( path.C_Str() );
        }
    }

    /// metallic roughness
    {
        aiString path;
        if ( aimaterial->GetTexture( AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &path ) == AI_SUCCESS )
        {
            mat->metallicRoughnessTexture = m_currentPath;
            mat->metallicRoughnessTexture.append( path.C_Str() );
        }
    }

    /// normal texture
    {
        aiString path;
        if ( aimaterial->GetTexture( aiTextureType_NORMALS, 0, &path ) == AI_SUCCESS )
        {
            mat->normalTexture = m_currentPath;
            mat->normalTexture.append( path.C_Str() );
        }
    }

    return mat;
}

MeshComponent* SceneLoader::processMesh( const aiMesh* aimesh )
{
    core_assert( aimesh->mNumVertices );

    string name         = aimesh->mName.C_Str();
    MeshComponent* mesh = new MeshComponent;
    mesh->positions.reserve( aimesh->mNumVertices );
    mesh->normals.reserve( aimesh->mNumVertices );
    mesh->uvs.reserve( aimesh->mNumVertices );

    bool hasUv = aimesh->mTextureCoords[0];

    mesh->flags = MeshComponent::HAS_UV_FLAG | MeshComponent::HAS_NORMAL_FLAG;

    for ( uint32_t i = 0; i < aimesh->mNumVertices; ++i )
    {
        auto& position = aimesh->mVertices[i];
        mesh->positions.emplace_back( vec3( position.x, position.y, position.z ) );
        auto& normal = aimesh->mNormals[i];
        mesh->normals.emplace_back( vec3( normal.x, normal.y, normal.z ) );

        if ( hasUv )
        {
            auto& uv = aimesh->mTextureCoords[0][i];
            mesh->uvs.emplace_back( vec2( uv.x, uv.y ) );
        }
        else
        {
            mesh->uvs.emplace_back( vec2( 0 ) );
        }
    }

    bool hasTangent = aimesh->mTangents != nullptr;
    if ( aimesh->mTangents )
    {
        mesh->flags |= MeshComponent::HAS_BITANGENT_FLAG;
        mesh->tangents.reserve( aimesh->mNumVertices );
        mesh->bitangents.reserve( aimesh->mNumVertices );
        for ( uint32_t i = 0; i < aimesh->mNumVertices; ++i )
        {
            auto& tangent = aimesh->mTangents[i];
            mesh->tangents.emplace_back( vec3( tangent.x, tangent.y, tangent.z ) );
            auto& bitangent = aimesh->mBitangents[i];
            mesh->bitangents.emplace_back( vec3( bitangent.x, bitangent.y, bitangent.z ) );
        }
    }

    mesh->indices.reserve( aimesh->mNumFaces * 3 );
    for ( uint32_t i = 0; i < aimesh->mNumFaces; ++i )
    {
        aiFace& face = aimesh->mFaces[i];
        mesh->indices.emplace_back( face.mIndices[0] );
        mesh->indices.emplace_back( face.mIndices[1] );
        mesh->indices.emplace_back( face.mIndices[2] );
    }

    mesh->materialIdx = aimesh->mMaterialIndex;

    return mesh;
}

}  // namespace vct

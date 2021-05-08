#include "SceneLoader.h"

#include <assert.h>
#include <assimp/pbrmaterial.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>

#include "universal/core_assert.h"
#include "universal/print.h"

using std::string;
using std::vector;

namespace vct {

void SceneLoader::loadGltf( const char* path, Scene& scene, const mat4& transform, bool flipUVs )
{
    Com_Printf( "[assimp] loading model from '%s'", path );
    Assimp::Importer importer;

    unsigned int flag = aiProcess_CalcTangentSpace | aiProcess_Triangulate;
    flag |= flipUVs ? aiProcess_FlipUVs : 0;

    const aiScene* aiscene = importer.ReadFile( path, flag );

    // check for errors
    if ( !aiscene || aiscene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiscene->mRootNode )  // if is Not Zero
    {
        // std::cout << "[ERROR] assimp failed to load.";
        // std::cout << importer.GetErrorString() << std::endl;
        // string error( "assimp: failed to load scene [" );
        // error.append( path ).append( "]\n\t" ).append( importer.GetErrorString() );
        // THROW_EXCEPTION( error );
        core_assert( 0 && "TODO: check parsing error" );
    }

    // set base path
    m_currentPath = path;
    auto found    = m_currentPath.find_last_of( "/\\" );
    m_currentPath = m_currentPath.substr( 0, found + 1 );

    size_t materialOffset = scene.materials.size();

    for ( uint32_t i = 0; i < aiscene->mNumMaterials; ++i )
    {
        const aiMaterial* aimat = aiscene->mMaterials[i];
        Material* mat           = processMaterial( aimat );
        scene.materials.push_back( std::move( std::unique_ptr<Material>( mat ) ) );
    }

    GeometryNode node;
    node.transform = transform;

    for ( uint32_t i = 0; i < aiscene->mNumMeshes; ++i )
    {
        const aiMesh* aimesh = aiscene->mMeshes[i];
        Mesh* mesh           = processMesh( aimesh );
        size_t index         = materialOffset + mesh->materialIndex;
        Material* mat        = scene.materials.at( index ).get();
        Box3 box;
        box.Expand( mesh->positions.data(), mesh->positions.size() );
        box.ApplyMatrix( transform );
        node.geometries.push_back( { mesh, mat, box } );
        scene.meshes.push_back( std::move( std::unique_ptr<Mesh>( mesh ) ) );

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

Mesh* SceneLoader::processMesh( const aiMesh* aimesh )
{
    string name = aimesh->mName.C_Str();
    Mesh* mesh  = new Mesh;
    mesh->positions.reserve( aimesh->mNumVertices );
    mesh->normals.reserve( aimesh->mNumVertices );
    mesh->uvs.reserve( aimesh->mNumVertices );

    bool hasUv = aimesh->mTextureCoords[0];

    for ( uint32_t i = 0; i < aimesh->mNumVertices; ++i )
    {
        auto& position = aimesh->mVertices[i];
        mesh->positions.push_back( vec3( position.x, position.y, position.z ) );
        auto& normal = aimesh->mNormals[i];
        mesh->normals.push_back( vec3( normal.x, normal.y, normal.z ) );

        if ( hasUv )
        {
            auto& uv = aimesh->mTextureCoords[0][i];
            mesh->uvs.push_back( vec2( uv.x, uv.y ) );
        }
        else
        {
            mesh->uvs.push_back( vec2( 0 ) );
        }
    }

    bool hasTangent = aimesh->mTangents != nullptr;
    if ( aimesh->mTangents )
    {
        mesh->tangents.reserve( aimesh->mNumVertices );
        mesh->bitangents.reserve( aimesh->mNumVertices );
        for ( uint32_t i = 0; i < aimesh->mNumVertices; ++i )
        {
            auto& tangent = aimesh->mTangents[i];
            mesh->tangents.push_back( vec3( tangent.x, tangent.y, tangent.z ) );
            auto& bitangent = aimesh->mBitangents[i];
            mesh->bitangents.push_back( vec3( bitangent.x, bitangent.y, bitangent.z ) );
        }
    }

    mesh->faces.reserve( aimesh->mNumFaces );
    for ( uint32_t i = 0; i < aimesh->mNumFaces; ++i )
    {
        aiFace& face = aimesh->mFaces[i];
        mesh->faces.push_back( uvec3( face.mIndices[0], face.mIndices[1], face.mIndices[2] ) );
    }

    mesh->materialIndex = aimesh->mMaterialIndex;

    return mesh;
}

}  // namespace vct

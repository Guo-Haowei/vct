#include "r_editor.h"

#include "GlslProgram.h"
#include "GpuTexture.h"
#include "common/com_filesystem.h"
#include "common/com_system.h"
#include "common/geometry.h"
#include "gl_utils.h"
#include "r_cbuffers.h"
#include "universal/universal.h"

using namespace vct;

struct VertexPoint3D {
    vec3 position;
    vec3 color;
};

static GlslProgram g_lineProgram;
static GlslProgram g_imageProgram;
static MeshData g_boxWireFrame;
static MeshData g_gridWireFrame;
static MeshData g_imageBuffer;
static GpuTexture g_lightTexture;

struct TextureVertex {
    vec2 pos;
    vec2 uv;
};

static void CreateImageBuffer()
{
    MeshData& mesh = g_imageBuffer;
    glGenVertexArrays( 1, &mesh.vao );
    glGenBuffers( 1, mesh.vbos );
    glBindVertexArray( mesh.vao );
    glBindBuffer( GL_ARRAY_BUFFER, mesh.vbos[0] );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof( TextureVertex ), (void*)0 );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof( TextureVertex ), (void*)( sizeof( vec2 ) ) );
    glEnableVertexAttribArray( 1 );
    glBindVertexArray( 0 );
}

static void CreateBoxWireFrameData()
{
    std::vector<VertexPoint3D> vertices;
    const MeshComponent box = geometry::MakeBoxWireFrame();
    vertices.reserve( box.positions.size() );
    for ( const vec3& pos : box.positions )
    {
        vertices.emplace_back( VertexPoint3D{ pos, vec3( 1 ) } );
    }

    MeshData& mesh = g_boxWireFrame;

    glGenVertexArrays( 1, &mesh.vao );
    glGenBuffers( 2, &mesh.ebo );
    glBindVertexArray( mesh.vao );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh.ebo );
    glBindBuffer( GL_ARRAY_BUFFER, mesh.vbos[0] );

    gl::NamedBufferStorage( mesh.vbos[0], vertices );
    gl::NamedBufferStorage( mesh.ebo, box.indices );

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( VertexPoint3D ), (void*)0 );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( VertexPoint3D ), (void*)( sizeof( vec3 ) ) );
    glEnableVertexAttribArray( 1 );

    glBindVertexArray( 0 );

    g_boxWireFrame.count = uint32_t( box.indices.size() );
}

void R_CreateEditorResource()
{
    g_lineProgram.Create( gl::CreateShaderProgram( ProgramCreateInfo::VSPS( "editor/line3d" ) ) );
    g_imageProgram.Create( gl::CreateShaderProgram( ProgramCreateInfo::VSPS( "editor/image" ) ) );

    CreateBoxWireFrameData();
    CreateImageBuffer();

    char buffer[kMaxOSPath];
    Com_FsBuildPath( buffer, kMaxOSPath, "pointlight.png", "data/images" );
    g_lightTexture.create2DImageFromFile( buffer );
}

void R_DestroyEditorResource()
{
    g_lineProgram.Destroy();
    g_imageProgram.Destroy();

    g_lightTexture.destroy();

    glDeleteVertexArrays( 1, &g_boxWireFrame.vao );
    glDeleteBuffers( 2, &g_boxWireFrame.ebo );
}

static inline void FillTextureIconBuffer( std::vector<TextureVertex>& iconBuffer, const vec2& offset, float aspect )
{
    constexpr TextureVertex kVertices[] = {
        { vec2( -1, +1 ), vec2( 0, 0 ) },  // top-left
        { vec2( -1, -1 ), vec2( 0, 1 ) },  // bottom-left
        { vec2( +1, -1 ), vec2( 1, 1 ) },  // bottom-right
        { vec2( +1, +1 ), vec2( 1, 0 ) },  // top-right
    };

    constexpr uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };
    constexpr float kScale       = 0.07f;
    const vec2 scale( kScale, aspect * kScale );

    for ( size_t idx = 0; idx < array_length( indices ); ++idx )
    {
        TextureVertex vertex = kVertices[indices[idx]];
        vertex.pos *= scale;
        vertex.pos += offset;
        iconBuffer.emplace_back( vertex );
    }
}

// draw grid, bounding box, ui
void R_DrawEditor()
{
    const Scene& scene = Com_GetScene();

    if ( const Geometry* node = scene.selected )
    {
        const AABB box = node->boundingBox;
        const mat4 M   = glm::translate( mat4( 1 ), box.Center() ) * glm::scale( mat4( 1 ), box.Size() );

        g_lineProgram.use();
        glBindVertexArray( g_boxWireFrame.vao );
        g_perframeCache.cache.PVM = g_perframeCache.cache.PV * M;
        g_perframeCache.Update();
        glDrawElements( GL_LINES, g_boxWireFrame.count, GL_UNSIGNED_INT, 0 );
    }

    // draw light
    const Light& light   = scene.light;
    const Camera& camera = scene.camera;
    const mat4 P         = camera.perspective();
    const mat4 V         = camera.view();

    constexpr float distance = 10.0f;
    vec4 lightPos            = vec4( light.direction * distance, 1.0 );
    lightPos                 = P * ( V * lightPos );
    if ( lightPos.z > 0.0f )
    {
        lightPos /= lightPos.w;
        const vec2 lightPos2d( P * lightPos );
        std::vector<TextureVertex> iconBuffer;

        FillTextureIconBuffer( iconBuffer, lightPos2d, camera.aspect );
        glNamedBufferData( g_imageBuffer.vbos[0], sizeof( TextureVertex ) * iconBuffer.size(), iconBuffer.data(), GL_STREAM_DRAW );

        glActiveTexture( GL_TEXTURE0 );
        g_lightTexture.bind();

        g_imageProgram.use();
        glBindVertexArray( g_imageBuffer.vao );
        glDrawArrays( GL_TRIANGLES, 0, 6 );
    }
}
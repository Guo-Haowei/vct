#include "r_editor.h"

#include "GlslProgram.h"
#include "common/com_system.h"
#include "common/geometry.h"
#include "gl_utils.h"
#include "r_cbuffers.h"

using namespace vct;

struct VertexPoint3D {
    vec3 position;
    vec3 color;
};

static GlslProgram g_lineProgram;
static MeshData g_boxWireFrame;
static MeshData g_gridWireFrame;

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
    CreateBoxWireFrameData();
}

void R_DestroyEditorResource()
{
    g_lineProgram.Destroy();
    glDeleteVertexArrays( 1, &g_boxWireFrame.vao );
    glDeleteBuffers( 2, &g_boxWireFrame.ebo );
}

// draw grid, bounding box, ui
void R_DrawEditor()
{
    const Scene& scene = Com_GetScene();
    if ( const Geometry* node = scene.selected )
    {
        const Box3 box = node->boundingBox;
        const mat4 M   = glm::translate( mat4( 1 ), box.Center() ) * glm::scale( mat4( 1 ), box.Size() );

        g_lineProgram.use();
        glBindVertexArray( g_boxWireFrame.vao );
        g_perframeCache.cache.PVM = g_perframeCache.cache.PV * M;
        g_perframeCache.Update();
        glDrawElements( GL_LINES, g_boxWireFrame.count, GL_UNSIGNED_INT, 0 );
    }
}
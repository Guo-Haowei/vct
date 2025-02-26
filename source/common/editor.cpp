#include "editor.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "com_dvars.h"
#include "common/com_misc.h"
#include "common/main_window.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "renderer/r_cbuffers.h"
#include "universal/core_assert.h"
#include "universal/dvar_api.h"
#include "universal/print.h"

class Editor {
    ImVec2 pos;
    ImVec2 size;

    void DbgWindow();
    void DockSpace();

    Editor() = default;

   public:
    static Editor& Singleton()
    {
        static Editor editor;
        return editor;
    }

    void Update();
};

static const char* DrawTextureToStr( int mode )
{
    const char* str = "scene";
    switch ( mode )
    {
        case TEXTURE_VOXEL_ALBEDO:
            str = "voxel color";
            break;
        case TEXTURE_VOXEL_NORMAL:
            str = "voxel normal";
            break;
        case TEXTURE_GBUFFER_DEPTH:
            str = "depth";
            break;
        case TEXTURE_GBUFFER_ALBEDO:
            str = "albedo";
            break;
        case TEXTURE_GBUFFER_NORMAL:
            str = "normal";
            break;
        case TEXTURE_GBUFFER_METALLIC:
            str = "metallic";
            break;
        case TEXTURE_GBUFFER_ROUGHNESS:
            str = "roughness";
            break;
        case TEXTURE_GBUFFER_SHADOW:
            str = "shadow";
            break;
        case TEXTURE_SSAO:
            str = "ssao";
            break;
        default:
            break;
    }
    return str;
}

void Editor::DbgWindow()
{
    if ( ImGui::Begin( "Debug" ) )
    {
        Scene& scene         = Com_GetScene();
        const Camera& camera = scene.camera;
        bool dirty           = false;

        const vec3& eye = camera.position;
        ImGui::Text( "eye: %.2f, %.2f, %.2f", eye.x, eye.y, eye.z );
        ImGui::Separator();

        ImGui::Text( "Voxel GI" );
        ImGui::Checkbox( "Enable GI", (bool*)( Dvar_GetPtr( r_enableVXGI ) ) );
        ImGui::Checkbox( "No Texture", (bool*)( Dvar_GetPtr( r_noTexture ) ) );
        dirty |= ImGui::Checkbox( "Force Voxel GI texture update", (bool*)( Dvar_GetPtr( r_forceVXGI ) ) );
        ImGui::Separator();

        ImGui::Text( "CSM" );
        ImGui::Checkbox( "Debug CSM", (bool*)( Dvar_GetPtr( r_debugCSM ) ) );
        ImGui::Separator();

        ImGui::Text( "SSAO" );
        ImGui::Checkbox( "Enable SSAO", (bool*)( Dvar_GetPtr( r_enableSsao ) ) );
        ImGui::Text( "SSAO Kernal Radius" );
        ImGui::SliderFloat( "Kernal Radius", (float*)( Dvar_GetPtr( r_ssaoKernelRadius ) ), 0.1f, 5.0f );
        ImGui::Separator();

        ImGui::Text( "FXAA" );
        ImGui::Checkbox( "Enable FXAA", (bool*)( Dvar_GetPtr( r_enableFXAA ) ) );
        ImGui::Separator();

        ImGui::Text( "Display Texture" );
        ImGui::SliderInt( "Display Texture", (int*)( Dvar_GetPtr( r_debugTexture ) ), DrawTexture::TEXTURE_FINAL_IMAGE, DrawTexture::TEXTURE_MAX );
        ImGui::Text( "%s", DrawTextureToStr( Dvar_GetInt( r_debugTexture ) ) );

        ImGui::Separator();
        ImGui::Text( "Light" );
        float* lightDir = (float*)Dvar_GetPtr( light_dir );
        dirty |= ImGui::SliderFloat( "x", lightDir, -20.f, 20.f );
        dirty |= ImGui::SliderFloat( "z", lightDir + 2, -20.f, 20.f );

        ImGui::Text( "Floor" );
        if ( scene.selected )
        {
            void* mat              = scene.selected->material->gpuResource;
            MaterialData* drawData = reinterpret_cast<MaterialData*>( mat );
            dirty |= ImGui::SliderFloat( "metallic", &drawData->metallic, 0.0f, 1.0f );
            dirty |= ImGui::SliderFloat( "roughness", &drawData->roughness, 0.0f, 1.0f );
        }

        scene.light.direction = glm::normalize( Dvar_GetVec3( light_dir ) );
        scene.dirty           = dirty;

        ImGui::End();
    }
}

void Editor::DockSpace()
{
    ImGui::GetMainViewport();

    static bool opt_padding                   = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos( viewport->WorkPos );
    ImGui::SetNextWindowSize( viewport->WorkSize );
    ImGui::SetNextWindowViewport( viewport->ID );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    if ( dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode )
    {
        window_flags |= ImGuiWindowFlags_NoBackground;
    }

    if ( !opt_padding )
    {
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
    }
    ImGui::Begin( "DockSpace Demo", nullptr, window_flags );
    if ( !opt_padding )
    {
        ImGui::PopStyleVar();
    }

    ImGui::PopStyleVar( 2 );

    // Submit the DockSpace
    ImGuiIO& io          = ImGui::GetIO();
    ImGuiID dockspace_id = ImGui::GetID( "MyDockSpace" );
    ImGui::DockSpace( dockspace_id, ImVec2( 0.0f, 0.0f ), dockspace_flags );

    const auto* node = ImGui::DockBuilderGetCentralNode( dockspace_id );
    core_assert( node );

    pos  = node->Pos;
    size = node->Size;

    if ( ImGui::BeginMenuBar() )
    {
        if ( ImGui::BeginMenu( "Options" ) )
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            ImGui::MenuItem( "Padding", NULL, &opt_padding );
            ImGui::Separator();

            if ( ImGui::MenuItem( "Flag: NoSplit", "", ( dockspace_flags & ImGuiDockNodeFlags_NoSplit ) != 0 ) )
            {
                dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
            }
            if ( ImGui::MenuItem( "Flag: NoResize", "", ( dockspace_flags & ImGuiDockNodeFlags_NoResize ) != 0 ) )
            {
                dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
            }
            if ( ImGui::MenuItem( "Flag: NoDockingInCentralNode", "", ( dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode ) != 0 ) )
            {
                dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
            }
            if ( ImGui::MenuItem( "Flag: AutoHideTabBar", "", ( dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar ) != 0 ) )
            {
                dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
            }
            if ( ImGui::MenuItem( "Flag: PassthruCentralNode", "", ( dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode ) != 0 ) )
            {
                dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
            }
            ImGui::Separator();

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::End();
}

void Editor::Update()
{
    static bool hideUI = false;
    if ( ImGui::IsKeyPressed( GLFW_KEY_ESCAPE ) )
    {
        hideUI = !hideUI;
    }

    if ( !hideUI )
    {
        DockSpace();
        DbgWindow();
    }

    ImGuiIO& io  = ImGui::GetIO();
    Scene& scene = Com_GetScene();

    // select object
    if ( !io.WantCaptureMouse && MainWindow::IsMouseInScreen() )
    {
        const vec2 mousePos = MainWindow::MousePos();
        const ivec2 extent  = MainWindow::FrameSize();
        if ( ImGui::IsMouseClicked( GLFW_MOUSE_BUTTON_1 ) )
        {
            const Camera& camera = scene.camera;

            const mat4& PV   = camera.ProjView();
            const mat4 invPV = glm::inverse( PV );
            vec2 pos( mousePos.x / extent.x, 1.0f - mousePos.y / extent.y );
            pos -= 0.5f;
            pos *= 2.0f;

            Ray ray{ camera.position, glm::normalize( vec3( invPV * vec4( pos.x, pos.y, 1.0f, 1.0f ) ) ) };

            for ( const auto& node : scene.geometryNodes )
            {
                for ( const auto& geom : node.geometries )
                {
                    if ( !geom.visible )
                    {
                        continue;
                    }
                    const AABB& box  = geom.boundingBox;
                    const auto& mesh = geom.mesh;
                    if ( ray.Intersects( box ) )
                    {
                        for ( uint32_t idx = 0; idx < mesh->indices.size(); )
                        {
                            const vec3& a = mesh->positions[mesh->indices[idx++]];
                            const vec3& b = mesh->positions[mesh->indices[idx++]];
                            const vec3& c = mesh->positions[mesh->indices[idx++]];
                            if ( ray.Intersects( a, b, c ) )
                            {
                                scene.selected = &geom;
                            }
                        }
                    }
                }
            }
        }
        else if ( ImGui::IsMouseClicked( GLFW_MOUSE_BUTTON_2 ) )
        {
            scene.selected = nullptr;
        }
    }

    if ( scene.selected && ImGui::IsKeyPressed( GLFW_KEY_DELETE ) )
    {
        if ( scene.selected->visible )
        {
            Com_PrintWarning( "material %s deleted", scene.selected->mesh->name.c_str() );

            scene.selected->visible = false;
            scene.dirty             = true;
            scene.selected          = nullptr;
        }
    }
}

void EditorSetup()
{
    Editor::Singleton().Update();
}

void EditorSetupStyle()
{
    ImGuiStyle* style = &ImGui::GetStyle();
    ImVec4* colors    = style->Colors;

    colors[ImGuiCol_Text]                  = ImVec4( 1.000f, 1.000f, 1.000f, 1.000f );
    colors[ImGuiCol_TextDisabled]          = ImVec4( 0.500f, 0.500f, 0.500f, 1.000f );
    colors[ImGuiCol_WindowBg]              = ImVec4( 0.180f, 0.180f, 0.180f, 1.000f );
    colors[ImGuiCol_ChildBg]               = ImVec4( 0.280f, 0.280f, 0.280f, 0.000f );
    colors[ImGuiCol_PopupBg]               = ImVec4( 0.313f, 0.313f, 0.313f, 1.000f );
    colors[ImGuiCol_Border]                = ImVec4( 0.266f, 0.266f, 0.266f, 1.000f );
    colors[ImGuiCol_BorderShadow]          = ImVec4( 0.000f, 0.000f, 0.000f, 0.000f );
    colors[ImGuiCol_FrameBg]               = ImVec4( 0.160f, 0.160f, 0.160f, 1.000f );
    colors[ImGuiCol_FrameBgHovered]        = ImVec4( 0.200f, 0.200f, 0.200f, 1.000f );
    colors[ImGuiCol_FrameBgActive]         = ImVec4( 0.280f, 0.280f, 0.280f, 1.000f );
    colors[ImGuiCol_TitleBg]               = ImVec4( 0.148f, 0.148f, 0.148f, 1.000f );
    colors[ImGuiCol_TitleBgActive]         = ImVec4( 0.148f, 0.148f, 0.148f, 1.000f );
    colors[ImGuiCol_TitleBgCollapsed]      = ImVec4( 0.148f, 0.148f, 0.148f, 1.000f );
    colors[ImGuiCol_MenuBarBg]             = ImVec4( 0.195f, 0.195f, 0.195f, 1.000f );
    colors[ImGuiCol_ScrollbarBg]           = ImVec4( 0.160f, 0.160f, 0.160f, 1.000f );
    colors[ImGuiCol_ScrollbarGrab]         = ImVec4( 0.277f, 0.277f, 0.277f, 1.000f );
    colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4( 0.300f, 0.300f, 0.300f, 1.000f );
    colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4( 1.000f, 0.391f, 0.000f, 1.000f );
    colors[ImGuiCol_CheckMark]             = ImVec4( 1.000f, 1.000f, 1.000f, 1.000f );
    colors[ImGuiCol_SliderGrab]            = ImVec4( 0.391f, 0.391f, 0.391f, 1.000f );
    colors[ImGuiCol_SliderGrabActive]      = ImVec4( 1.000f, 0.391f, 0.000f, 1.000f );
    colors[ImGuiCol_Button]                = ImVec4( 1.000f, 1.000f, 1.000f, 0.000f );
    colors[ImGuiCol_ButtonHovered]         = ImVec4( 1.000f, 1.000f, 1.000f, 0.156f );
    colors[ImGuiCol_ButtonActive]          = ImVec4( 1.000f, 1.000f, 1.000f, 0.391f );
    colors[ImGuiCol_Header]                = ImVec4( 0.313f, 0.313f, 0.313f, 1.000f );
    colors[ImGuiCol_HeaderHovered]         = ImVec4( 0.469f, 0.469f, 0.469f, 1.000f );
    colors[ImGuiCol_HeaderActive]          = ImVec4( 0.469f, 0.469f, 0.469f, 1.000f );
    colors[ImGuiCol_Separator]             = colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered]      = ImVec4( 0.391f, 0.391f, 0.391f, 1.000f );
    colors[ImGuiCol_SeparatorActive]       = ImVec4( 1.000f, 0.391f, 0.000f, 1.000f );
    colors[ImGuiCol_ResizeGrip]            = ImVec4( 1.000f, 1.000f, 1.000f, 0.250f );
    colors[ImGuiCol_ResizeGripHovered]     = ImVec4( 1.000f, 1.000f, 1.000f, 0.670f );
    colors[ImGuiCol_ResizeGripActive]      = ImVec4( 1.000f, 0.391f, 0.000f, 1.000f );
    colors[ImGuiCol_Tab]                   = ImVec4( 0.098f, 0.098f, 0.098f, 1.000f );
    colors[ImGuiCol_TabHovered]            = ImVec4( 0.352f, 0.352f, 0.352f, 1.000f );
    colors[ImGuiCol_TabActive]             = ImVec4( 0.195f, 0.195f, 0.195f, 1.000f );
    colors[ImGuiCol_TabUnfocused]          = ImVec4( 0.098f, 0.098f, 0.098f, 1.000f );
    colors[ImGuiCol_TabUnfocusedActive]    = ImVec4( 0.195f, 0.195f, 0.195f, 1.000f );
    colors[ImGuiCol_DockingPreview]        = ImVec4( 1.000f, 0.391f, 0.000f, 0.781f );
    colors[ImGuiCol_DockingEmptyBg]        = ImVec4( 0.180f, 0.180f, 0.180f, 1.000f );
    colors[ImGuiCol_PlotLines]             = ImVec4( 0.469f, 0.469f, 0.469f, 1.000f );
    colors[ImGuiCol_PlotLinesHovered]      = ImVec4( 1.000f, 0.391f, 0.000f, 1.000f );
    colors[ImGuiCol_PlotHistogram]         = ImVec4( 0.586f, 0.586f, 0.586f, 1.000f );
    colors[ImGuiCol_PlotHistogramHovered]  = ImVec4( 1.000f, 0.391f, 0.000f, 1.000f );
    colors[ImGuiCol_TextSelectedBg]        = ImVec4( 1.000f, 1.000f, 1.000f, 0.156f );
    colors[ImGuiCol_DragDropTarget]        = ImVec4( 1.000f, 0.391f, 0.000f, 1.000f );
    colors[ImGuiCol_NavHighlight]          = ImVec4( 1.000f, 0.391f, 0.000f, 1.000f );
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4( 1.000f, 0.391f, 0.000f, 1.000f );
    colors[ImGuiCol_NavWindowingDimBg]     = ImVec4( 0.000f, 0.000f, 0.000f, 0.586f );
    colors[ImGuiCol_ModalWindowDimBg]      = ImVec4( 0.000f, 0.000f, 0.000f, 0.586f );

    style->ChildRounding     = 4.0f;
    style->FrameBorderSize   = 1.0f;
    style->FrameRounding     = 2.0f;
    style->GrabMinSize       = 7.0f;
    style->PopupRounding     = 2.0f;
    style->ScrollbarRounding = 12.0f;
    style->ScrollbarSize     = 13.0f;
    style->TabBorderSize     = 1.0f;
    style->TabRounding       = 0.0f;
    style->WindowRounding    = 4.0f;

    ImGuiIO& io = ImGui::GetIO();
    if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
    {
        style->WindowRounding              = 0.0f;
        style->Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
}

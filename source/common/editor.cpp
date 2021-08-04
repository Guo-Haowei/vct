#include "editor.h"

#include "Globals.h"
#include "common/com_system.h"
#include "imgui/imgui.h"
#include "universal/core_assert.h"

using namespace vct;

UIControlls g_UIControls;

static void EditorConsole();

void EditorSelectObject()
{
    Scene& scene = Com_GetScene();
    ImGui::Begin( "Debug" );
    ImGui::Text( "Voxel GI" );
    ImGui::SameLine();
    ImGui::RadioButton( "On", &g_UIControls.voxelGiMode, 1 );
    ImGui::SameLine();
    ImGui::RadioButton( "Off", &g_UIControls.voxelGiMode, 0 );
    ImGui::Separator();

    ImGui::Text( "Voxel Texture Size: %d", VOXEL_TEXTURE_SIZE );
    ImGui::Text( "Voxel Mip Level: %d", VOXEL_TEXTURE_MIP_LEVEL );
    if ( ImGui::Checkbox( "Force voxel texture update", &g_UIControls.forceUpdateVoxelTexture ) )
        scene.dirty = true;
    ImGui::Separator();
    ImGui::SliderInt( "Voxel Mipmap Level", &g_UIControls.voxelMipLevel, 0, VOXEL_TEXTURE_MIP_LEVEL - 1 );
    ImGui::Separator();

    ImGui::Text( "Debug Bounding Box" );
    if ( ImGui::Checkbox( "Show Object Bounding Box", &g_UIControls.showObjectBoundingBox ) )
        g_UIControls.showWorldBoundingBox = false;
    if ( ImGui::Checkbox( "Show World Bounding Box", &g_UIControls.showWorldBoundingBox ) )
        g_UIControls.showObjectBoundingBox = false;

    ImGui::Separator();

    ImGui::Text( "Debug Texture" );
    ImGui::RadioButton( "Voxel GI", &g_UIControls.drawTexture, DrawTexture::TEXTURE_FINAL_IMAGE );
    ImGui::RadioButton( "Voxel Albedo", &g_UIControls.drawTexture, DrawTexture::TEXTURE_VOXEL_ALBEDO );
    ImGui::RadioButton( "Voxel Normal", &g_UIControls.drawTexture, DrawTexture::TEXTURE_VOXEL_NORMAL );
    ImGui::RadioButton( "Gbuffer Depth", &g_UIControls.drawTexture, DrawTexture::TEXTURE_GBUFFER_DEPTH );
    ImGui::RadioButton( "Gbuffer Albedo", &g_UIControls.drawTexture, DrawTexture::TEXTURE_GBUFFER_ALBEDO );
    ImGui::RadioButton( "Gbuffer Normal", &g_UIControls.drawTexture, DrawTexture::TEXTURE_GBUFFER_NORMAL );
    ImGui::RadioButton( "Gbuffer Metallic", &g_UIControls.drawTexture, DrawTexture::TEXTURE_GBUFFER_METALLIC );
    ImGui::RadioButton( "Gbuffer Roughness", &g_UIControls.drawTexture, DrawTexture::TEXTURE_GBUFFER_ROUGHNESS );
    ImGui::RadioButton( "Gbuffer Shadow", &g_UIControls.drawTexture, DrawTexture::TEXTURE_GBUFFER_SHADOW );

    ImGui::End();
    // Scene* scene = Com_GetScene();
    // if ( scene == nullptr )
    // {
    //     return;
    // }

    // if ( ImGui::IsMouseClicked( GLFW_MOUSE_BUTTON_2 ) )
    // {
    //     scene->selected = nullptr;
    //     return;
    // }

    // ImGuiIO& io = ImGui::GetIO();
    // if ( ImGui::IsMousePosValid() && ImGui::IsMouseClicked( GLFW_MOUSE_BUTTON_1 ) )
    // {
    //     scene->selected = nullptr;
    //     float x         = io.MousePos.x - s_glob.pos.x;
    //     float y         = io.MousePos.y - s_glob.pos.y;
    //     x               = 2.0f * ( x / s_glob.vp.width ) - 1.0f;
    //     y               = 1.0f - 2.0f * ( y / s_glob.vp.height );

    //     if ( !( x > -1.0f && x < 1.0f && y > -1.0f && y < 1.0f ) )
    //     {
    //         return;
    //     }

    //     const mat4 PV    = s_glob.camera->CalcProj( s_glob.vp.width / s_glob.vp.height ) * s_glob.camera->CalcView();
    //     const mat4 invPV = glm::inverse( PV );

    //     Ray ray{ s_glob.camera->eye_, glm::normalize( vec3( invPV * vec4( x, y, 1.0f, 1.0f ) ) ) };

    //     for ( uint32_t idx = 0; idx < scene->mesheCache.size(); ++idx )
    //     {
    //         const MeshComponent& mesh = scene->mesheCache[idx];
    //         if ( RayIntersectsMesh( ray, mesh ) )
    //         {
    //             scene->selected = &mesh;
    //         }
    //     }
    // }
}

static void EditorDockSpace()
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
    // const auto* node = ImGui::DockBuilderGetCentralNode( dockspace_id );
    // core_assert( node );

    // // viewport
    // s_glob.vp.topLeftX    = node->Pos.x - viewport->WorkPos.x;
    // s_glob.vp.topLeftY    = node->Pos.y - viewport->WorkPos.y;
    // s_glob.vp.bottomLeftY = viewport->WorkSize.y - node->Size.y - ( node->Pos.y - viewport->WorkPos.y );
    // s_glob.vp.width       = node->Size.x;
    // s_glob.vp.height      = node->Size.y;
    // position
    // s_glob.pos  = node->Pos;
    // s_glob.size = node->Size;

    if ( ImGui::BeginMenuBar() )
    {
        if ( ImGui::BeginMenu( "File" ) )
        {
            ImGui::MenuItem( "(demo menu)", NULL, false, false );
            if ( ImGui::MenuItem( "New" ) )
            {
            }
            if ( ImGui::MenuItem( "Open", "Ctrl+O" ) )
            {
            }
            if ( ImGui::BeginMenu( "Open Recent" ) )
            {
                ImGui::MenuItem( "fish_hat.c" );
                ImGui::MenuItem( "fish_hat.inl" );
                ImGui::MenuItem( "fish_hat.h" );
                ImGui::EndMenu();
            }
            if ( ImGui::MenuItem( "Save", "Ctrl+S" ) )
            {
            }
            if ( ImGui::MenuItem( "Save As.." ) )
            {
            }
            ImGui::EndMenu();
        }
        ImGui::Separator();
        if ( ImGui::BeginMenu( "Edit" ) )
        {
            if ( ImGui::MenuItem( "Undo", "CTRL+Z" ) )
            {
            }
            if ( ImGui::MenuItem( "Redo", "CTRL+Y", false, false ) )
            {
            }  // Disabled item
            ImGui::Separator();
            if ( ImGui::MenuItem( "Cut", "CTRL+X" ) )
            {
            }
            if ( ImGui::MenuItem( "Copy", "CTRL+C" ) )
            {
            }
            if ( ImGui::MenuItem( "Paste", "CTRL+V" ) )
            {
            }
            ImGui::EndMenu();
        }
        ImGui::Separator();
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

static void EditorDebugWindow()
{
    ImGui::Begin( "Debug" );
    ImGui::Text( "%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate );
    ImGui::Text( "This is some useful text." );  // Display some text (you can use a format strings too)

    // int w, h;
    // Com_GetFrameSize( &w, &h );
    // ImGui::Separator();
    // ImGui::Text( "Framebuffer size: %d x %d", w, h );
    // w = int( s_glob.size.x );
    // h = int( s_glob.size.y );
    // ImGui::Text( "Viewport size: %d x %d", w, h );
    ImGui::Separator();
    if ( ImGui::IsMousePosValid() )
    {
        const ImGuiIO& io = ImGui::GetIO();
        // const float x     = io.MousePos.x - s_glob.pos.x;
        // const float y     = io.MousePos.y - s_glob.pos.y;
        const float x = io.MousePos.x;
        const float y = io.MousePos.y;
        ImGui::Text( "Mouse Position: (%.3f,%.3f)", x, y );
    }
    else
    {
        ImGui::Text( "Mouse Position: <invalid>" );
    }

    ImGui::End();
}

static void EditorLightWindow()
{
    ImGui::Begin( "Selected Light" );
    // Scene* scene = Com_GetScene();
    // if ( scene )
    // {
    //     ImGui::DragFloat3( "position", &( scene->light.position.x ), 1.f, -100.f, 100.f );
    //     ImGui::Separator();
    //     ImGui::ColorEdit3( "color", &( scene->light.color.x ) );
    // }
    ImGui::End();
}

static void EditorCameraWindow()
{
    ImGui::Begin( "Camera" );
    // Camera& camera = *s_glob.camera;
    // ImGui::DragFloat( "move speed", &camera.moveSpeed_, 0.1f, 0.1f, 10.f );
    // ImGui::DragFloat( "rotate speed", &camera.rotateSpeed_, 1.f, 1.f, 10.f );
    // ImGui::DragFloat( "zFar", &camera.zFar_, 10.f, 100.f, 1000.f );
    ImGui::End();
}

static void EditorMeshWindow()
{
    ImGui::Begin( "Selected Mesh" );
    // const Scene* scene                = Com_GetScene();
    // const MeshComponent* selected     = scene ? scene->selected : nullptr;
    // const MaterialComponent* material = selected ? selected->material : nullptr;
    // const char* meshName              = selected ? selected->name.c_str() : "none";
    // const char* matName               = material ? material->name.c_str() : "none";
    // int faces                         = selected ? int( selected->faces.size() ) : 0;

    // ImGui::Text( "Mesh" );
    // ImGui::Text( "name: %s", meshName );
    // ImGui::Text( "triangles: %d", faces );
    // ImGui::Separator();
    // ImGui::Text( "Material" );
    // ImGui::Text( "name: %s", matName );
    // constexpr float imageSize = 128.f;
    // if ( material && ( material->flag & MaterialComponent::HasAlbedoMap ) )
    // {
    //     ImGui::Image( (void*)material->albedoMapId, ImVec2( imageSize, imageSize ) );
    // }
    // if ( material && ( material->flag & MaterialComponent::HasNormalMap ) )
    // {
    //     ImGui::SameLine();
    //     ImGui::Image( (void*)material->normalMapId, ImVec2( imageSize, imageSize ) );
    // }
    // if ( material && ( material->flag & MaterialComponent::HasMetallicRoughnessMap ) )
    // {
    //     ImGui::SameLine();
    //     ImGui::Image( (void*)material->metallicRoughnessMapId, ImVec2( imageSize, imageSize ) );
    // }
    ImGui::End();
}

static void EditorSceneWindow()
{
    ImGui::Begin( "Scene Hierarchy" );
    // Scene* scene                = Com_GetScene();
    // const uint32_t numMeshes    = scene ? uint32_t( scene->mesheCache.size() ) : 0;
    // const uint32_t numMaterials = scene ? uint32_t( scene->materialCache.size() ) : 0;
    // ImGui::Text( "scene has %u meshes, %u materials", numMeshes, numMaterials );
    // ImGui::Separator();
    // if ( ImGui::TreeNode( "root" ) )
    // {
    //     ImGui::TreePop();
    //     for ( uint32_t idx = 0; idx < numMeshes; ++idx )
    //     {
    //         const MeshComponent& mesh = scene->mesheCache.at( idx );
    //         if ( ImGui::TreeNode( (void*)(intptr_t)idx, "mesh %s", mesh.name.c_str() ) )
    //         {
    //             scene->selected = &mesh;
    //             ImGui::TreePop();
    //         }
    //     }
    // }
    ImGui::End();
}

void EditorSetup()
{
    static bool show_demo_window = true;
    EditorDockSpace();
    EditorDebugWindow();
    EditorSelectObject();
    // EditorCameraWindow();
    // EditorMeshWindow();
    // EditorSceneWindow();
    // EditorLightWindow();
    // EditorConsole();
    // EditorDebugWindow();
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

//-----------------------------------------------------------------------------
// [SECTION] Example App: Debug Console / ShowExampleAppConsole()
//-----------------------------------------------------------------------------

// Demonstrate creating a simple console window, with scrolling, filtering, completion and history.
// For the console example, we are using a more C++ like approach of declaring a class to hold both data and functions.
struct ExampleAppConsole {
    char InputBuf[256];
    ImVector<char*> Items;
    ImVector<const char*> Commands;
    ImVector<char*> History;
    int HistoryPos;  // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter Filter;
    bool AutoScroll;
    bool ScrollToBottom;

    ExampleAppConsole()
    {
        ClearLog();
        memset( InputBuf, 0, sizeof( InputBuf ) );
        HistoryPos = -1;

        // "CLASSIFY" is here to provide the test case where "C"+[tab] completes to "CL" and display multiple matches.
        Commands.push_back( "HELP" );
        Commands.push_back( "HISTORY" );
        Commands.push_back( "CLEAR" );
        Commands.push_back( "CLASSIFY" );
        AutoScroll     = true;
        ScrollToBottom = false;
        AddLog( "Welcome to Dear ImGui!" );
    }
    ~ExampleAppConsole()
    {
        ClearLog();
        for ( int i = 0; i < History.Size; i++ )
            free( History[i] );
    }

    // Portable helpers
    static int Stricmp( const char* s1, const char* s2 )
    {
        int d;
        while ( ( d = toupper( *s2 ) - toupper( *s1 ) ) == 0 && *s1 )
        {
            s1++;
            s2++;
        }
        return d;
    }
    static int Strnicmp( const char* s1, const char* s2, int n )
    {
        int d = 0;
        while ( n > 0 && ( d = toupper( *s2 ) - toupper( *s1 ) ) == 0 && *s1 )
        {
            s1++;
            s2++;
            n--;
        }
        return d;
    }
    static char* Strdup( const char* s )
    {
        core_assert( s );
        size_t len = strlen( s ) + 1;
        void* buf  = malloc( len );
        core_assert( buf );
        return (char*)memcpy( buf, (const void*)s, len );
    }
    static void Strtrim( char* s )
    {
        char* str_end = s + strlen( s );
        while ( str_end > s && str_end[-1] == ' ' )
            str_end--;
        *str_end = 0;
    }

    void ClearLog()
    {
        for ( int i = 0; i < Items.Size; i++ )
            free( Items[i] );
        Items.clear();
    }

    void AddLog( const char* fmt, ... ) IM_FMTARGS( 2 )
    {
        // FIXME-OPT
        char buf[1024];
        va_list args;
        va_start( args, fmt );
        vsnprintf( buf, IM_ARRAYSIZE( buf ), fmt, args );
        buf[IM_ARRAYSIZE( buf ) - 1] = 0;
        va_end( args );
        Items.push_back( Strdup( buf ) );
    }

    void Draw( const char* title, bool* p_open )
    {
        ImGui::SetNextWindowSize( ImVec2( 520, 600 ), ImGuiCond_FirstUseEver );
        if ( !ImGui::Begin( title, p_open ) )
        {
            ImGui::End();
            return;
        }

        // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
        // So e.g. IsItemHovered() will return true when hovering the title bar.
        // Here we create a context menu only available from the title bar.
        if ( ImGui::BeginPopupContextItem() )
        {
            if ( ImGui::MenuItem( "Close Console" ) )
                *p_open = false;
            ImGui::EndPopup();
        }

        ImGui::TextWrapped(
            "This example implements a console with basic coloring, completion (TAB key) and history (Up/Down keys). A more elaborate "
            "implementation may want to store entries along with extra data such as timestamp, emitter, etc." );
        ImGui::TextWrapped( "Enter 'HELP' for help." );

        // TODO: display items starting from the bottom

        if ( ImGui::SmallButton( "Add Debug Text" ) )
        {
            AddLog( "%d some text", Items.Size );
            AddLog( "some more text" );
            AddLog( "display very important message here!" );
        }
        ImGui::SameLine();
        if ( ImGui::SmallButton( "Add Debug Error" ) )
        {
            AddLog( "[error] something went wrong" );
        }
        ImGui::SameLine();
        if ( ImGui::SmallButton( "Clear" ) )
        {
            ClearLog();
        }
        ImGui::SameLine();
        bool copy_to_clipboard = ImGui::SmallButton( "Copy" );
        //static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }

        ImGui::Separator();

        // Options menu
        if ( ImGui::BeginPopup( "Options" ) )
        {
            ImGui::Checkbox( "Auto-scroll", &AutoScroll );
            ImGui::EndPopup();
        }

        // Options, Filter
        if ( ImGui::Button( "Options" ) )
            ImGui::OpenPopup( "Options" );
        ImGui::SameLine();
        Filter.Draw( "Filter (\"incl,-excl\") (\"error\")", 180 );
        ImGui::Separator();

        // Reserve enough left-over height for 1 separator + 1 input text
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild( "ScrollingRegion", ImVec2( 0, -footer_height_to_reserve ), false, ImGuiWindowFlags_HorizontalScrollbar );
        if ( ImGui::BeginPopupContextWindow() )
        {
            if ( ImGui::Selectable( "Clear" ) )
                ClearLog();
            ImGui::EndPopup();
        }

        ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 4, 1 ) );  // Tighten spacing
        if ( copy_to_clipboard )
            ImGui::LogToClipboard();
        for ( int i = 0; i < Items.Size; i++ )
        {
            const char* item = Items[i];
            if ( !Filter.PassFilter( item ) )
                continue;

            // Normally you would store more information in your item than just a string.
            // (e.g. make Items[] an array of structure, store color/type etc.)
            ImVec4 color;
            bool has_color = false;
            if ( strstr( item, "[error]" ) )
            {
                color     = ImVec4( 1.0f, 0.4f, 0.4f, 1.0f );
                has_color = true;
            }
            else if ( strncmp( item, "# ", 2 ) == 0 )
            {
                color     = ImVec4( 1.0f, 0.8f, 0.6f, 1.0f );
                has_color = true;
            }
            if ( has_color )
                ImGui::PushStyleColor( ImGuiCol_Text, color );
            ImGui::TextUnformatted( item );
            if ( has_color )
                ImGui::PopStyleColor();
        }
        if ( copy_to_clipboard )
            ImGui::LogFinish();

        if ( ScrollToBottom || ( AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() ) )
            ImGui::SetScrollHereY( 1.0f );
        ScrollToBottom = false;

        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::Separator();

        // Command-line
        bool reclaim_focus                   = false;
        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
        if ( ImGui::InputText( "Input", InputBuf, IM_ARRAYSIZE( InputBuf ), input_text_flags, &TextEditCallbackStub, (void*)this ) )
        {
            char* s = InputBuf;
            Strtrim( s );
            if ( s[0] )
                ExecCommand( s );
            strcpy( s, "" );
            reclaim_focus = true;
        }

        // Auto-focus on window apparition
        ImGui::SetItemDefaultFocus();
        if ( reclaim_focus )
            ImGui::SetKeyboardFocusHere( -1 );  // Auto focus previous widget

        ImGui::End();
    }

    void ExecCommand( const char* command_line )
    {
        AddLog( "# %s\n", command_line );

        // Insert into history. First find match and delete it so it can be pushed to the back.
        // This isn't trying to be smart or optimal.
        HistoryPos = -1;
        for ( int i = History.Size - 1; i >= 0; i-- )
            if ( Stricmp( History[i], command_line ) == 0 )
            {
                free( History[i] );
                History.erase( History.begin() + i );
                break;
            }
        History.push_back( Strdup( command_line ) );

        // Process command
        if ( Stricmp( command_line, "CLEAR" ) == 0 )
        {
            ClearLog();
        }
        else if ( Stricmp( command_line, "HELP" ) == 0 )
        {
            AddLog( "Commands:" );
            for ( int i = 0; i < Commands.Size; i++ )
                AddLog( "- %s", Commands[i] );
        }
        else if ( Stricmp( command_line, "HISTORY" ) == 0 )
        {
            int first = History.Size - 10;
            for ( int i = first > 0 ? first : 0; i < History.Size; i++ )
                AddLog( "%3d: %s\n", i, History[i] );
        }
        else
        {
            AddLog( "Unknown command: '%s'\n", command_line );
        }

        // On command input, we scroll to bottom even if AutoScroll==false
        ScrollToBottom = true;
    }

    // In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
    static int TextEditCallbackStub( ImGuiInputTextCallbackData* data )
    {
        ExampleAppConsole* console = (ExampleAppConsole*)data->UserData;
        return console->TextEditCallback( data );
    }

    int TextEditCallback( ImGuiInputTextCallbackData* data )
    {
        //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
        switch ( data->EventFlag )
        {
            case ImGuiInputTextFlags_CallbackCompletion: {
                // Example of TEXT COMPLETION

                // Locate beginning of current word
                const char* word_end   = data->Buf + data->CursorPos;
                const char* word_start = word_end;
                while ( word_start > data->Buf )
                {
                    const char c = word_start[-1];
                    if ( c == ' ' || c == '\t' || c == ',' || c == ';' )
                        break;
                    word_start--;
                }

                // Build a list of candidates
                ImVector<const char*> candidates;
                for ( int i = 0; i < Commands.Size; i++ )
                    if ( Strnicmp( Commands[i], word_start, (int)( word_end - word_start ) ) == 0 )
                        candidates.push_back( Commands[i] );

                if ( candidates.Size == 0 )
                {
                    // No match
                    AddLog( "No match for \"%.*s\"!\n", (int)( word_end - word_start ), word_start );
                }
                else if ( candidates.Size == 1 )
                {
                    // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
                    data->DeleteChars( (int)( word_start - data->Buf ), (int)( word_end - word_start ) );
                    data->InsertChars( data->CursorPos, candidates[0] );
                    data->InsertChars( data->CursorPos, " " );
                }
                else
                {
                    // Multiple matches. Complete as much as we can..
                    // So inputing "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
                    int match_len = (int)( word_end - word_start );
                    for ( ;; )
                    {
                        int c                       = 0;
                        bool all_candidates_matches = true;
                        for ( int i = 0; i < candidates.Size && all_candidates_matches; i++ )
                            if ( i == 0 )
                                c = toupper( candidates[i][match_len] );
                            else if ( c == 0 || c != toupper( candidates[i][match_len] ) )
                                all_candidates_matches = false;
                        if ( !all_candidates_matches )
                            break;
                        match_len++;
                    }

                    if ( match_len > 0 )
                    {
                        data->DeleteChars( (int)( word_start - data->Buf ), (int)( word_end - word_start ) );
                        data->InsertChars( data->CursorPos, candidates[0], candidates[0] + match_len );
                    }

                    // List matches
                    AddLog( "Possible matches:\n" );
                    for ( int i = 0; i < candidates.Size; i++ )
                        AddLog( "- %s\n", candidates[i] );
                }

                break;
            }
            case ImGuiInputTextFlags_CallbackHistory: {
                // Example of HISTORY
                const int prev_history_pos = HistoryPos;
                if ( data->EventKey == ImGuiKey_UpArrow )
                {
                    if ( HistoryPos == -1 )
                        HistoryPos = History.Size - 1;
                    else if ( HistoryPos > 0 )
                        HistoryPos--;
                }
                else if ( data->EventKey == ImGuiKey_DownArrow )
                {
                    if ( HistoryPos != -1 )
                        if ( ++HistoryPos >= History.Size )
                            HistoryPos = -1;
                }

                // A better implementation would preserve the data on the current input line along with cursor position.
                if ( prev_history_pos != HistoryPos )
                {
                    const char* history_str = ( HistoryPos >= 0 ) ? History[HistoryPos] : "";
                    data->DeleteChars( 0, data->BufTextLen );
                    data->InsertChars( 0, history_str );
                }
            }
        }
        return 0;
    }
};

static void EditorConsole()
{
    static ExampleAppConsole console;
    bool open = true;
    console.Draw( "Console", &open );
}

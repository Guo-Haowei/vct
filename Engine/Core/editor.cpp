#include "editor.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Core/WindowManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "Base/Asserts.h"
#include "Base/Logger.h"

#include "Graphics/r_cbuffers.h"
#include "Manager/SceneManager.hpp"

// @TODO: refactor
class Editor {
    ImVec2 pos;
    ImVec2 size;

    Editor() = default;

public:
    static Editor& Singleton()
    {
        static Editor editor;
        return editor;
    }

    void Update();
};

void Editor::Update()
{
#if 0
    ImGuiIO& io = ImGui::GetIO();
    Scene& scene = Com_GetScene();

    // select object
    if ( !io.WantCaptureMouse && g_wndMgr->IsMouseInScreen() ) {
        const vec2 mousePos = g_wndMgr->MousePos();
        const ivec2 extent = g_wndMgr->FrameSize();
        if ( ImGui::IsMouseClicked( GLFW_MOUSE_BUTTON_1 ) ) {
            const Camera& camera = scene.camera;

            const mat4& PV = camera.ProjView();
            const mat4 invPV = glm::inverse( PV );
            vec2 cursor( mousePos.x / extent.x, 1.0f - mousePos.y / extent.y );
            cursor -= 0.5f;
            cursor *= 2.0f;

            Ray ray{ camera.position, glm::normalize( vec3( invPV * vec4( cursor.x, cursor.y, 1.0f, 1.0f ) ) ) };

            for ( const auto& node : scene.geometryNodes ) {
                for ( const auto& geom : node.geometries ) {
                    const AABB& box = geom.boundingBox;
                    const auto& mesh = geom.mesh;
                    if ( ray.Intersects( box ) ) {
                        for ( uint32_t idx = 0; idx < mesh->indices.size(); ) {
                            const vec3& a = mesh->positions[mesh->indices[idx++]];
                            const vec3& b = mesh->positions[mesh->indices[idx++]];
                            const vec3& c = mesh->positions[mesh->indices[idx++]];
                            if ( ray.Intersects( a, b, c ) ) {
                                scene.selected = &geom;
                            }
                        }
                    }
                }
            }
        }
        else if ( ImGui::IsMouseClicked( GLFW_MOUSE_BUTTON_2 ) ) {
            scene.selected = nullptr;
        }
    }
#endif
}

void EditorSetup()
{
    Editor::Singleton().Update();
}

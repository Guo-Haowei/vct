#include "r_sun_shadow.h"

#include "Core/CommonDvars.h"
#include "Framework/SceneManager.h"
#include "Framework/ProgramManager.h"
#include "Framework/WindowManager.h"
#include "gl_utils.h"
#include "r_cbuffers.h"
#include "r_rendertarget.h"
#include "Core/Check.h"
#include "Core/DynamicVariable.h"
#include "Core/Log.h"
#include "Math/Frustum.h"

static mat4 R_HackLightSpaceMatrix(const vec3& lightDir)
{
    const Scene& scene = Com_GetScene();
    const vec3 center = scene.boundingBox.Center();
    const vec3 extents = scene.boundingBox.Size();
    const float size = 0.5f * glm::max(extents.x, glm::max(extents.y, extents.z));
    const mat4 V = glm::lookAt(center + glm::normalize(lightDir) * size, center, vec3(0, 1, 0));
    const mat4 P = glm::ortho(-size, size, -size, size, 0.0f, 2.0f * size);
    return P * V;
}

void R_LightSpaceMatrix(const Camera& camera, const vec3& lightDir, mat4 lightPVs[NUM_CASCADES])
{
    // if ( !DVAR_GET_BOOL( r_enableCSM ) )
    {
        lightPVs[0] = lightPVs[1] = lightPVs[2] = R_HackLightSpaceMatrix(lightDir);
        return;
    }

    const vec4 cascades = DVAR_GET_VEC4(cam_cascades);
    const mat4 vInv = glm::inverse(camera.View());  // inversed V
    const float tanHalfHFOV = glm::tan(0.5f * camera.fovy);
    const float tanHalfVFOV = glm::tan(0.5f * camera.fovy * camera.GetAspect());

    for (int idx = 0; idx < NUM_CASCADES; ++idx)
    {
        constexpr float offset = 0.1f;
        const float xn = cascades[0] * tanHalfHFOV;
        const float xf = cascades[idx + 1] * tanHalfHFOV;
        const float yn = cascades[0] * tanHalfVFOV;
        const float yf = cascades[idx + 1] * tanHalfVFOV;
        const float zNear = cascades[0];
        const float zFar = cascades[idx + 1];

        const vec3 closest = vInv * vec4(-xf, -yf, zNear, 1.0f);
        const vec3 farest = vInv * vec4(+xf, +yf, zFar, 1.0f);
        const vec3 center = 0.5f * (farest + closest);
        const float size = (0.5f + offset) * glm::distance(closest, farest);
        const mat4 V = glm::lookAt(center + glm::normalize(lightDir) * size, center, vec3(0, 1, 0));
        const mat4 P = glm::ortho(-size, size, -size, size, 0.0f, 2.0f * size);
        lightPVs[idx] = P * V;
    }
}

void R_ShadowPass()
{
    const Scene& scene = Com_GetScene();
    g_shadowRT.Bind();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glClear(GL_DEPTH_BUFFER_BIT);
    const auto& program = gProgramManager->GetShaderProgram(ProgramType::SHADOW);
    program.Bind();

    const int res = DVAR_GET_INT(r_shadowRes);
    // render scene 3 times
    for (int idx = 0; idx < NUM_CASCADES; ++idx)
    {
        glViewport(idx * res, 0, res, res);
        const mat4& PV = g_perFrameCache.cache.LightPVs[idx];
        const Frustum frustum(PV);
        for (const GeometryNode& node : scene.geometryNodes)
        {
            g_perBatchCache.cache.PVM = PV * node.transform;
            g_perBatchCache.cache.Model = node.transform;
            g_perBatchCache.Update();
            for (const Geometry& geom : node.geometries)
            {
                if (!geom.visible)
                {
                    continue;
                }
                if (!frustum.Intersects(geom.boundingBox))
                {
                    continue;
                }

                const MeshData* drawData = reinterpret_cast<const MeshData*>(geom.mesh->gpuResource);
                glBindVertexArray(drawData->vao);
                glDrawElements(GL_TRIANGLES, drawData->count, GL_UNSIGNED_INT, 0);
            }
        }
    }

    g_shadowRT.Unbind();
    glCullFace(GL_BACK);
}
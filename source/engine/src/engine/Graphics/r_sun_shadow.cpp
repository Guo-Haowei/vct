#include "r_sun_shadow.h"

#include "Framework/ProgramManager.h"
#include "Framework/SceneManager.h"
#include "Framework/WindowManager.h"
#include "core/dynamic_variable/common_dvars.h"
#include "gl_utils.h"
#include "r_cbuffers.h"
#include "r_rendertarget.h"
//////////////////////////
#include "core/math/frustum.h"

static mat4 R_HackLightSpaceMatrix(const vec3& lightDir) {
    const Scene& scene = Com_GetScene();
    const vec3 center = scene.bound.center();
    const vec3 extents = scene.bound.size();
    const float size = 0.5f * max_val(extents.x, max_val(extents.y, extents.z));
    const mat4 V = glm::lookAt(center + glm::normalize(lightDir) * size, center, vec3(0, 1, 0));
    const mat4 P = glm::ortho(-size, size, -size, size, 0.0f, 2.0f * size);
    return P * V;
}

void R_LightSpaceMatrix(const Camera& camera, const vec3& lightDir, mat4 lightPVs[NUM_CASCADES]) {
    unused(camera);
    lightPVs[0] = lightPVs[1] = lightPVs[2] = R_HackLightSpaceMatrix(lightDir);
    return;
}

void R_ShadowPass() {
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

    const uint32_t numObjects = (uint32_t)scene.GetCount<ObjectComponent>();
    for (int idx = 0; idx < NUM_CASCADES; ++idx) {
        glViewport(idx * res, 0, res, res);
        const mat4& PV = g_perFrameCache.cache.LightPVs[idx];
        const Frustum frustum(PV);

        for (uint32_t i = 0; i < numObjects; ++i) {
            const ObjectComponent& obj = scene.GetComponentArray<ObjectComponent>()[i];
            ecs::Entity entity = scene.GetEntity<ObjectComponent>(i);
            DEV_ASSERT(scene.Contains<TransformComponent>(entity));
            const TransformComponent& transform = *scene.GetComponent<TransformComponent>(entity);
            DEV_ASSERT(scene.Contains<MeshComponent>(obj.meshID));
            const MeshComponent& mesh = *scene.GetComponent<MeshComponent>(obj.meshID);

            const mat4& M = transform.GetWorldMatrix();
            AABB aabb = mesh.mLocalBound;
            aabb.apply_matrix(M);
            if (!frustum.intersects(aabb)) {
                continue;
            }

            g_perBatchCache.cache.PVM = PV * M;
            g_perBatchCache.cache.Model = M;
            g_perBatchCache.Update();

            const MeshData* drawData = reinterpret_cast<MeshData*>(mesh.gpuResource);
            glBindVertexArray(drawData->vao);
            glDrawElements(GL_TRIANGLES, drawData->count, GL_UNSIGNED_INT, 0);
        }
    }

    g_shadowRT.Unbind();
    glCullFace(GL_BACK);
}
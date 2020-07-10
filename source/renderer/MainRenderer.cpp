#pragma once
#include "MainRenderer.h"
#include "scene/Scene.h"
#include "application/Globals.h"
#include "base/Assertion.h"
#include <unordered_map>
#include <iostream>

namespace vct {

static std::unordered_map<const Mesh*, MeshData> g_meshLUT;
static std::unordered_map<const Material*, MaterialData> g_matLUT;

void MainRenderer::createGpuResources()
{
    Vector3 center = g_scene.boundingBox.getCenter();
    Vector3 size = g_scene.boundingBox.getSize();
    float sizeMax = std::max(size.x, std::max(size.y, size.z));
    Vector4 world(center, sizeMax);

    // create uniform buffer
    m_cameraBuffer.createAndBind(UNIFORM_BUFFER_CAMERA_SLOT);

    m_lightBuffer.createAndBind(UNIFORM_BUFFER_LIGHT_SLOT);
    m_lightBuffer.cache.position = g_scene.light.position;
    m_lightBuffer.cache.lightSpacePV = lightSpaceMatrix(g_scene.light.position, g_scene.shadowBox);
    m_lightBuffer.update();

    m_materialBuffer.createAndBind(UNIFORM_BUFFER_MATERIAL_SLOT);

    // create shader
    m_gbufferProgram.createFromFiles(
        DATA_DIR "shaders/gbuffer.vert",
        DATA_DIR "shaders/gbuffer.frag");
    {
        m_gbufferProgram.use();
        m_gbufferProgram.setUniform(m_gbufferProgram.getUniformLocation("u_albedo_map"), ALBEDO_MAP_SLOT);
        m_gbufferProgram.setUniform(m_gbufferProgram.getUniformLocation("u_normal_map"), NORMAL_MAP_SLOT);
        m_gbufferProgram.setUniform(m_gbufferProgram.getUniformLocation("u_metallic_roughness_map"), METALLIC_ROUGHNESS_SLOT);
        m_gbufferProgram.stop();
    }

    m_boxWireframeProgram.createFromFiles(
        DATA_DIR "shaders/debug/box_wireframe.vert",
        DATA_DIR "shaders/debug/box_wireframe.frag");

    m_basicProgram.createFromFiles(
        DATA_DIR "shaders/basic.vert",
        DATA_DIR "shaders/basic.frag");
    {
        m_basicProgram.use();
        // TODO: set in shader
        m_basicProgram.setUniform(m_basicProgram.getUniformLocation("u_albedo_map"), ALBEDO_MAP_SLOT);
        m_basicProgram.setUniform(m_basicProgram.getUniformLocation("u_shadow_map"), SHADOW_MAP_SLOT);
        m_basicProgram.stop();
    }

    m_vctProgram.createFromFiles(
        DATA_DIR "shaders/vct.vert",
        DATA_DIR "shaders/vct.frag");
    {
        m_vctProgram.use();
        m_vctProgram.setUniform(m_vctProgram.getUniformLocation("u_albedo_map"), ALBEDO_MAP_SLOT);
        m_vctProgram.setUniform(m_vctProgram.getUniformLocation("u_shadow_map"), SHADOW_MAP_SLOT);
        m_vctProgram.stop();
    }

    m_depthProgram.createFromFiles(
        DATA_DIR "shaders/depth.vert",
        DATA_DIR "shaders/depth.frag");

    m_debugTextureProgram.createFromFiles(
        DATA_DIR "shaders/debug/texture.vert",
        DATA_DIR "shaders/debug/texture.frag");

    m_voxelProgram.createFromFiles(
        DATA_DIR "shaders/voxel/voxelization.vert",
        DATA_DIR "shaders/voxel/voxelization.frag",
        DATA_DIR "shaders/voxel/voxelization.geom");
    {
        // set one time uniforms
        m_voxelProgram.use();
        m_voxelProgram.setUniform(m_voxelProgram.getUniformLocation("u_world"), world);
        m_voxelProgram.setUniform(m_voxelProgram.getUniformLocation("u_voxel_texture_size"), (int)VOXEL_TEXTURE_SIZE);
        m_voxelProgram.setUniform(m_voxelProgram.getUniformLocation("u_albedo_map"), ALBEDO_MAP_SLOT);
        m_voxelProgram.setUniform(m_voxelProgram.getUniformLocation("u_shadow_map"), SHADOW_MAP_SLOT);
        m_voxelProgram.stop();
    }

    m_visualizeProgram.createFromFiles(
        DATA_DIR "shaders/voxel/visualization.vert",
        DATA_DIR "shaders/voxel/visualization.frag");
    {
        // set one time uniforms
        m_visualizeProgram.use();
        m_visualizeProgram.setUniform(m_visualizeProgram.getUniformLocation("u_world"), world);
        m_voxelProgram.stop();
    }

    m_voxelPostProgram.createFromFile(DATA_DIR "shaders/voxel/post.comp");

    // frame buffers
    createFrameBuffers();

    // create box wireframe
    {
        std::vector<Vector3> points;
        std::vector<unsigned int> indices;
        three::geometry::boxWireframe(points, indices);

        glGenVertexArrays(1, &m_boxWireframe.vao);
        glGenBuffers(2, &m_boxWireframe.ebo);
        glBindVertexArray(m_boxWireframe.vao);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_boxWireframe.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_boxWireframe.vbos[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * points.size(), points.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);
        glEnableVertexAttribArray(0);

        m_boxWireframe.count = static_cast<unsigned int>(indices.size());
    }

    // create box quad
    {
        float points[] =
        {
            -1.0f, +1.0f,
            -1.0f, -1.0f,
            +1.0f, +1.0f,
            +1.0f, +1.0f,
            -1.0f, -1.0f,
            +1.0f, -1.0f,
        };

        glGenVertexArrays(1, &m_quad.vao);
        glGenBuffers(1, m_quad.vbos);
        glBindVertexArray(m_quad.vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_quad.vbos[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
    }

    {
        std::vector<Vector3> points;
        std::vector<unsigned int> indices;
        three::geometry::box(points, indices);

        glGenVertexArrays(1, &m_box.vao);
        glGenBuffers(2, &m_box.ebo);
        glBindVertexArray(m_box.vao);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_box.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_box.vbos[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * points.size(), points.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);
        glEnableVertexAttribArray(0);

        m_box.count = static_cast<unsigned int>(indices.size());
    }

    /// create voxel image
    {
        Texture3DCreateInfo info;
        info.wrapS = info.wrapT = info.wrapR = GL_CLAMP_TO_BORDER;
        info.size = VOXEL_TEXTURE_SIZE;
        info.minFilter = GL_LINEAR_MIPMAP_LINEAR;
        info.magFilter = GL_NEAREST;
        info.mipLevel = VOXEL_TEXTURE_MIP_LEVEL;
        info.format = GL_RGBA16F;

        m_albedoVoxel.create3DEmpty(info);
        m_normalVoxel.create3DEmpty(info);
    }

    // load scene
    // create mesh
    for (const std::unique_ptr<Mesh>& mesh : g_scene.meshes)
    {
        // TODO: buffer storage
        MeshData data;
        glGenVertexArrays(1, &data.vao);
        glGenBuffers(4, &data.ebo);
        glBindVertexArray(data.vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ebo);
        glNamedBufferStorage(data.ebo, sizeof(Vector3u) * mesh->faces.size(), mesh->faces.data(), 0);

        glBindBuffer(GL_ARRAY_BUFFER, data.vbos[0]);
        glNamedBufferStorage(data.vbos[0], sizeof(Vector3) * mesh->positions.size(), mesh->positions.data(), 0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, data.vbos[1]);
        glNamedBufferStorage(data.vbos[1], sizeof(Vector3) * mesh->normals.size(), mesh->normals.data(), 0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, data.vbos[2]);
        glNamedBufferStorage(data.vbos[2], sizeof(Vector2) * mesh->uvs.size(), mesh->uvs.data(), 0);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), 0);
        glEnableVertexAttribArray(2);

        if (mesh->tangents.size() != 0)
        {
            glGenBuffers(2, &data.vbos[3]);

            glBindBuffer(GL_ARRAY_BUFFER, data.vbos[3]);
            glNamedBufferStorage(data.vbos[3], sizeof(Vector3) * mesh->tangents.size(), mesh->tangents.data(), 0);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);
            glEnableVertexAttribArray(3);

            glBindBuffer(GL_ARRAY_BUFFER, data.vbos[4]);
            glNamedBufferStorage(data.vbos[4], sizeof(Vector3) * mesh->bitangents.size(), mesh->bitangents.data(), 0);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);
            glEnableVertexAttribArray(4);
        }

        glBindVertexArray(0);
        data.count = 3 * static_cast<unsigned int>(mesh->faces.size());

        g_meshLUT.insert({ mesh.get(), data });
    }

    // create material
    for (const std::unique_ptr<Material>& mat : g_scene.materials)
    {
        MaterialData matData;
        if (!mat->albedoTexture.empty())
        {
            matData.albedoMap.create2DImageFromFile(mat->albedoTexture.c_str());
            matData.albedoColor = Vector4::Zero;
        }
        else
        {
            matData.albedoColor = Vector4(mat->albedo, 1.0f);
        }

        if (!mat->metallicRoughnessTexture.empty())
        {
            matData.materialMap.create2DImageFromFile(mat->metallicRoughnessTexture.c_str());
        }
        else
        {
            matData.metallic = mat->metallic;
            matData.roughness = mat->roughness;
        }

        if (!mat->normalTexture.empty())
        {
            matData.normalMap.create2DImageFromFile(mat->normalTexture.c_str());
        }

        g_matLUT.insert({ mat.get(), matData });
    }

    glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_SLOT);
    m_shadowBuffer.getDepthTexture().bind();

    glActiveTexture(GL_TEXTURE0 + GBUFFER_POSITION_METALLIC_SLOT);
    m_gbuffer.getColorAttachment(0).bind();

    glActiveTexture(GL_TEXTURE0 + GBUFFER_NORMAL_ROUGHNESS_SLOT);
    m_gbuffer.getColorAttachment(1).bind();

    glActiveTexture(GL_TEXTURE0 + GBUFFER_ALBEDO_SLOT);
    m_gbuffer.getColorAttachment(2).bind();
}

void MainRenderer::visualizeVoxels()
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_BLEND);

    m_visualizeProgram.use();
    glBindVertexArray(m_box.vao);

    int mipLevel = g_UIControls.voxelMipLevel;
    m_albedoVoxel.bindImageTexture(0, mipLevel);
    GpuTexture& voxelTexture = g_UIControls.drawTexture == DrawTexture::TEXTURE_VOXEL_ALBEDO ? m_albedoVoxel : m_normalVoxel;

    glBindImageTexture(0, voxelTexture.getHandle(), mipLevel, GL_TRUE, 0,
                       GL_READ_ONLY, voxelTexture.getFormat());

    int size = VOXEL_TEXTURE_SIZE >> mipLevel;
    glDrawElementsInstanced(GL_TRIANGLES, m_box.count, GL_UNSIGNED_INT, 0, size * size * size);
}

void MainRenderer::gbufferPass()
{
    GlslProgram& program = m_gbufferProgram;

    m_gbuffer.bind();
    program.use();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static GLint location_M = program.getUniformLocation("M");
    // TODO: refactor draw scene
    for (const GeometryNode& node : g_scene.geometryNodes)
    {
        program.setUniform(location_M, node.transform);

        for (const Geometry& geom : node.geometries)
        {
            if (!g_scene.camera.frustum.intersectsBox(geom.boundingBox))
            {
                ++g_UIControls.objectOccluded;
                continue;
            }

            const auto& meshPair = g_meshLUT.find(geom.pMesh);
            ASSERT(meshPair != g_meshLUT.end());
            const MeshData& drawData = meshPair->second;

            const auto& matPair = g_matLUT.find(geom.pMaterial);
            ASSERT(matPair != g_matLUT.end());
            const MaterialData& matData = matPair->second;

            m_materialBuffer.cache = matData;
            m_materialBuffer.update();

            glBindVertexArray(drawData.vao);
            glActiveTexture(GL_TEXTURE0 + ALBEDO_MAP_SLOT);
            matData.albedoMap.bind();
            glActiveTexture(GL_TEXTURE0 + METALLIC_ROUGHNESS_SLOT);
            matData.materialMap.bind();
            glActiveTexture(GL_TEXTURE0 + NORMAL_MAP_SLOT);
            matData.normalMap.bind();

            glDrawElements(GL_TRIANGLES, drawData.count, GL_UNSIGNED_INT, 0);
        }
    }

    m_gbufferProgram.stop();
    m_gbuffer.unbind();
}

void MainRenderer::shadowPass()
{
    m_shadowBuffer.bind();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glViewport(0, 0, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);
    glClear(GL_DEPTH_BUFFER_BIT);
    // render scene
    m_depthProgram.use();

    static GLint PVMLocation = m_depthProgram.getUniformLocation("u_PVM");

    for (const GeometryNode& node : g_scene.geometryNodes)
    {
        Matrix4 PVM = m_lightBuffer.cache.lightSpacePV * node.transform;
        m_depthProgram.setUniform(PVMLocation, PVM);
        for (const Geometry& geom : node.geometries)
        {
            const auto& meshPair = g_meshLUT.find(geom.pMesh);
            ASSERT(meshPair != g_meshLUT.end());
            const MeshData& drawData = meshPair->second;

            glBindVertexArray(drawData.vao);
            glDrawElements(GL_TRIANGLES, drawData.count, GL_UNSIGNED_INT, 0);
        }
    }

    m_shadowBuffer.unbind();

    glCullFace(GL_BACK);
}

void MainRenderer::renderToVoxelTexture()
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glViewport(0, 0, VOXEL_TEXTURE_SIZE, VOXEL_TEXTURE_SIZE);

    m_albedoVoxel.bindImageTexture(ALBEDO_VOXEL_SLOT);
    m_normalVoxel.bindImageTexture(NORMAL_VOXEL_SLOT);
    m_voxelProgram.use();
    static GLint mLocation = m_voxelProgram.getUniformLocation("u_M");

    for (const GeometryNode& node : g_scene.geometryNodes)
    {
        m_voxelProgram.setUniform(mLocation, node.transform);
        for (const Geometry& geom : node.geometries)
        {
            const auto& meshPair = g_meshLUT.find(geom.pMesh);
            ASSERT(meshPair != g_meshLUT.end());
            const MeshData& drawData = meshPair->second;

            const auto& matPair = g_matLUT.find(geom.pMaterial);
            ASSERT(matPair != g_matLUT.end());
            const MaterialData& matData = matPair->second;

            m_materialBuffer.cache = matData;
            m_materialBuffer.update();

            glBindVertexArray(drawData.vao);
            glActiveTexture(GL_TEXTURE0 + ALBEDO_MAP_SLOT);
            matData.albedoMap.bind();

            glDrawElements(GL_TRIANGLES, drawData.count, GL_UNSIGNED_INT, 0);
        }
    }

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // post process
    m_voxelPostProgram.use();

    constexpr GLuint workGroupX = 512;
    constexpr GLuint workGroupY = 512;
    constexpr GLuint workGroupZ =
        (VOXEL_TEXTURE_SIZE * VOXEL_TEXTURE_SIZE * VOXEL_TEXTURE_SIZE) /
        (workGroupX * workGroupY);

    glDispatchCompute(workGroupX, workGroupY, workGroupZ);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    m_albedoVoxel.bind();
    m_albedoVoxel.genMipMap();
    m_normalVoxel.bind();
    m_normalVoxel.genMipMap();
}

void MainRenderer::renderSceneVCT()
{
    m_vctProgram.use();
    static const GLint mLocation = m_vctProgram.getUniformLocation("u_M");

    for (const GeometryNode& node : g_scene.geometryNodes)
    {
        m_vctProgram.setUniform(mLocation, node.transform);
        for (const Geometry& geom : node.geometries)
        {
            if (!g_scene.camera.frustum.intersectsBox(geom.boundingBox))
            {
                ++g_UIControls.objectOccluded;
                continue;
            }

            const auto& meshPair = g_meshLUT.find(geom.pMesh);
            ASSERT(meshPair != g_meshLUT.end());
            const MeshData& drawData = meshPair->second;

            const auto& matPair = g_matLUT.find(geom.pMaterial);
            ASSERT(matPair != g_matLUT.end());
            const MaterialData& matData = matPair->second;

            m_materialBuffer.cache = matData;
            m_materialBuffer.update();

            glBindVertexArray(drawData.vao);
            glActiveTexture(GL_TEXTURE0 + ALBEDO_MAP_SLOT);
            matData.albedoMap.bind();

            glDrawElements(GL_TRIANGLES, drawData.count, GL_UNSIGNED_INT, 0);
        }
    }
}

void MainRenderer::renderSceneNoGI()
{
    m_basicProgram.use();
    // TODO: clean up uniforms
    static const GLint mLocation = m_basicProgram.getUniformLocation("u_M");
    static const GLint camPosLocation = m_basicProgram.getUniformLocation("u_camera_position");

    m_basicProgram.setUniform(camPosLocation, g_scene.camera.position);

    for (const GeometryNode& node : g_scene.geometryNodes)
    {
        m_basicProgram.setUniform(mLocation, node.transform);
        for (const Geometry& geom : node.geometries)
        {
            if (!g_scene.camera.frustum.intersectsBox(geom.boundingBox))
            {
                ++g_UIControls.objectOccluded;
                continue;
            }

            const auto& meshPair = g_meshLUT.find(geom.pMesh);
            ASSERT(meshPair != g_meshLUT.end());
            const MeshData& drawData = meshPair->second;

            const auto& matPair = g_matLUT.find(geom.pMaterial);
            ASSERT(matPair != g_matLUT.end());
            const MaterialData& matData = matPair->second;

            m_materialBuffer.cache = matData;
            m_materialBuffer.update();

            glBindVertexArray(drawData.vao);
            glActiveTexture(GL_TEXTURE0 + ALBEDO_MAP_SLOT);
            matData.albedoMap.bind();

            glDrawElements(GL_TRIANGLES, drawData.count, GL_UNSIGNED_INT, 0);
        }
    }
}

void MainRenderer::renderBoundingBox()
{
    /// TODO: refactor
    m_boxWireframeProgram.use();
    static const GLint centerLocation = m_boxWireframeProgram.getUniformLocation("u_center");
    static const GLint sizeLocation = m_boxWireframeProgram.getUniformLocation("u_size");
    static const GLint colorLocation = m_boxWireframeProgram.getUniformLocation("u_color");

    glBindVertexArray(m_boxWireframe.vao);

    if (g_UIControls.showObjectBoundingBox)
    {
        m_boxWireframeProgram.setUniform(colorLocation, Vector3::UnitY);
        for (const GeometryNode& node : g_scene.geometryNodes)
        {
            for (const Geometry& geom : node.geometries)
            {
                m_boxWireframeProgram.setUniform(centerLocation, geom.boundingBox.getCenter());
                m_boxWireframeProgram.setUniform(sizeLocation, geom.boundingBox.getSize());
                glDrawElements(GL_LINES, m_boxWireframe.count, GL_UNSIGNED_INT, 0);
            }
        }
    }

    if (g_UIControls.showWorldBoundingBox)
    {
        m_boxWireframeProgram.setUniform(colorLocation, Vector3::UnitZ);
        m_boxWireframeProgram.setUniform(centerLocation, g_scene.boundingBox.getCenter());
        m_boxWireframeProgram.setUniform(sizeLocation, g_scene.boundingBox.getSize());
        glDrawElements(GL_LINES, m_boxWireframe.count, GL_UNSIGNED_INT, 0);
    }
}

void MainRenderer::renderFrameBufferTextures(const Extent2i& extent)
{
    GlslProgram& program = m_debugTextureProgram;

    program.use();

    glDisable(GL_DEPTH_TEST);

    static GLint location_texture = program.getUniformLocation("u_texture");
    static GLint location_type = program.getUniformLocation("u_type");

    constexpr float scale = 0.13f;
    int width = static_cast<int>(scale * extent.witdh);
    int height = static_cast<int>(scale * extent.height);

    int xOffset = extent.witdh - width;

    glBindVertexArray(m_quad.vao);
    glViewport(0, 0, extent.witdh, extent.height);

    int textureSlot = 0;
    int type = -1;
    // normal
    switch (g_UIControls.drawTexture)
    {
    case DrawTexture::TEXTURE_GBUFFER_ALBEDO:
        textureSlot = GBUFFER_ALBEDO_SLOT;
        type = 1;
        break;
    case DrawTexture::TEXTURE_GBUFFER_NORMAL:
        textureSlot = GBUFFER_NORMAL_ROUGHNESS_SLOT;
        type = 2;
        break;
    case DrawTexture::TEXTURE_GBUFFER_METALLIC:
        textureSlot = GBUFFER_POSITION_METALLIC_SLOT;
        type = 3;
        break;
    case DrawTexture::TEXTURE_GBUFFER_ROUGHNESS:
        textureSlot = GBUFFER_NORMAL_ROUGHNESS_SLOT;
        type = 3;
        break;
    case DrawTexture::TEXTURE_GBUFFER_SHADOW:
        textureSlot = SHADOW_MAP_SLOT;
        type = 0;
        break;
    default: break;
    }

    program.setUniform(location_texture, textureSlot);
    program.setUniform(location_type, type);
    glDrawArrays(GL_TRIANGLES, 0, 6);

#if 0
    // albedo
    program.setUniform(location_texture, GBUFFER_ALBEDO_SLOT);
    program.setUniform(location_type, 1);
    glViewport(0 * width, 0, width, height);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // position
    program.setUniform(location_texture, GBUFFER_POSITION_METALLIC_SLOT);
    glViewport(1 * width, 0, width, height);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // normal
    program.setUniform(location_texture, GBUFFER_NORMAL_ROUGHNESS_SLOT);
    program.setUniform(location_type, 2);
    glViewport(2 * width, 0, width, height);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // metallic
    program.setUniform(location_texture, GBUFFER_POSITION_METALLIC_SLOT);
    program.setUniform(location_type, 3);
    glViewport(3 * width, 0, width, height);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // roughness
    program.setUniform(location_texture, GBUFFER_NORMAL_ROUGHNESS_SLOT);
    glViewport(4 * width, 0, width, height);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // ao

    // shadow
    program.setUniform(location_texture, SHADOW_MAP_SLOT);
    program.setUniform(location_type, 0);
    glViewport(6 * width, 0, width, height);
    glDrawArrays(GL_TRIANGLES, 0, 6);
#endif

    program.stop();
}

void MainRenderer::render()
{
    if (g_scene.lightDirty)
    {
        m_lightBuffer.cache.position = g_scene.light.position;
        m_lightBuffer.cache.lightSpacePV = lightSpaceMatrix(g_scene.light.position, g_scene.shadowBox);
        m_lightBuffer.update();
        shadowPass();
    }

    if (g_scene.dirty || g_UIControls.forceUpdateVoxelTexture)
    {
        m_albedoVoxel.clear();
        m_normalVoxel.clear();
        renderToVoxelTexture();
    }

    auto extent = m_pWindow->getFrameExtent();

    if (extent.witdh * extent.height > 0)
    {
        // skip rendering if minimized
        Matrix4 PV = g_scene.camera.perspective() * g_scene.camera.view();
        // TODO: move this code to somewhere else
        g_scene.camera.frustum.setFromProjectionView(PV);

        m_cameraBuffer.cache.PV = PV;
        m_cameraBuffer.update();

        glViewport(0, 0, extent.witdh, extent.height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        gbufferPass();

        if (g_UIControls.drawTexture == DrawTexture::TEXTURE_NO_GI)
        {
            renderSceneNoGI();
        }
        else if (g_UIControls.drawTexture < DrawTexture::TEXTURE_GBUFFER_NONE)
        {
            visualizeVoxels();
        }
        else
        {
            renderFrameBufferTextures(extent);
        }

        if (g_UIControls.showObjectBoundingBox || g_UIControls.showWorldBoundingBox)
            renderBoundingBox();

    }
}

void MainRenderer::createFrameBuffers()
{
    auto extent = m_pWindow->getFrameExtent();

    m_shadowBuffer.create(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);
    // m_earlyZBuffer.create(extent.witdh, extent.height);

    m_gbuffer.create(extent.witdh, extent.height);
}

void MainRenderer::destroyGpuResources()
{
    // gpu resource
    m_voxelProgram.destroy();
    m_visualizeProgram.destroy();
    m_basicProgram.destroy();
    m_boxWireframeProgram.destroy();
    m_voxelPostProgram.destroy();
    m_depthProgram.destroy();
    m_debugTextureProgram.destroy();

    // render targets
    m_shadowBuffer.destroy();
    m_gbuffer.destroy();
}

} // namespace vct

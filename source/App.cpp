#include "App.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <stdexcept>
// temp
#include "GL/ShaderProgram.h"
#include "GL/VertexArray.h"
#include "SceneManager.h"
// temp
#ifdef _DEBUG
#   include "internal/Debug.h"
#endif
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
#define TITLE "Voxel GI"
#ifndef MODEL_DIR
#define MODEL_DIR ""
#endif

void App::run()
{
    try
    {
        /*************************  GLFW  *************************/
        glfwSetErrorCallback([](int error, const char* desc){
            throw std::runtime_error("[ERROR][Glfw] " + std::string(desc));
        });

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#ifdef _DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
        // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        m_pWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, TITLE, NULL, NULL);

        glfwSetWindowUserPointer(m_pWindow, this);
        // glfwGetFramebufferSize(m_pWindow, &m_frameBufferSizeCache.x, &m_frameBufferSizeCache.y);

        // glfwSetWindowUserPointer(m_pGlfwWindow, this);
        // glfwSetFramebufferSizeCallback(m_pGlfwWindow, FramebufferCallback);
        // glfwSetScrollCallback(m_pGlfwWindow, ScrollCallback);

        glfwMakeContextCurrent(m_pWindow);
        glfwSwapInterval(1); // Enable vsync

        /*************************  GLAD  *************************/
        if (!gladLoadGL())
        {
            throw std::runtime_error("[Error][Glad] Failed to load gl functions.");
        }

        std::cout << "Vendor:          " << glGetString(GL_VENDOR) << std::endl;
        std::cout << "Renderer:        " << glGetString(GL_RENDERER) << std::endl;
        std::cout << "Version OpenGL:  " << glGetString(GL_VERSION) << std::endl;
        std::cout << "Version GLSL:    " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

#ifdef _DEBUG
        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(internal::glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
#endif
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        ///////////////////////////////////////////////////////////////////////
        // temp code
        // shader
        ShaderProgram::CreateInfo shaderCreateInfo {};
        shaderCreateInfo.vs = "voxelization.vs.glsl";
        shaderCreateInfo.fs = "voxelization.fs.glsl";
        ShaderProgram voxelShader("voxelization", shaderCreateInfo);

        // load scene
        SceneManager sm;
        // sm.load(MODEL_DIR "cube/", "cube.json");
        sm.load(MODEL_DIR "bunny/", "bunny.json");
        // sm.write();

        auto& mesh = sm.getScene().meshes.front();

        // position buffer
        GpuBuffer::CreateInfo vertexBufferCreateInfo {};
        vertexBufferCreateInfo.type = GL_ARRAY_BUFFER;
        vertexBufferCreateInfo.usage = GL_STATIC_DRAW;
        vertexBufferCreateInfo.initialBuffer.data = mesh->positions.data(); 
        vertexBufferCreateInfo.initialBuffer.size = sizeof(vec3) * mesh->positions.size();
        GpuBuffer vertexBuffer("mesh.positions", vertexBufferCreateInfo);
        // normal buffer
        GpuBuffer::CreateInfo normalBufferCreateInfo {};
        normalBufferCreateInfo.type = GL_ARRAY_BUFFER;
        normalBufferCreateInfo.usage = GL_STATIC_DRAW;
        normalBufferCreateInfo.initialBuffer.data = mesh->normals.data();
        normalBufferCreateInfo.initialBuffer.size = sizeof(vec3) * mesh->normals.size();
        GpuBuffer normalBuffer("mesh.colors", normalBufferCreateInfo);
        // index array
        GpuBuffer::CreateInfo indexBufferCreateInfo {};
        indexBufferCreateInfo.type = GL_ELEMENT_ARRAY_BUFFER;
        indexBufferCreateInfo.usage = GL_STATIC_DRAW;
        indexBufferCreateInfo.initialBuffer.data = mesh->indices.data(); 
        indexBufferCreateInfo.initialBuffer.size = sizeof(unsigned int) * mesh->indices.size();
        GpuBuffer indexBuffer("mesh.indices", indexBufferCreateInfo);
        // vertex array
        VertexArray vertexArray("mesh.vao", { GL_TRIANGLES });
        vertexArray
            .bind()
            .appendAttribute({ GL_FLOAT, 3, sizeof(vec3), 0 }, vertexBuffer)
            .appendAttribute({ GL_FLOAT, 3, sizeof(vec3), 0 }, normalBuffer)
            .appendIndexBuffer({ GL_UNSIGNED_INT }, indexBuffer)
            .unbind();

        ////////////////////////////////////////////////////////////////////////
        // timer stuff, needs refactor
        double currentTime = glfwGetTime();
        double elapsedTime = 0;
        struct _Timer { unsigned int t: 4; _Timer(): t(0) {} } t;

        while (!glfwWindowShouldClose(m_pWindow))
        {
            glfwPollEvents();

            int width, height;
            glfwGetFramebufferSize(m_pWindow, &width, &height);
            glViewport(0, 0, width, height);

            glClearColor(0.3f, 0.4f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // uniforms
            glUseProgram(voxelShader.getHandle());
            static mat4 P = glm::perspective(0.25f * glm::pi<float>(), (float)width/height, 0.1f, 100.0f);
            static mat4 V = glm::lookAt(vec3(0, 2, 4), vec3(0), vec3(0, 1, 0));
            mat4 PV = P * V;
            voxelShader.setUniform("PV", PV);

            glBindVertexArray(vertexArray.getHandle());
            //glDrawArrays(GL_TRIANGLES, 0, mesh->positions.size());
            glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);

            // swap front and back buffers
            glfwSwapBuffers(m_pWindow);

            // timer
            double deltaTime = glfwGetTime() - currentTime;
            currentTime = glfwGetTime();
            
            float frameRate = static_cast<int>((1. / deltaTime) * 100) / 100.f;

            // TODO: refactor
            if (++t.t == 0)
            {
                std::string title = TITLE " FPS: " + std::to_string(frameRate);
                glfwSetWindowTitle(m_pWindow, title.c_str());
            }
        }

        // temp
        indexBuffer.release();
        normalBuffer.release();
        voxelShader.release();
        vertexBuffer.release();
        vertexArray.release();
        // temp

        glfwDestroyWindow(m_pWindow);
        glfwTerminate();
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << '\n';
    }
}

    // GLFWwindow* m_pWindow;
    // int m_width, m_height;

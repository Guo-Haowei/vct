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

        SceneManager sm;
        sm.load("bunny.obj");

        ///////////////////////////////////////////////////////////////////////
        // temp code
        // shader
        ShaderProgram::CreateInfo shaderCreateInfo {};
        shaderCreateInfo.vs = "voxelization.vs.glsl";
        shaderCreateInfo.fs = "voxelization.fs.glsl";
        ShaderProgram voxelShader("voxelization", shaderCreateInfo);
        // position buffer
        struct Vertex
        {
            float x, y, z;
            // float r, g, b;
        };
        Vertex positions[4] = {
            { -0.5f,  0.5f, 0.0f }, // top left
            { -0.5f, -0.5f, 0.0f }, // bottom left
            {  0.5f, -0.5f, 0.0f }, // bottom right
            {  0.5f,  0.5f, 0.0f }, // top right
        };
        GpuBuffer::CreateInfo vertexBufferCreateInfo {};
        vertexBufferCreateInfo.type = GL_ARRAY_BUFFER;
        vertexBufferCreateInfo.usage = GL_STATIC_DRAW;
        vertexBufferCreateInfo.initialBuffer.data = positions; 
        vertexBufferCreateInfo.initialBuffer.size = sizeof(positions); 
        GpuBuffer vertexBuffer("triangle.positions", vertexBufferCreateInfo);
        // color buffer
        struct Color
        {
            float r, g, b;
        };
        Vertex colors[4] = {
            { 0.0f, 0.9f, 0.0f }, // top left
            { 0.0f, 0.0f, 0.0f }, // bottom left
            { 0.9f, 0.0f, 0.0f }, // bottom right
            { 0.9f, 0.9f, 0.0f }, // top right
        };
        GpuBuffer::CreateInfo colorBufferCreateInfo {};
        colorBufferCreateInfo.type = GL_ARRAY_BUFFER;
        colorBufferCreateInfo.usage = GL_STATIC_DRAW;
        colorBufferCreateInfo.initialBuffer.data = colors;
        colorBufferCreateInfo.initialBuffer.size = sizeof(colors); 
        GpuBuffer colorBuffer("triangle.colors", colorBufferCreateInfo);
        // index array
        unsigned short indices[6] = {
            0, 1, 2,
            0, 2, 3
        };
        GpuBuffer::CreateInfo indexBufferCreateInfo {};
        indexBufferCreateInfo.type = GL_ELEMENT_ARRAY_BUFFER;
        indexBufferCreateInfo.usage = GL_STATIC_DRAW;
        indexBufferCreateInfo.initialBuffer.data = indices; 
        indexBufferCreateInfo.initialBuffer.size = sizeof(indices); 
        GpuBuffer indexBuffer("triangle.indices", indexBufferCreateInfo);
        // vertex array
        VertexArray vertexArray("triangle.vao", { GL_TRIANGLES });
        vertexArray
            .bind()
            .appendAttribute({ GL_FLOAT, 3, sizeof(Vertex), 0 }, vertexBuffer)
            .appendAttribute({ GL_FLOAT, 3, sizeof(Color), 0 }, colorBuffer)
            .appendIndexBuffer({ GL_UNSIGNED_SHORT }, indexBuffer)
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

            glUseProgram(voxelShader.getHandle());
            glBindVertexArray(vertexArray.getHandle());
            // glDrawArrays(GL_TRIANGLES, 0, 6);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

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
        colorBuffer.release();
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

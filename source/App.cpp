#include "App.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#ifdef _DEBUG
#   include "internal/Debug.h"
#endif
#include <iostream>
#include <string>
#include <stdexcept>
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

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
        m_pWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Voxel GI", NULL, NULL);

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
        
        while (!glfwWindowShouldClose(m_pWindow))
        {
            glfwPollEvents();

            glClearColor(0.3f, 0.4f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glfwSwapBuffers(m_pWindow);
        }

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

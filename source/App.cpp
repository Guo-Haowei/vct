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
#include "VoxelPass.h"
// temp
#include "InputManager.h"
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
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
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

        /*************************  Input Manager  *************************/
        // cursor
        InputManager& im = InputManager::getInstance();
        glfwGetCursorPos(m_pWindow, &im.m_cursorPos.x, &im.m_cursorPos.y);
        im.m_prevCursorPos = im.m_cursorPos;
        glfwSetCursorPosCallback(m_pWindow, [](GLFWwindow* window, double xpos, double ypos){
            InputManager::getInstance().m_cursorPos = { xpos, ypos };
        });

        // mouse button
        glfwSetMouseButtonCallback(m_pWindow, [](GLFWwindow* window, int button, int action, int mods){
            if (button > InputManager::BUTTON_COUNT)
                return;

            InputManager::getInstance().m_mouseButtons[button] = action;
        });

        // scroll
        glfwSetScrollCallback(m_pWindow, [](GLFWwindow* window, double xoffset, double yoffset){
            InputManager::getInstance().m_scroll = static_cast<float>(yoffset);
            std::cout << "scroll is " << yoffset << std::endl;
        });

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

        //// load scene
        g_pSceneManager->load(MODEL_DIR "bunny", "bunny.json");

        ////// temp
        // manually set camera position
        auto& cam = g_pSceneManager->getScene().camera;
        cam.moveFront(-3.0f);
        cam.moveUp(1.0f);

        VoxelPass voxelPass;
        voxelPass.initialize();

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
            cam.setAspect(width, height);
            cam.update();

            // glViewport(0, 0, width, height);

            // glClearColor(0.3f, 0.4f, 0.3f, 1.0f);
            // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // post update
            InputManager::getInstance().postUpdate();
            // swap front and back buffers
            voxelPass.render();
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
        voxelPass.finalize();
        // temp

        glfwDestroyWindow(m_pWindow);
        glfwTerminate();
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void App::getFrameBufferSize(int& width, int& height)
{
    glfwGetFramebufferSize(m_pWindow, &width, &height);
}

App* g_pApp = new App();

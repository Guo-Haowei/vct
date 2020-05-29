#pragma once

struct GLFWwindow;

class App
{
public:
    void run();
    void getFrameBufferSize(int& width, int& height);

private:
    GLFWwindow* m_pWindow;
    int m_width, m_height;
};

extern App* g_pApp;

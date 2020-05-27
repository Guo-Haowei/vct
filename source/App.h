#pragma once

struct GLFWwindow;

class App
{
public:
    void run();

private:
    GLFWwindow* m_pWindow;
    int m_width, m_height;
};

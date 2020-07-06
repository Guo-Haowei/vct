#pragma once
#include "math/GeoMath.h"

class InputManager
{
public:
    enum MOUSE_BUTTON
    {
        BUTTON_LEFT = 0,
        BUTTON_RIGHT,
        BUTTON_MIDDLE,
        BUTTON_COUNT
    };

    inline bool isButtonDown(int button) { return m_mouseButtons[button]; }

    static InputManager& getInstance()
    {
        static InputManager instance;
        return instance;
    }

    void postUpdate()
    {
        m_prevCursorPos = m_cursorPos;
        m_scroll = 0.0f;
    }

    inline float getScroll() { return m_scroll; }

    inline dvec2 getCursorDelta() { return m_cursorPos - m_prevCursorPos; }

private:
    InputManager() = default;

private:
    dvec2 m_cursorPos;
    dvec2 m_prevCursorPos;

    float m_scroll = 0.0f;
    bool m_mouseButtons[BUTTON_COUNT] = { false, false, false };
    // bool m_mouseButtonsPrev[BUTTON_COUNT];

    friend class App;
};

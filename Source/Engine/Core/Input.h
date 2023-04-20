#pragma once
#include <array>

#include "InputCode.h"

#include "Core/Types.h"
#include "Math/GeoMath.h"

class WindowManager;

using Point = glm::vec2;

class Input
{
    using KeyArray = std::array<bool, underlying(EKeyCode::COUNT)>;
    using ButtonArray = std::array<bool, underlying(EMouseButton::COUNT)>;

public:
    static void BeginFrame();
    static void EndFrame();

    static bool IsButtonDown(EMouseButton key);
    static bool IsButtonPressed(EMouseButton key);
    static bool IsButtonReleased(EMouseButton key);

    static bool IsKeyDown(EKeyCode key);
    static bool IsKeyPressed(EKeyCode key);
    static bool IsKeyReleased(EKeyCode key);

    static const Point& Wheel();
    static Point MouseMove();

private:
    Input();

    void SetButton(int button, bool pressed);
    void SetKey(int key, bool pressed);

    void SetCursor(float x, float y);
    void SetWheel(float x, float y);

    KeyArray mKeys;
    KeyArray mPrevKeys;

    ButtonArray mButtons;
    ButtonArray mPrevButtons;

    Point mCursor{ 0, 0 };
    Point mPrevCursor{ 0, 0 };

    Point mWheel{ 0, 0 };

    static Input gInput;

    friend class WindowManager;
};

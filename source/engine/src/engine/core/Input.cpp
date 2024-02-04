#include "Input.h"

Input Input::gInput;

Input::Input() {
    static int sCounter = 0;
    DEV_ASSERT(++sCounter == 1);

    mKeys.fill(false);
    mPrevKeys.fill(false);
    mButtons.fill(false);
    mPrevButtons.fill(false);
}

void Input::BeginFrame() {
    // called after glfwPollEvent, before everything else
}

void Input::EndFrame() {
    gInput.mPrevKeys = gInput.mKeys;
    gInput.mPrevButtons = gInput.mButtons;
    gInput.mPrevCursor = gInput.mCursor;

    gInput.mWheel = vec2(0);
}

template<size_t N>
static inline bool InputIsDown(const std::array<bool, N>& arr, int index) {
    DEV_ASSERT_INDEX(index, N);
    return arr[index];
}

template<size_t N>
static inline bool InputChange(const std::array<bool, N>& current, const std::array<bool, N>& prev, int index) {
    DEV_ASSERT_INDEX(index, N);
    return current[index] == true && prev[index] == false;
}

bool Input::IsButtonDown(EMouseButton key) { return InputIsDown(gInput.mButtons, underlying(key)); }

bool Input::IsButtonPressed(EMouseButton key) {
    return InputChange(gInput.mButtons, gInput.mPrevButtons, underlying(key));
}

bool Input::IsButtonReleased(EMouseButton key) {
    return InputChange(gInput.mPrevButtons, gInput.mButtons, underlying(key));
}

bool Input::IsKeyDown(EKeyCode key) { return InputIsDown(gInput.mKeys, underlying(key)); }

bool Input::IsKeyPressed(EKeyCode key) { return InputChange(gInput.mKeys, gInput.mPrevKeys, underlying(key)); }

bool Input::IsKeyReleased(EKeyCode key) { return InputChange(gInput.mPrevKeys, gInput.mKeys, underlying(key)); }

vec2 Input::MouseMove() {
    vec2 point;
    point = gInput.mCursor - gInput.mPrevCursor;
    return point;
}

const vec2& Input::GetCursor() { return gInput.mCursor; }

const vec2& Input::Wheel() { return gInput.mWheel; }

void Input::SetButton(int button, bool pressed) {
    DEV_ASSERT_INDEX(button, EMouseButton::COUNT);
    mButtons[button] = pressed;
}

void Input::SetKey(int key, bool pressed) {
    DEV_ASSERT_INDEX(key, EKeyCode::COUNT);
    mKeys[key] = pressed;
}

void Input::SetCursor(float x, float y) {
    mCursor.x = x;
    mCursor.y = y;
}

void Input::SetWheel(float x, float y) {
    mWheel.x = x;
    mWheel.y = y;
}

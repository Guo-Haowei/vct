#include "input.h"

#include <bitset>

namespace vct::input {

static struct {
    std::bitset<KEY_MAX> keys;
    std::bitset<KEY_MAX> prev_keys;
    std::bitset<MOUSE_BUTTON_MAX> buttons = { false };
    std::bitset<MOUSE_BUTTON_MAX> prev_buttons = { false };

    vec2 cursor{ 0, 0 };
    vec2 prev_cursor{ 0, 0 };

    vec2 wheel{ 0, 0 };
} s_glob;

void begin_frame() {
}

void end_frame() {
    s_glob.prev_keys = s_glob.keys;
    s_glob.prev_buttons = s_glob.buttons;
    s_glob.prev_cursor = s_glob.cursor;
    s_glob.wheel = vec2(0);
}

template<size_t N>
static inline bool input_is_down(const std::bitset<N>& arr, int index) {
    DEV_ASSERT_INDEX(index, N);
    return arr[index];
}

template<size_t N>
static inline bool input_has_changed(const std::bitset<N>& current, const std::bitset<N>& prev, int index) {
    DEV_ASSERT_INDEX(index, N);
    return current[index] == true && prev[index] == false;
}

bool is_button_down(MouseButton key) {
    return input_is_down(s_glob.buttons, std::to_underlying(key));
}

bool is_button_pressed(MouseButton key) {
    return input_has_changed(s_glob.buttons, s_glob.prev_buttons, std::to_underlying(key));
}

bool is_button_released(MouseButton key) {
    return input_has_changed(s_glob.prev_buttons, s_glob.buttons, std::to_underlying(key));
}

bool is_key_down(KeyCode key) {
    return input_is_down(s_glob.keys, std::to_underlying(key));
}

bool is_key_pressed(KeyCode key) {
    return input_has_changed(s_glob.keys, s_glob.prev_keys, std::to_underlying(key));
}

bool is_key_released(KeyCode key) {
    return input_has_changed(s_glob.prev_keys, s_glob.keys, std::to_underlying(key));
}

vec2 mouse_move() {
    vec2 point;
    point = s_glob.cursor - s_glob.prev_cursor;
    return point;
}

const vec2& get_cursor() { return s_glob.cursor; }

const vec2& get_wheel() { return s_glob.wheel; }

void set_button(int button, bool pressed) {
    DEV_ASSERT_INDEX(button, MOUSE_BUTTON_MAX);
    s_glob.buttons[button] = pressed;
}

void set_key(int key, bool pressed) {
    DEV_ASSERT_INDEX(key, KEY_MAX);
    s_glob.keys[key] = pressed;
}

void set_cursor(float x, float y) {
    s_glob.cursor.x = x;
    s_glob.cursor.y = y;
}

void set_wheel(float x, float y) {
    s_glob.wheel.x = x;
    s_glob.wheel.y = y;
}
}  // namespace vct::input

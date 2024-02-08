#pragma once
#include "core/math/geomath.h"
#include "input_code.h"

namespace vct::input {

void begin_frame();
void end_frame();

bool is_button_down(MouseButton key);
bool is_button_pressed(MouseButton key);
bool is_button_released(MouseButton key);

bool is_key_down(KeyCode key);
bool is_key_pressed(KeyCode key);
bool is_key_released(KeyCode key);

const vec2& get_cursor();
const vec2& get_wheel();
vec2 mouse_move();

void set_button(int button, bool pressed);
void set_key(int key, bool pressed);

void set_cursor(float x, float y);
void set_wheel(float x, float y);

};  // namespace vct::input

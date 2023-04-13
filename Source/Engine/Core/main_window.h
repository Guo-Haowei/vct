#pragma once
#include "Math/GeoMath.h"

struct GLFWwindow;

namespace MainWindow
{

[[nodiscard]] bool Init();
[[nodiscard]] bool ShouldClose();
[[nodiscard]] GLFWwindow* GetRaw();
[[nodiscard]] ivec2 FrameSize();
[[nodiscard]] vec2 MousePos();
[[nodiscard]] bool IsMouseInScreen();

[[nodiscard]] bool IsKeyDown(int code);

void NewFrame();
void Present();
void Shutdown();

}  // namespace MainWindow

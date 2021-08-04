#pragma once
#include "universal/core_math.h"

struct GLFWwindow;

namespace MainWindow {

[[nodiscard]] bool Init();
[[nodiscard]] bool ShouldClose();
[[nodiscard]] ivec2 FrameSize();
[[nodiscard]] GLFWwindow* GetRaw();

void NewFrame();
void Present();
void Shutdown();

}  // namespace MainWindow

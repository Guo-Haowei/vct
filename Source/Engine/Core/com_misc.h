#pragma once
#include "scene.h"

Scene& Com_GetScene();

[[nodiscard]] bool Com_LoadScene();

[[nodiscard]] bool Com_ImGuiInit();
void Com_ImGuiShutdown();

void Com_UpdateWorld();

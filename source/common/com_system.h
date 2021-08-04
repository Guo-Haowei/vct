#pragma once
#include "scene/Scene.h"

vct::Scene& Com_GetScene();

[[nodiscard]] bool Com_RegisterDvars();
[[nodiscard]] bool Com_LoadScene();

[[nodiscard]] bool Com_InitMainWindow();
void Com_DestroyMainWindow();

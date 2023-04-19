#pragma once
#include "scene.h"

Scene& Com_GetScene();

[[nodiscard]] bool Com_LoadScene();

void Com_UpdateWorld();

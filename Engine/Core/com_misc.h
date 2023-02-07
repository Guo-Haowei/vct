#pragma once
#include "SceneGraph/Scene.hpp"

Scene& Com_GetScene();

[[nodiscard]] bool Com_LoadScene();

void Com_UpdateWorld();

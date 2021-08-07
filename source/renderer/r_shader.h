#pragma once

#include "GlslProgram.h"

enum class ProgramType {
    IMAGE2D,
    LINE3D,
    SHADOW,
    COUNT,
};

void R_CreateShaderPrograms();
void R_DestroyShaderPrograms();

const GlslProgram& R_GetShaderProgram( ProgramType type );

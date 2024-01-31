#include "Program.h"

#include "GLPrerequisites.h"

void Program::Bind() const {
    DEV_ASSERT(mHandle);
    glUseProgram(mHandle);
}

void Program::Unbind() const { glUseProgram(0); }

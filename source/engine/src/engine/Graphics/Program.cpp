#include "Program.h"

#include "Core/Check.h"
#include "GLPrerequisites.h"

void Program::Bind() const {
    check(mHandle);
    glUseProgram(mHandle);
}

void Program::Unbind() const { glUseProgram(0); }

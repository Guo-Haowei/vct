#include "Program.h"

#include "GLPrerequisites.h"

#include "Core/Check.h"

void Program::Bind() const
{
    check(mHandle);
    glUseProgram(mHandle);
}

void Program::Unbind() const
{
    glUseProgram(0);
}

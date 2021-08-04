#include "r_graphics.h"

#include "gl_utils.h"

[[nodiscard]] bool R_Init()
{
    return gl::Init();
}
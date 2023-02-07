#pragma once
#include "../Base/Defines.h"

#if defined( USE_DX11 )
#define DX11_PROGRAM   IN_USE
#define OPENGL_PROGRAM NOT_IN_USE
#else
#define DX11_PROGRAM   NOT_IN_USE
#define OPENGL_PROGRAM IN_USE
#endif

#if USING( DX11_PROGRAM )
#include "D3d11/D3d11GraphicsManager.hpp"
#include "D3d11/D3d11PipelineStateManager.hpp"
using TGraphicsManager = D3d11GraphicsManager;
using TPipelineStateManager = D3d11PipelineStateManager;
#else
#include "OpenGL/OpenGLGraphicsManager.hpp"
#include "OpenGL/OpenGLPipelineStateManager.hpp"
using TGraphicsManager = OpenGLGraphicsManager;
using TPipelineStateManager = OpenGLPipelineStateManager;
#endif

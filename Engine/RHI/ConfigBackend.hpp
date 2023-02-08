#pragma once
#include "../Base/Defines.h"
#include "../Core/Configuration.hpp"

#if !defined( _USE_DX11 ) && !defined( _USE_DX12 )
#define _USE_OPENGL
#endif

#if defined( _USE_OPENGL )
#define OPENGL_PROGRAM IN_USE
#define DX11_PROGRAM   NOT_IN_USE
#define DX12_PROGRAM   NOT_IN_USE
#elif defined( _USE_DX11 )
#define OPENGL_PROGRAM NOT_IN_USE
#define DX11_PROGRAM   IN_USE
#define DX12_PROGRAM   NOT_IN_USE
#elif defined( _USE_DX12 )
#define OPENGL_PROGRAM NOT_IN_USE
#define DX11_PROGRAM   NOT_IN_USE
#define DX12_PROGRAM   IN_USE
#else
#error "Unknown backend"
#define OPENGL_PROGRAM NOT_IN_USE
#define DX11_PROGRAM   NOT_IN_USE
#define DX12_PROGRAM   NOT_IN_USE
#endif

#if USING( OPENGL_PROGRAM )
#include "OpenGL/OpenGLGraphicsManager.hpp"
#include "OpenGL/OpenGLPipelineStateManager.hpp"
using TGraphicsManager = OpenGLGraphicsManager;
using TPipelineStateManager = OpenGLPipelineStateManager;
constexpr GfxBackend DEFAULT_GFX_BACKEND = GfxBackend::OpenGL;
#endif

#if USING( DX11_PROGRAM )
#include "D3d11/D3d11GraphicsManager.hpp"
#include "D3d11/D3d11PipelineStateManager.hpp"
using TGraphicsManager = D3d11GraphicsManager;
using TPipelineStateManager = D3d11PipelineStateManager;
constexpr GfxBackend DEFAULT_GFX_BACKEND = GfxBackend::D3d11;
#endif

#if USING( DX12_PROGRAM )
#include "D3d12/D3d12GraphicsManager.hpp"
#include "D3d12/D3d12PipelineStateManager.hpp"
using TGraphicsManager = D3d12GraphicsManager;
using TPipelineStateManager = D3d12PipelineStateManager;
constexpr GfxBackend DEFAULT_GFX_BACKEND = GfxBackend::D3d12;
#endif

#pragma once

//#define JSH_DEBUG
#define JSH_PLATFORM_WINDOWS
#define JSH_DEBUG

// debug macros
#ifdef JSH_DEBUG

#define JSH_IMGUI
#define JSH_CONSOLE

#else

#define NDEBUG

#endif

#include <assert.h>

// platform macros
#ifndef JSH_PLATFORM_WINDOWS
#error Platform not supported
#endif

// includes
#include <stdint.h>
#include <DirectXMath.h>
#include <string>

using namespace DirectX;

using byte		= uint8_t;
using wchar		= wchar_t;
using ID_t		= uint32_t;

using uint8		= uint8_t;
using uint16	= uint16_t;
using uint32	= uint32_t;
using uint64	= uint64_t;

using int8		= int8_t;
using int16		= int16_t;
using int32		= int32_t;
using int64		= int64_t;
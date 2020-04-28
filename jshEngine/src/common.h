#pragma once

#include "config.h"

#include <assert.h>

// imgui macros
#ifdef JSH_IMGUI

#define jshImGui(x) x
#include "ImGui/imgui.h"

#else

#define jshImGui(x)

#endif 

#define BIT(x) (1 << x)
#define jshZeroMemory(dest, size) memset(dest, 0u, size)

// platform macros
#ifndef JSH_PLATFORM_WINDOWS
#error Platform not supported
#endif

// includes
#include <string>
#include <memory>
#include <sstream>
#include <algorithm>
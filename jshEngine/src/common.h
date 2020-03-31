#pragma once

#define JSH_DEBUG
#define JSH_PLATFORM_WINDOWS

// debug macros
#ifdef JSH_DEBUG

#define JSH_IMGUI
#define JSH_CONSOLE

#else

#define NDEBUG

#endif

#include <assert.h>

// imgui macros
#ifdef JSH_IMGUI

#define jshImGui(x) x
#include "ImGui/imgui.h"

#else

#define jshImGui(x)

#endif 

#define BIT(x) 1 << x

// platform macros
#ifndef JSH_PLATFORM_WINDOWS
#error Platform not supported
#endif

// includes
#include "utils/Math.h"
#include <string>

constexpr const wchar* JSH_FILEPATH_ASSET = L"..//assets/";
constexpr const wchar* JSH_FILEPATH_RES = L"..//res/";
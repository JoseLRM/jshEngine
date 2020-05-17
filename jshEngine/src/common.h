#pragma once

#include "config.h"

///////////////////////////STD INCLUDES////////////////////////////////////////

// math
#include <cmath>
#include <DirectXMath.h>

// misc
#include <memory>
#include <algorithm>
#include <functional>

// thread
#include <mutex>
#include <condition_variable>
#include <atomic>

// data structures
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <queue>
#include <map>

using namespace DirectX;

// DEBUGGING
#include "Debug.h"

#ifdef JSH_DEBUG
#define JSH_ASSERT(x) do{ if((x) == false) { jshDebug::_internal::LogA(#x, __LINE__, __FILE__); } } while(0)
#else
#define JSH_ASSERT(x)
#endif

// IMGUI
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

//////////////////////////////JSH INCLUDES////////////////////////////////////////

// math
#include "types.h"
#include "Math.h"

// data structures
#include "Archive.h"

// misc
#include "Engine.h"
#include "Timer.h"
#include "TaskSystem.h"
#include "Scene.h"
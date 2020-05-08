#pragma once

#include "Math.h"

#define JSH_DEBUG
#define JSH_IMGUI
#define JSH_CONSOLE
#define JSH_PLATFORM_WINDOWS

#define JSH_ECS_MAX_COMPONENTS_TYPES 64
#define JSH_ECS_NAMECOMPONENT_SIZE 32

// GRAPHICS
#define JSH_GFX_BATCH_COUNT 10000u
#define JSH_GFX_MAX_INSTANCE_SIZE 88u

#define JSH_GFX_COMMANDLISTS_COUNT 32

#define JSH_GFX_VERTEX_BUFFERS_COUNT 16
#define JSH_GFX_CONSTANT_BUFFERS_COUNT 16
#define JSH_GFX_INPUT_LAYOUT_COUNT 16
#define JSH_GFX_TEXTURES_COUNT 32
#define JSH_GFX_SAMPLER_STATES_COUNT 16
#define JSH_GFX_RASTERIZER_STATES_COUNT 16
#define JSH_GFX_BLEND_STATES_COUNT 16
#define JSH_GFX_RENDER_TARGETS_COUNT 16
#define JSH_GFX_VIEWPORTS_COUNT 16

#define JSH_GFX_MAX_LIGHTS 120
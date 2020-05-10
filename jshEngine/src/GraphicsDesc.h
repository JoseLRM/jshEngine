#pragma once

#include "common.h"
#include "Exception.h"

#define jshGfxException jsh::Exception(L"Graphics Exception")
#define jshGfx(x) if(x != 0) throw jshGfxException
#define jshGfxNullAPIException jsh::Exception(L"Invalid GraphicsAPI");

// shader slots
#define JSH_GFX_SLOT_CBUFFER_CAMERA		0
#define JSH_GFX_SLOT_CBUFFER_LIGHTS		1
#define JSH_GFX_SLOT_CBUFFER_FREE0		2

#define JSH_GFX_SLOT_TEXTURE2D_FREE0	0
#define JSH_GFX_SLOT_TEXTURE2D_FREE1	1
#define JSH_GFX_SLOT_TEXTURE2D_FREE2	2
#define JSH_GFX_SLOT_TEXTURE2D_FREE3	3
#define JSH_GFX_SLOT_TEXTURE2D_FREE4	4
#define JSH_GFX_SLOT_TEXTURE2D_FREE5	5
#define JSH_GFX_SLOT_TEXTURE2D_FREE6	6
#define JSH_GFX_SLOT_TEXTURE2D_FREE7	7
#define JSH_GFX_SLOT_TEXTURE2D_FREE8	8
#define JSH_GFX_SLOT_TEXTURE2D_FREE9	9
#define JSH_GFX_SLOT_TEXTURE2D_FREE10	10
#define JSH_GFX_SLOT_TEXTURE2D_FREE11	11
#define JSH_GFX_SLOT_TEXTURE2D_FREE12	12
#define JSH_GFX_SLOT_TEXTURE2D_FREE13	13
#define JSH_GFX_SLOT_TEXTURE2D_FREE14	14
#define JSH_GFX_SLOT_TEXTURE2D_FREE15	15

#define JSH_GFX_SLOT_SAMPLER_FREE0		0
#define JSH_GFX_SLOT_SAMPLER_FREE1		1
#define JSH_GFX_SLOT_SAMPLER_FREE2		2
#define JSH_GFX_SLOT_SAMPLER_FREE3		3
#define JSH_GFX_SLOT_SAMPLER_FREE4		4
#define JSH_GFX_SLOT_SAMPLER_FREE5		5
#define JSH_GFX_SLOT_SAMPLER_FREE6		6
#define JSH_GFX_SLOT_SAMPLER_FREE7		7
#define JSH_GFX_SLOT_SAMPLER_FREE8		8
#define JSH_GFX_SLOT_SAMPLER_FREE9		9
#define JSH_GFX_SLOT_SAMPLER_FREE10		10
#define JSH_GFX_SLOT_SAMPLER_FREE11		11
#define JSH_GFX_SLOT_SAMPLER_FREE12		12
#define JSH_GFX_SLOT_SAMPLER_FREE13		13
#define JSH_GFX_SLOT_SAMPLER_FREE14		14
#define JSH_GFX_SLOT_SAMPLER_FREE15		15

enum JSH_GRAPHICS_API : uint8 {
	JSH_GRAPHCS_API_NULL, 
	JSH_GRAPHCS_API_DIRECTX11
};

enum JSH_PRIMITIVE_TYPE : uint8 {
	JSH_PRIMITIVE_INVALID,
	JSH_PRIMITIVE_VERTEX_BUFFER,
	JSH_PRIMITIVE_INDEX_BUFFER,
	JSH_PRIMITIVE_CONSTANT_BUFFER,
	JSH_PRIMITIVE_VERTEX_SHADER,
	JSH_PRIMITIVE_PIXEL_SHADER,
	JSH_PRIMITIVE_GEOMETRY_SHADER,
	JSH_PRIMITIVE_HULL_SHADER,
	JSH_PRIMITIVE_DOMAIN_SHADER,
	JSH_PRIMITIVE_COMPUTE_SHADER,
	JSH_PRIMITIVE_INPUT_LAYOUT,
	JSH_PRIMITIVE_TEXTURE,
	JSH_PRIMITIVE_SAMPLER_STATE,
	JSH_PRIMITIVE_BLEND_STATE,
	JSH_PRIMITIVE_DEPTHSTENCIL_STATE,
	JSH_PRIMITIVE_RASTERIZER_STATE,
	JSH_PRIMITIVE_RENDER_TARGET_VIEW
};

enum JSH_BIND_FLAG
{
	JSH_BIND_VERTEX_BUFFER = 0x1L,
	JSH_BIND_INDEX_BUFFER = 0x2L,
	JSH_BIND_CONSTANT_BUFFER = 0x4L,
	JSH_BIND_SHADER_RESOURCE = 0x8L,
	JSH_BIND_STREAM_OUTPUT = 0x10L,
	JSH_BIND_RENDER_TARGET = 0x20L,
	JSH_BIND_DEPTH_STENCIL = 0x40L,
	JSH_BIND_UNORDERED_ACCESS = 0x80L,
	JSH_BIND_DECODER = 0x200L,
	JSH_BIND_VIDEO_ENCODER = 0x400L
};
enum JSH_TOPOLOGY {
	JSH_TOPOLOGY_TRIANGLES,
	JSH_TOPOLOGY_TRIANGLE_STRIP,
	JSH_TOPOLOGY_LINES,
	JSH_TOPOLOGY_LINE_STRIP,
	JSH_TOPOLOGY_POINTS
};

namespace jshGraphics {

	constexpr uint32 GetTopologyVertexCount(JSH_TOPOLOGY topology)
	{
		switch (topology)
		{
		case JSH_TOPOLOGY_TRIANGLES:
			return 3;
		case JSH_TOPOLOGY_TRIANGLE_STRIP:
			return 3;
		case JSH_TOPOLOGY_LINES:
			return 2;
		case JSH_TOPOLOGY_LINE_STRIP:
			return 2;
		case JSH_TOPOLOGY_POINTS:
			return 1;
		default:
			return 0;
		}
	}

}

enum JSH_USAGE {
	JSH_USAGE_DEFAULT = 0,
	JSH_USAGE_IMMUTABLE = 1,
	JSH_USAGE_DYNAMIC = 2,
	JSH_USAGE_STAGING = 3
};

enum JSH_SHADER_TYPE : uint8 {
	JSH_SHADER_TYPE_NULL,
	JSH_SHADER_TYPE_VERTEX,
	JSH_SHADER_TYPE_PIXEL,
	JSH_SHADER_TYPE_GEOMETRY
};

enum JSH_BUFFER_TYPE : uint8 {
	JSH_BUFFER_TYPE_NULL,
	JSH_BUFFER_TYPE_VERTEX,
	JSH_BUFFER_TYPE_INDEX,
	JSH_BUFFER_TYPE_CONSTANT
};

enum JSH_MODE_SCALING
{
	JSH_MODE_SCALING_UNSPECIFIED = 0,
	JSH_MODE_SCALING_CENTERED = 1,
	JSH_MODE_SCALING_STRETCHED = 2
};

enum JSH_MODE_SCANLINE_ORDER
{
	JSH_MODE_SCANLINE_ORDER_UNSPECIFIED = 0,
	JSH_MODE_SCANLINE_ORDER_PROGRESSIVE = 1,
	JSH_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST = 2,
	JSH_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST = 3
};

enum JSH_FORMAT {
	JSH_FORMAT_UNKNOWN = 0,
	JSH_FORMAT_R32G32B32A32_TYPELESS = 1,
	JSH_FORMAT_R32G32B32A32_FLOAT = 2,
	JSH_FORMAT_R32G32B32A32_UINT = 3,
	JSH_FORMAT_R32G32B32A32_SINT = 4,
	JSH_FORMAT_R32G32B32_TYPELESS = 5,
	JSH_FORMAT_R32G32B32_FLOAT = 6,
	JSH_FORMAT_R32G32B32_UINT = 7,
	JSH_FORMAT_R32G32B32_SINT = 8,
	JSH_FORMAT_R16G16B16A16_TYPELESS = 9,
	JSH_FORMAT_R16G16B16A16_FLOAT = 10,
	JSH_FORMAT_R16G16B16A16_UNORM = 11,
	JSH_FORMAT_R16G16B16A16_UINT = 12,
	JSH_FORMAT_R16G16B16A16_SNORM = 13,
	JSH_FORMAT_R16G16B16A16_SINT = 14,
	JSH_FORMAT_R32G32_TYPELESS = 15,
	JSH_FORMAT_R32G32_FLOAT = 16,
	JSH_FORMAT_R32G32_UINT = 17,
	JSH_FORMAT_R32G32_SINT = 18,
	JSH_FORMAT_R32G8X24_TYPELESS = 19,
	JSH_FORMAT_D32_FLOAT_S8X24_UINT = 20,
	JSH_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
	JSH_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
	JSH_FORMAT_R10G10B10A2_TYPELESS = 23,
	JSH_FORMAT_R10G10B10A2_UNORM = 24,
	JSH_FORMAT_R10G10B10A2_UINT = 25,
	JSH_FORMAT_R11G11B10_FLOAT = 26,
	JSH_FORMAT_R8G8B8A8_TYPELESS = 27,
	JSH_FORMAT_R8G8B8A8_UNORM = 28,
	JSH_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
	JSH_FORMAT_R8G8B8A8_UINT = 30,
	JSH_FORMAT_R8G8B8A8_SNORM = 31,
	JSH_FORMAT_R8G8B8A8_SINT = 32,
	JSH_FORMAT_R16G16_TYPELESS = 33,
	JSH_FORMAT_R16G16_FLOAT = 34,
	JSH_FORMAT_R16G16_UNORM = 35,
	JSH_FORMAT_R16G16_UINT = 36,
	JSH_FORMAT_R16G16_SNORM = 37,
	JSH_FORMAT_R16G16_SINT = 38,
	JSH_FORMAT_R32_TYPELESS = 39,
	JSH_FORMAT_D32_FLOAT = 40,
	JSH_FORMAT_R32_FLOAT = 41,
	JSH_FORMAT_R32_UINT = 42,
	JSH_FORMAT_R32_SINT = 43,
	JSH_FORMAT_R24G8_TYPELESS = 44,
	JSH_FORMAT_D24_UNORM_S8_UINT = 45,
	JSH_FORMAT_R24_UNORM_X8_TYPELESS = 46,
	JSH_FORMAT_X24_TYPELESS_G8_UINT = 47,
	JSH_FORMAT_R8G8_TYPELESS = 48,
	JSH_FORMAT_R8G8_UNORM = 49,
	JSH_FORMAT_R8G8_UINT = 50,
	JSH_FORMAT_R8G8_SNORM = 51,
	JSH_FORMAT_R8G8_SINT = 52,
	JSH_FORMAT_R16_TYPELESS = 53,
	JSH_FORMAT_R16_FLOAT = 54,
	JSH_FORMAT_D16_UNORM = 55,
	JSH_FORMAT_R16_UNORM = 56,
	JSH_FORMAT_R16_UINT = 57,
	JSH_FORMAT_R16_SNORM = 58,
	JSH_FORMAT_R16_SINT = 59,
	JSH_FORMAT_R8_TYPELESS = 60,
	JSH_FORMAT_R8_UNORM = 61,
	JSH_FORMAT_R8_UINT = 62,
	JSH_FORMAT_R8_SNORM = 63,
	JSH_FORMAT_R8_SINT = 64,
	JSH_FORMAT_A8_UNORM = 65,
	JSH_FORMAT_R1_UNORM = 66,
	JSH_FORMAT_R9G9B9E5_SHAREDEXP = 67,
	JSH_FORMAT_R8G8_B8G8_UNORM = 68,
	JSH_FORMAT_G8R8_G8B8_UNORM = 69,
	JSH_FORMAT_BC1_TYPELESS = 70,
	JSH_FORMAT_BC1_UNORM = 71,
	JSH_FORMAT_BC1_UNORM_SRGB = 72,
	JSH_FORMAT_BC2_TYPELESS = 73,
	JSH_FORMAT_BC2_UNORM = 74,
	JSH_FORMAT_BC2_UNORM_SRGB = 75,
	JSH_FORMAT_BC3_TYPELESS = 76,
	JSH_FORMAT_BC3_UNORM = 77,
	JSH_FORMAT_BC3_UNORM_SRGB = 78,
	JSH_FORMAT_BC4_TYPELESS = 79,
	JSH_FORMAT_BC4_UNORM = 80,
	JSH_FORMAT_BC4_SNORM = 81,
	JSH_FORMAT_BC5_TYPELESS = 82,
	JSH_FORMAT_BC5_UNORM = 83,
	JSH_FORMAT_BC5_SNORM = 84,
	JSH_FORMAT_B5G6R5_UNORM = 85,
	JSH_FORMAT_B5G5R5A1_UNORM = 86,
	JSH_FORMAT_B8G8R8A8_UNORM = 87,
	JSH_FORMAT_B8G8R8X8_UNORM = 88,
	JSH_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
	JSH_FORMAT_B8G8R8A8_TYPELESS = 90,
	JSH_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
	JSH_FORMAT_B8G8R8X8_TYPELESS = 92,
	JSH_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
	JSH_FORMAT_BC6H_TYPELESS = 94,
	JSH_FORMAT_BC6H_UF16 = 95,
	JSH_FORMAT_BC6H_SF16 = 96,
	JSH_FORMAT_BC7_TYPELESS = 97,
	JSH_FORMAT_BC7_UNORM = 98,
	JSH_FORMAT_BC7_UNORM_SRGB = 99,
	JSH_FORMAT_AYUV = 100,
	JSH_FORMAT_Y410 = 101,
	JSH_FORMAT_Y416 = 102,
	JSH_FORMAT_NV12 = 103,
	JSH_FORMAT_P010 = 104,
	JSH_FORMAT_P016 = 105,
	JSH_FORMAT_420_OPAQUE = 106,
	JSH_FORMAT_YUY2 = 107,
	JSH_FORMAT_Y210 = 108,
	JSH_FORMAT_Y216 = 109,
	JSH_FORMAT_NV11 = 110,
	JSH_FORMAT_AI44 = 111,
	JSH_FORMAT_IA44 = 112,
	JSH_FORMAT_P8 = 113,
	JSH_FORMAT_A8P8 = 114,
	JSH_FORMAT_B4G4R4A4_UNORM = 115,
	JSH_FORMAT_P208 = 130,
	JSH_FORMAT_V208 = 131,
	JSH_FORMAT_V408 = 132,
	JSH_FORMAT_FORCE_UINT = 0xffffffff

};

namespace jshGraphics {
	constexpr uint32 GetFormatStride(JSH_FORMAT format) {
		switch (format)
		{
		case JSH_FORMAT_UNKNOWN:
			return 0u;
		case JSH_FORMAT_R32G32B32A32_TYPELESS:
			return 16u;
		case JSH_FORMAT_R32G32B32A32_FLOAT:
			return 16u;
		case JSH_FORMAT_R32G32B32A32_UINT:
			return 16u;
		case JSH_FORMAT_R32G32B32A32_SINT:
			return 16u;
		case JSH_FORMAT_R32G32B32_TYPELESS:
			return 12u;
		case JSH_FORMAT_R32G32B32_FLOAT:
			return 12u;
		case JSH_FORMAT_R32G32B32_UINT:
			return 12u;
		case JSH_FORMAT_R32G32B32_SINT:
			return 12u;
		case JSH_FORMAT_R16G16B16A16_TYPELESS:
			return 8u;
		case JSH_FORMAT_R16G16B16A16_FLOAT:
			return 8u;
		case JSH_FORMAT_R16G16B16A16_UNORM:
			return 8u;
		case JSH_FORMAT_R16G16B16A16_UINT:
			return 8u;
		case JSH_FORMAT_R16G16B16A16_SNORM:
			return 8u;
		case JSH_FORMAT_R16G16B16A16_SINT:
			return 8u;
		case JSH_FORMAT_R32G32_TYPELESS:
			return 8u;
		case JSH_FORMAT_R32G32_FLOAT:
			return 8u;
		case JSH_FORMAT_R32G32_UINT:
			return 8u;
		case JSH_FORMAT_R32G32_SINT:
			return 8u;
		case JSH_FORMAT_R8G8B8A8_TYPELESS:
			return 4u;
		case JSH_FORMAT_R8G8B8A8_UNORM:
			return 4u;
		case JSH_FORMAT_R8G8B8A8_UNORM_SRGB:
			return 4u;
		case JSH_FORMAT_R8G8B8A8_UINT:
			return 4u;
		case JSH_FORMAT_R8G8B8A8_SNORM:
			return 4u;
		case JSH_FORMAT_R8G8B8A8_SINT:
			return 4u;
		case JSH_FORMAT_R16G16_TYPELESS:
			return 4u;
		case JSH_FORMAT_R16G16_FLOAT:
			return 4u;
		case JSH_FORMAT_R16G16_UNORM:
			return 4u;
		case JSH_FORMAT_R16G16_UINT:
			return 4u;
		case JSH_FORMAT_R16G16_SNORM:
			return 4u;
		case JSH_FORMAT_R16G16_SINT:
			return 4u;
		case JSH_FORMAT_R32_TYPELESS:
			return 4u;
		case JSH_FORMAT_D32_FLOAT:
			return 4u;
		case JSH_FORMAT_R32_FLOAT:
			return 4u;
		case JSH_FORMAT_R32_UINT:
			return 4u;
		case JSH_FORMAT_R32_SINT:
			return 4u;
		case JSH_FORMAT_R24G8_TYPELESS:
			return 4u;
		case JSH_FORMAT_D24_UNORM_S8_UINT:
			return 4u;
		case JSH_FORMAT_R24_UNORM_X8_TYPELESS:
			return 4u;
		case JSH_FORMAT_X24_TYPELESS_G8_UINT:
			return 4u;
		case JSH_FORMAT_R8G8_TYPELESS:
			return 2u;
		case JSH_FORMAT_R8G8_UNORM:
			return 2u;
		case JSH_FORMAT_R8G8_UINT:
			return 2u;
		case JSH_FORMAT_R8G8_SNORM:
			return 2u;
		case JSH_FORMAT_R8G8_SINT:
			return 2u;
		case JSH_FORMAT_R16_TYPELESS:
			return 2u;
		case JSH_FORMAT_R16_FLOAT:
			return 2u;
		case JSH_FORMAT_D16_UNORM:
			return 2u;
		case JSH_FORMAT_R16_UNORM:
			return 2u;
		case JSH_FORMAT_R16_UINT:
			return 2u;
		case JSH_FORMAT_R16_SNORM:
			return 2u;
		case JSH_FORMAT_R16_SINT:
			return 2u;
		case JSH_FORMAT_R8_TYPELESS:
			return 1u;
		case JSH_FORMAT_R8_UNORM:
			return 1u;
		case JSH_FORMAT_R8_UINT:
			return 1u;
		case JSH_FORMAT_R8_SNORM:
			return 1u;
		case JSH_FORMAT_R8_SINT:
			return 1u;
		case JSH_FORMAT_A8_UNORM:
			return 1u;
		case JSH_FORMAT_R8G8_B8G8_UNORM:
			return 4u;
		case JSH_FORMAT_G8R8_G8B8_UNORM:
			return 4u;
		case JSH_FORMAT_B8G8R8A8_UNORM:
			return 4u;
		case JSH_FORMAT_B8G8R8X8_UNORM:
			return 4u;
		case JSH_FORMAT_B8G8R8A8_TYPELESS:
			return 4u;
		case JSH_FORMAT_B8G8R8A8_UNORM_SRGB:
			return 4u;
		case JSH_FORMAT_B8G8R8X8_TYPELESS:
			return 4u;
		case JSH_FORMAT_B8G8R8X8_UNORM_SRGB:
			return 4u;
		default:
			assert("Unknown stride :(");
			return 0u;
		}
	}
}

enum JSH_TEXTURE_ADDRESS_MODE
{
	JSH_TEXTURE_ADDRESS_WRAP = 1,
	JSH_TEXTURE_ADDRESS_MIRROR = 2,
	JSH_TEXTURE_ADDRESS_CLAMP = 3,
	JSH_TEXTURE_ADDRESS_BORDER = 4,
	JSH_TEXTURE_ADDRESS_MIRROR_ONCE = 5
};

enum JSH_FILTER
{
	JSH_FILTER_MIN_MAG_MIP_POINT = 0,
	JSH_FILTER_MIN_MAG_POINT_MIP_LINEAR = 0x1,
	JSH_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x4,
	JSH_FILTER_MIN_POINT_MAG_MIP_LINEAR = 0x5,
	JSH_FILTER_MIN_LINEAR_MAG_MIP_POINT = 0x10,
	JSH_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x11,
	JSH_FILTER_MIN_MAG_LINEAR_MIP_POINT = 0x14,
	JSH_FILTER_MIN_MAG_MIP_LINEAR = 0x15,
	JSH_FILTER_ANISOTROPIC = 0x55,
	JSH_FILTER_COMPARISON_MIN_MAG_MIP_POINT = 0x80,
	JSH_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR = 0x81,
	JSH_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x84,
	JSH_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR = 0x85,
	JSH_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT = 0x90,
	JSH_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x91,
	JSH_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT = 0x94,
	JSH_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR = 0x95,
	JSH_FILTER_COMPARISON_ANISOTROPIC = 0xd5,
	JSH_FILTER_MINIMUM_MIN_MAG_MIP_POINT = 0x100,
	JSH_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x101,
	JSH_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x104,
	JSH_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x105,
	JSH_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x110,
	JSH_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x111,
	JSH_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x114,
	JSH_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR = 0x115,
	JSH_FILTER_MINIMUM_ANISOTROPIC = 0x155,
	JSH_FILTER_MAXIMUM_MIN_MAG_MIP_POINT = 0x180,
	JSH_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x181,
	JSH_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x184,
	JSH_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x185,
	JSH_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x190,
	JSH_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x191,
	JSH_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x194,
	JSH_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR = 0x195,
	JSH_FILTER_MAXIMUM_ANISOTROPIC = 0x1d5
};

///////////////////////////DESCRIPTORS//////////////////////////

// RESOURCES
struct JSH_BUFFER_DESC
{
	uint32 ByteWidth;
	JSH_USAGE Usage;
	uint32 BindFlags;
	uint32 CPUAccessFlags;
	uint32 MiscFlags;
	uint32 StructureByteStride;
};

enum JSH_CPU_ACCESS_FLAG
{
	JSH_CPU_ACCESS_WRITE = 0x10000L,
	JSH_CPU_ACCESS_READ = 0x20000L
};

struct JSH_SAMPLE_DESC
{
	uint32 Count;
	uint32 Quality;
};

struct JSH_TEXTURE2D_DESC
{
	uint32 Width;
	uint32 Height;
	uint32 MipLevels;
	uint32 ArraySize;
	JSH_FORMAT Format;
	JSH_SAMPLE_DESC SampleDesc;
	JSH_USAGE Usage;
	uint32 BindFlags;
	uint32 CPUAccessFlags;
	uint32 MiscFlags;
};

struct JSH_SUBRESOURCE_DATA
{
	const void* pSysMem;
	uint32 SysMemPitch;
	uint32 SysMemSlicePitch;
};

// INPUT LAYOUT
struct JSH_INPUT_ELEMENT_DESC {
	const char* semanticName;
	uint32 semanticIndex;
	JSH_FORMAT format;
	uint32 inputSlot;
	bool perVertexData;
	uint32 alignedByteOffset;
	uint32 instanceDataStepRate;
};

// SAMPLER
enum JSH_COMPARISON_FUNC
{
	JSH_COMPARISON_NEVER = 1,
	JSH_COMPARISON_LESS = 2,
	JSH_COMPARISON_EQUAL = 3,
	JSH_COMPARISON_LESS_EQUAL = 4,
	JSH_COMPARISON_GREATER = 5,
	JSH_COMPARISON_NOT_EQUAL = 6,
	JSH_COMPARISON_GREATER_EQUAL = 7,
	JSH_COMPARISON_ALWAYS = 8
};

struct JSH_SAMPLER_DESC
{
	JSH_FILTER Filter;
	JSH_TEXTURE_ADDRESS_MODE AddressU;
	JSH_TEXTURE_ADDRESS_MODE AddressV;
	JSH_TEXTURE_ADDRESS_MODE AddressW;
	float MipLODBias;
	uint32 MaxAnisotropy;
	JSH_COMPARISON_FUNC ComparisonFunc;
	float BorderColor[4];
	float MinLOD;
	float MaxLOD;
};

// BLEND
enum JSH_COLOR_WRITE_ENABLE
{
	JSH_COLOR_WRITE_ENABLE_RED = 1,
	JSH_COLOR_WRITE_ENABLE_GREEN = 2,
	JSH_COLOR_WRITE_ENABLE_BLUE = 4,
	JSH_COLOR_WRITE_ENABLE_ALPHA = 8,
	JSH_COLOR_WRITE_ENABLE_ALL = (((JSH_COLOR_WRITE_ENABLE_RED | JSH_COLOR_WRITE_ENABLE_GREEN) | JSH_COLOR_WRITE_ENABLE_BLUE) | JSH_COLOR_WRITE_ENABLE_ALPHA)
};

enum JSH_BLEND
{
	JSH_BLEND_ZERO = 1,
	JSH_BLEND_ONE = 2,
	JSH_BLEND_SRC_COLOR = 3,
	JSH_BLEND_INV_SRC_COLOR = 4,
	JSH_BLEND_SRC_ALPHA = 5,
	JSH_BLEND_INV_SRC_ALPHA = 6,
	JSH_BLEND_DEST_ALPHA = 7,
	JSH_BLEND_INV_DEST_ALPHA = 8,
	JSH_BLEND_DEST_COLOR = 9,
	JSH_BLEND_INV_DEST_COLOR = 10,
	JSH_BLEND_SRC_ALPHA_SAT = 11,
	JSH_BLEND_BLEND_FACTOR = 14,
	JSH_BLEND_INV_BLEND_FACTOR = 15,
	JSH_BLEND_SRC1_COLOR = 16,
	JSH_BLEND_INV_SRC1_COLOR = 17,
	JSH_BLEND_SRC1_ALPHA = 18,
	JSH_BLEND_INV_SRC1_ALPHA = 19
};

enum JSH_BLEND_OP
{
	JSH_BLEND_OP_ADD = 1,
	JSH_BLEND_OP_SUBTRACT = 2,
	JSH_BLEND_OP_REV_SUBTRACT = 3,
	JSH_BLEND_OP_MIN = 4,
	JSH_BLEND_OP_MAX = 5
};

struct JSH_RENDER_TARGET_BLEND_DESC
{
	bool BlendEnable;
	JSH_BLEND SrcBlend;
	JSH_BLEND DestBlend;
	JSH_BLEND_OP BlendOp;
	JSH_BLEND SrcBlendAlpha;
	JSH_BLEND DestBlendAlpha;
	JSH_BLEND_OP BlendOpAlpha;
	uint8 RenderTargetWriteMask;
};

struct JSH_BLEND_DESC
{
	bool AlphaToCoverageEnable;
	bool IndependentBlendEnable;
	JSH_RENDER_TARGET_BLEND_DESC RenderTarget[8];
};

// DEPTHSTENCIL
enum JSH_DEPTH_WRITE_MASK
{
	JSH_DEPTH_WRITE_MASK_ZERO = 0,
	JSH_DEPTH_WRITE_MASK_ALL = 1
};
enum JSH_STENCIL_OP
{
	JSH_STENCIL_OP_KEEP = 1,
	JSH_STENCIL_OP_ZERO = 2,
	JSH_STENCIL_OP_REPLACE = 3,
	JSH_STENCIL_OP_INCR_SAT = 4,
	JSH_STENCIL_OP_DECR_SAT = 5,
	JSH_STENCIL_OP_INVERT = 6,
	JSH_STENCIL_OP_INCR = 7,
	JSH_STENCIL_OP_DECR = 8
};
struct JSH_DEPTH_STENCILOP_DESC
{
	JSH_STENCIL_OP StencilFailOp;
	JSH_STENCIL_OP StencilDepthFailOp;
	JSH_STENCIL_OP StencilPassOp;
	JSH_COMPARISON_FUNC StencilFunc;
};
struct JSH_DEPTH_STENCIL_DESC
{
	bool DepthEnable;
	JSH_DEPTH_WRITE_MASK DepthWriteMask;
	JSH_COMPARISON_FUNC DepthFunc;
	bool StencilEnable;
	uint8 StencilReadMask;
	uint8 StencilWriteMask;
	JSH_DEPTH_STENCILOP_DESC FrontFace;
	JSH_DEPTH_STENCILOP_DESC BackFace;
};

// RASTERIZER
enum JSH_FILL_MODE
{
	JSH_FILL_WIREFRAME = 2,
	JSH_FILL_SOLID = 3
};
enum JSH_CULL_MODE
{
	JSH_CULL_NONE = 1,
	JSH_CULL_FRONT = 2,
	JSH_CULL_BACK = 3
};
struct JSH_RASTERIZER_DESC
{
	JSH_FILL_MODE FillMode;
	JSH_CULL_MODE CullMode;
	bool FrontCounterClockwise;
	int32 DepthBias;
	float DepthBiasClamp;
	float SlopeScaledDepthBias;
	bool DepthClipEnable;
	bool ScissorEnable;
	bool MultisampleEnable;
	bool AntialiasedLineEnable;
};

// RENDER TARGET
enum JSH_RTV_DIMENSION
{
	JSH_RTV_DIMENSION_UNKNOWN = 0,
	JSH_RTV_DIMENSION_BUFFER = 1,
	JSH_RTV_DIMENSION_TEXTURE1D = 2,
	JSH_RTV_DIMENSION_TEXTURE1DARRAY = 3,
	JSH_RTV_DIMENSION_TEXTURE2D = 4,
	JSH_RTV_DIMENSION_TEXTURE2DARRAY = 5,
	JSH_RTV_DIMENSION_TEXTURE2DMS = 6,
	JSH_RTV_DIMENSION_TEXTURE2DMSARRAY = 7,
	JSH_RTV_DIMENSION_TEXTURE3D = 8
};
struct JSH_TEX2D_RTV
{
	uint32 MipSlice;
};
struct JSH_RENDER_TARGET_VIEW_DESC
{
	JSH_FORMAT Format;
	JSH_RTV_DIMENSION ViewDimension;
	JSH_TEX2D_RTV Texture2D;
};
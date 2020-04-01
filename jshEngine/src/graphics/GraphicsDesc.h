#pragma once

#include "..//common.h"

enum JSH_GRAPHICS_API : uint8 {
	JSH_GRAPHCS_API_NULL, 
	JSH_GRAPHCS_API_DIRECTX11
};

enum JSH_GRAPHICS_PRIMITIVE : uint8 {
	JSH_GRAPHICS_PRIMITIVE_INVALID,
	JSH_GRAPHICS_PRIMITIVE_VERTEX_BUFFER,
	JSH_GRAPHICS_PRIMITIVE_INDEX_BUFFER,
	JSH_GRAPHICS_PRIMITIVE_CONSTANT_BUFFER,
	JSH_GRAPHICS_PRIMITIVE_VERTEX_SHADER,
	JSH_GRAPHICS_PRIMITIVE_PIXEL_SHADER,
	JSH_GRAPHICS_PRIMITIVE_GEOMETRY_SHADER,
	JSH_GRAPHICS_PRIMITIVE_INPUT_LAYOUT,
	JSH_GRAPHICS_PRIMITIVE_FRAME_BUFFER,
	JSH_GRAPHICS_PRIMITIVE_TEXTURE,
};

enum JSH_TOPOLOGY {

	JSH_TOPOLOGY_TRIANGLES,
	JSH_TOPOLOGY_TRIANGLE_STRIP,
	JSH_TOPOLOGY_LINES,
	JSH_TOPOLOGY_LINE_STRIP,
	JSH_TOPOLOGY_POINTS

};

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


struct JSH_INPUT_ELEMENT_DESC {
	const char* semanticName;
	uint32 semanticIndex;
	JSH_FORMAT format;
	uint32 inputSlot;
	bool perVertexData;
	uint32 alignedByteOffset;
	uint32 instanceDataStepRate;
};
#ifndef JSH_COMMON
#define JSH_COMMON

#define JSH_GFX_MAX_LIGHTS 120

#define JSH_GFX_SLOT_CBUFFER_CAMERA		b0
#define JSH_GFX_SLOT_CBUFFER_LIGHTS		b1
#define JSH_GFX_SLOT_CBUFFER_INSTANCE	b2
#define JSH_GFX_SLOT_CBUFFER_MATERIAL	b3

#define JSH_GFX_SLOT_TEXTURE2D_DIFFUSE	t0
#define JSH_GFX_SLOT_TEXTURE2D_NORMAL	t1
#define JSH_GFX_SLOT_TEXTURE2D_SPECULAR	t2

typedef int int32;
typedef min16int int16;
typedef unsigned int uint32;
typedef min16uint uint16;

struct Camera {
	matrix vm;
	matrix pm;

	float4 position;
	float4 fasgargvcfcfvaga;
	float4 fasgargcfcfvaga;
	float4 fasgarcfcfvaga;
};

struct Light {
	float4 position;
	float quadraticAtt;
	float constantAtt;
	float intensity;
	unsigned int type;
	float4 color;
};

struct Material {
	float shininess;
	float specularIntensity;
};

struct Instance {
	matrix tm;
};

#endif
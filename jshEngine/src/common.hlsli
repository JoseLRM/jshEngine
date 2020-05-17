#ifndef JSH_COMMON
#define JSH_COMMON

#define JSH_GFX_MAX_LIGHTS 2u

#define JSH_GFX_SLOT_CBUFFER_CAMERA		b0
#define JSH_GFX_SLOT_CBUFFER_LIGHTS		b1
#define JSH_GFX_SLOT_CBUFFER_FREE0	    b2

#define JSH_GFX_SLOT_TEXTURE2D_FREE0	t0
#define JSH_GFX_SLOT_TEXTURE2D_FREE1	t1
#define JSH_GFX_SLOT_TEXTURE2D_FREE2	t2
#define JSH_GFX_SLOT_TEXTURE2D_FREE3	t3
#define JSH_GFX_SLOT_TEXTURE2D_FREE4	t4
#define JSH_GFX_SLOT_TEXTURE2D_FREE5	t5
#define JSH_GFX_SLOT_TEXTURE2D_FREE6	t6
#define JSH_GFX_SLOT_TEXTURE2D_FREE7	t7
#define JSH_GFX_SLOT_TEXTURE2D_FREE8	t8
#define JSH_GFX_SLOT_TEXTURE2D_FREE9	t9
#define JSH_GFX_SLOT_TEXTURE2D_FREE10	t10
#define JSH_GFX_SLOT_TEXTURE2D_FREE11	t11
#define JSH_GFX_SLOT_TEXTURE2D_FREE12	t12
#define JSH_GFX_SLOT_TEXTURE2D_FREE13	t13
#define JSH_GFX_SLOT_TEXTURE2D_FREE14	t14
#define JSH_GFX_SLOT_TEXTURE2D_FREE15	t15

#define JSH_GFX_SLOT_SAMPLER_FREE0		s0
#define JSH_GFX_SLOT_SAMPLER_FREE1		s1
#define JSH_GFX_SLOT_SAMPLER_FREE2		s2
#define JSH_GFX_SLOT_SAMPLER_FREE3		s3
#define JSH_GFX_SLOT_SAMPLER_FREE4		s4
#define JSH_GFX_SLOT_SAMPLER_FREE5		s5
#define JSH_GFX_SLOT_SAMPLER_FREE6		s6
#define JSH_GFX_SLOT_SAMPLER_FREE7		s7
#define JSH_GFX_SLOT_SAMPLER_FREE8		s8
#define JSH_GFX_SLOT_SAMPLER_FREE9		s9
#define JSH_GFX_SLOT_SAMPLER_FREE10		s10
#define JSH_GFX_SLOT_SAMPLER_FREE11		s11
#define JSH_GFX_SLOT_SAMPLER_FREE12		s12
#define JSH_GFX_SLOT_SAMPLER_FREE13		s13
#define JSH_GFX_SLOT_SAMPLER_FREE14		s14
#define JSH_GFX_SLOT_SAMPLER_FREE15		s15

typedef int int32;
typedef min16int int16;
typedef unsigned int uint32;
typedef min16uint uint16;

struct Camera {
	matrix vm;

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
	float4 direction;
};

#endif
#include "common.hlsli"

Texture2D tex0 : register(t0);
Texture2D tex1 : register(t1);
Texture2D tex2 : register(t2);
Texture2D tex3 : register(t3);
Texture2D tex4 : register(t4);
Texture2D tex5 : register(t5);
Texture2D tex6 : register(t6);
Texture2D tex7 : register(t7);
Texture2D tex8 : register(t8);
Texture2D tex9 : register(t9);

SamplerState sam0 : register(s0);
SamplerState sam1 : register(s1);
SamplerState sam2 : register(s2);
SamplerState sam3 : register(s3);
SamplerState sam4 : register(s4);
SamplerState sam5 : register(s5);
SamplerState sam6 : register(s6);
SamplerState sam7 : register(s7);
SamplerState sam8 : register(s8);
SamplerState sam9 : register(s9);

float4 main(float4 color : FragColor, float2 texCoord : FragTexCoord, uint16 texID : FragTextureID) : SV_TARGET
{
	switch (texID) {
	case 1:
		color *= tex0.Sample(sam0, texCoord);
		break;
	case 2:
		color *= tex1.Sample(sam1, texCoord);
		break;
	case 3:
		color *= tex2.Sample(sam2, texCoord);
		break;
	case 4:
		color *= tex3.Sample(sam3, texCoord);
		break;
	case 5:
		color *= tex4.Sample(sam4, texCoord);
		break;
	case 6:
		color *= tex5.Sample(sam5, texCoord);
		break;
	case 7:
		color *= tex6.Sample(sam6, texCoord);
		break;
	case 8:
		color *= tex7.Sample(sam7, texCoord);
		break;
	case 9:
		color *= tex8.Sample(sam8, texCoord);
		break;
	case 10:
		color *= tex9.Sample(sam9, texCoord);
		break;
	};

	if (color.w < 0.05f) discard;

	return color;
}
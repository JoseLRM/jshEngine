#include "common.hlsli"

Texture2D tex0 : register(t0);

SamplerState sam0 : register(s0);

float4 main(float4 color : FragColor, float2 texCoord : FragTexCoord, uint16 texID : FragTextureID) : SV_TARGET
{
	switch (texID) {
	case 1:
		color *= tex0.Sample(sam0, texCoord);
		break;
	};

	if (color.w < 0.05f) discard;

	return color;
}
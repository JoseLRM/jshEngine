#include "common.hlsli"

cbuffer buffer : register(b0)
{
	float offset;
	int32 count;
	bool horizontal;

	float _aux_dangiusbg;
};

cbuffer buffer2 : register(b1)
{
	float coefficients[16];
};

Texture2D tex : register(t0);
SamplerState sam : register(s0);

float4 main(float2 texCoord : TexCoord) : SV_TARGET
{
	float xOffset = 0.f;
	float yOffset = 0.f;
	if (horizontal) xOffset = offset;
	else yOffset = offset;

	float4 color = float4(0.f, 0.f, 0.f, 0.f);
	for (int32 i = -count + 1; i < count; ++i) {
		float2 coords = float2(texCoord.x + xOffset * i, texCoord.y + yOffset * i);
		color += tex.Sample(sam, coords) * coefficients[abs(i)];
	}

	return color;
}
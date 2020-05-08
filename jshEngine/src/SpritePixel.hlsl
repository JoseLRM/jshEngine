#include "common.hlsli"

Texture2D tex : register(t0);
SamplerState sam : register(s0);

float4 main(float4 color : Color, float2 texCoord : TexCoord, bool t : Texture) : SV_TARGET
{
    if (t)
    {
        color *= tex.Sample(sam, texCoord);
    }
        if (color.w < 0.05f)
            discard;
        return color;
    }
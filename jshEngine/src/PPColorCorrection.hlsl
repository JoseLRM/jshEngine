cbuffer buffer : register(b0)
{
	float contrast;
	float brightness;
}

Texture2D tex : register(t0);
SamplerState sam : register(s0);

float4 main(float2 texCoord: TexCoord) : SV_TARGET
{
	float4 color = tex.Sample(sam, texCoord);

	// constrast
	color.rgb = (color.rgb - 0.5f) * (1.f + contrast) + 0.5f;

	// brightness
	color.rgb *= brightness;

	return float4(color.rgb, 1.0f);
}
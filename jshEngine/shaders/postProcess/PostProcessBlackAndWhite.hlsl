cbuffer c {
	float intensity;
};

Texture2D tex;
SamplerState samp;

float4 main(float2 texCoord : TexCoord) : SV_Target
{
	float4 colors = tex.Sample(samp, texCoord);
	float i = (colors.x + colors.y + colors.z) / 3.f;

	//return (colors * (1.f - intensity) + (colors * i) * intensity) / 2.f;
	//return float4(i,i,i,1.f);
	return colors;
}
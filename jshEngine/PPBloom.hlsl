Texture2D tex : register(t0);
SamplerState sam : register(s0);

cbuffer buffer : register(b0) {
	float intensity;
};

float4 main(float2 texCoord : TexCoord) : SV_Target
{
	float4 color = tex.Sample(sam, texCoord);
	float sum = color.x * 0.2126 + color.y * 0.7152 + color.z * 0.0722;

	if (sum < intensity) {
		discard;
	}
	
	return float4(color.xyz, (sum - intensity) / (1.f - intensity));
	//return float4(color.xyz, 1.f);
}
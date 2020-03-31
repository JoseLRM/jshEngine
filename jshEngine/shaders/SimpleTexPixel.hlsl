cbuffer light {

	float4 lightPos;
	float quadraticAtt;
	float linearAtt;
	float constantAtt;
	float intensity;
	float4 lightColor;

};

Texture2D tex;
SamplerState samp;

float4 main(float3 normal : FragNormal, float3 fragPos : FragPosition, float2 texCoord : FragTexCoord) : SV_TARGET
{
	float3 toLight = lightPos.xyz - fragPos;
	float distance = length(toLight);
	float diffuse = dot(normalize(toLight), normal);

	diffuse = intensity * diffuse * (1.f / (quadraticAtt * distance * distance + linearAtt * distance + constantAtt));
	diffuse = max(0.2f, diffuse);

	return diffuse * tex.Sample(samp, texCoord);
}
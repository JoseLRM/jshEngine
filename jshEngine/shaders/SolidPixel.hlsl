cbuffer light {

	float4 lightPos;
	float quadraticAtt;
	float linearAtt;
	float constantAtt;
	float intensity;
	float4 lightColor;

};

float4 main(float3 normal : FragNormal, float3 fragPos : FragPosition) : SV_TARGET
{
	float3 toLight = lightPos.xyz - fragPos;
	float distance = length(toLight);
	float diffuse = dot(normalize(toLight), normal);

	diffuse = intensity * diffuse * (1.f / (quadraticAtt * distance * distance + linearAtt * distance + constantAtt));
	diffuse = max(0.2f, diffuse);

	return diffuse * float4(0.0f, 1.0f, 0.0f, 1.0f);
}
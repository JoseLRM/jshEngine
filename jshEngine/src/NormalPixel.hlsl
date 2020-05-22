#include "Light.hlsli"

cbuffer material : register(JSH_GFX_SLOT_CBUFFER_FREE0)
{
    float shininess;
    float specularIntensity;
    
    bool diffuseEnabled;
    bool normalEnabled;
    bool specularEnabled;
};

struct PS_IN
{
    float3 position : FragPosition;
    float3x3 tanBiNor : FragTanBiNor;
    float2 texCoord : FragTexCoord;
};

Texture2D tex[3] : register(t0);
SamplerState diffuseSam : register(s0);
SamplerState sam : register(s1);

[earlydepthstencil]
float4 main(PS_IN input) : SV_TARGET
{
	float3 normal;
    
    if (normalEnabled)
    {
        normal = tex[1].Sample(sam, input.texCoord).xyz;
		normal.x = (normal.x * 2.f) - 1.f;
		normal.y = (normal.y * 2.f) - 1.f;
		normal.z = (normal.z * 2.f) - 1.f;

        normal = normalize(mul(normal, input.tanBiNor));
    }
	else {
        normal = normalize(input.tanBiNor[2]);
    }

    float specI = specularIntensity;
    float shiny = shininess;
    if (specularEnabled)
    {
        float4 s = tex[2].Sample(sam, input.texCoord);
		specI *= s.x;
		shiny *= s.w;
	}

    float3 lightColor = LoadLightColor(input.position, normal, specI, shiny);
	
    if (diffuseEnabled)
    {
        return float4(lightColor, 1.f) * tex[0].Sample(diffuseSam, input.texCoord);
    }
	else {
		return float4(lightColor, 1.f);
	}
}

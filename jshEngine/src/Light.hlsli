#include "common.hlsli"
cbuffer lig : register(JSH_GFX_SLOT_CBUFFER_LIGHTS) 
{ 
	Light light[JSH_GFX_MAX_LIGHTS];
}


inline float3 LoadLightColor(float3 position, float3 normal, float3 toCamera, float sIntensity, float sShiny)
{
	float3 count = 0.f;
	uint32 i = 0u;

	float3 reflectionToCamera = normalize(reflect(toCamera, normal));

	while (i < JSH_GFX_MAX_LIGHTS) {
		if (light[i].type == 0) break;
		
		float3 toLight = light[i].position.xyz - position;
		float distance = length(toLight);
		toLight = normalize(toLight);

		// diffuse
		float intensity = max(dot(toLight, normal), 0.f);

		// specular
		intensity += sIntensity * pow(max(dot(toLight, reflectionToCamera), 0.f), sShiny);

		// attenuation
		intensity = light[i].intensity * intensity * (1.f / (light[i].quadraticAtt * distance * distance + light[i].constantAtt));
		count += light[i].color * intensity;
		i++;
	}

	// ambient
    count = max(count, 0.2f);

	return count;
}
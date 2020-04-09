#include "common.hlsli"

struct Light {
	float4 position;
	float quadraticAtt;
	float constantAtt;
	float intensity;
	unsigned int type;
	float4 color;
};

cbuffer lights : register(b0) {
	Light light[JSH_GFX_MAX_LIGHTS];
};

/*
float3 LoadDiffuseColor(float3 position, float3 normal)
{
	float3 count = 0.f;
	unsigned int i = 0u;
	while (i < JSH_GFX_MAX_LIGHTS) {
		if (light[i].type == 0) break;
		float3 toLight = light[i].position.xyz - position;
		float distance = length(toLight);
		float diffuse = max(dot(normalize(toLight), normal), 0.f);

		diffuse = light[i].intensity * diffuse * (1.f / (light[i].quadraticAtt * distance * distance + light[i].constantAtt));
		count += light[i].color * diffuse;
		i++;
	}
	count /= i;
	if (count.x < 0.2f) count.x = 0.2f;
	if (count.y < 0.2f) count.y = 0.2f;
	if (count.z < 0.2f) count.z = 0.2f;
	return count;
}
float3 LoadSpecularColor(float3 position, float3 normal, float3 toCamera, float sIntensity, float sShiny)
{
	float3 count = 0.f;
	uint32 i = 0u;

	float3 reflectionToCamera = normalize(reflect(toCamera, normal));

	while (i < JSH_GFX_MAX_LIGHTS) {
		if (light[i].type == 0) break;

		float3 toLight = light[i].position - position;
		float distance = length(toLight);
		float spec = max(dot(normalize(toLight), reflectionToCamera), 0.f);

		count += light[i].color * (sIntensity * pow(spec, sShiny));
		i++;
	}
	count /= i;
	return count;
}

float3 LoadLightColor(float3 position, float3 normal, float3 toCamera, float sIntensity, float sShiny)
{
	return LoadDiffuseColor(position, normal) + LoadSpecularColor(position, normal, toCamera, sIntensity, sShiny);
}
*/
float3 LoadLightColor(float3 position, float3 normal, float3 toCamera, float sIntensity, float sShiny)
{
	float3 count = 0.f;
	uint32 i = 0u;

	float3 reflectionToCamera = normalize(reflect(toCamera, normal));

	while (i < JSH_GFX_MAX_LIGHTS) {
		if (light[i].type == 0) break;
		
		float3 toLight = light[i].position - position;
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
	count /= i;

	// ambient
	if (count.x < 0.2f) count.x = 0.2f;
	if (count.y < 0.2f) count.y = 0.2f;
	if (count.z < 0.2f) count.z = 0.2f;

	return count;
}
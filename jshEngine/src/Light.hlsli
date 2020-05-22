#include "common.hlsli"
cbuffer lig : register(JSH_GFX_SLOT_CBUFFER_LIGHTS) 
{ 
	Light light[JSH_GFX_MAX_LIGHTS];
    float4 ambient;
}

float3 GetLightColor(uint32 lightID, float3 position, float3 normal, float3 reflectionToCamera, float shininess, float specIntensity)
{
    float3 toLight;
    float distance;
    
    // to spot light
    float range = 0.f;
    float diffuseRange = 0.f;
    float finalIntensity = 1.f;
    
    switch (light[lightID].type)
    {
        case 0:
            return float3(0.f, 0.f, 0.f);
        case 1: // Point light
            toLight = light[lightID].position.xyz - position;
            distance = length(toLight);
            toLight = normalize(toLight);
            break;
        case 2: // direction light
            toLight = light[lightID].direction.xyz;
            toLight = normalize(toLight);
            distance = 1.f;
            break;
        default: // spot light
            toLight = light[lightID].position.xyz - position;
                
            range = dot(normalize(toLight), light[lightID].direction.xyz);
            diffuseRange = light[lightID].spotRange * 0.95f;
            if (range > light[lightID].spotRange)
            {
                distance = length(toLight);
                if (range > diffuseRange)
                {
                    finalIntensity = ((range - light[lightID].spotRange) / (range - diffuseRange));
                }
            }
            else
            {
                distance = -1.f;
            }
                
            toLight = normalize(toLight);
            break;
    }
   
    if (distance <= 0.f)
        return float3(0.f, 0.f, 0.f);
    
    // diffuse
    float intensity = max(dot(toLight, normal), 0.f);

    // specular
    intensity += specIntensity * pow(max(dot(toLight, reflectionToCamera), 0.f), shininess);

    // attenuation
    float attenuation = 1.f - smoothstep(light[lightID].range * light[lightID].smoothness, light[lightID].range, distance);
    intensity = light[lightID].intensity * intensity * attenuation;
    
    return light[lightID].color * intensity;
}


float3 LoadLightColor(float3 position, float3 normal, float sIntensity, float sShiny)
{
	float3 count = 0.f;

    float3 reflectionToCamera = normalize(reflect(normalize(-position), normal));

    [unrool]
    for (uint32 i = 0u; i < JSH_GFX_MAX_LIGHTS; ++i)
    {
        count += GetLightColor(i, position, normal, reflectionToCamera, sShiny, sIntensity);
    }

	// ambient
    count = max(count, ambient.xyz);

	return count;
}
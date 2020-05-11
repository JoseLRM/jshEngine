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
    
     // to spot light
    float range = 0.f;
    float diffuseRange = 0.f;

	while (i < JSH_GFX_MAX_LIGHTS) {
		if (light[i].type == 0) break;
		
        float3 toLight;
        float distance;
        float finalIntensity = 1.f;
        
        switch (light[i].type)
        {
            case 1: // point light
                toLight = light[i].position.xyz - position;
                distance = length(toLight);
                toLight = normalize(toLight);
                break;
            case 2: // direction light
                toLight = light[i].direction.xyz;
                toLight = normalize(toLight);
                distance = 1.f;
                break;
            default: // spot light
                toLight = light[i].position.xyz - position;
                
                range = dot(normalize(toLight), light[i].direction.xyz);
                diffuseRange = light[i].direction.w * 0.9f;
                if (range > light[i].direction.w)
                {
                    distance = length(toLight);
                    if (range > diffuseRange)
                    {
                        finalIntensity = ((range - light[i].direction.w) / (range - diffuseRange));
                    }
                }
                else
                {
                    distance = -1.f;
                }
                
                toLight = normalize(toLight);
                break;
        }
        
        if (distance > 0.f)
        {
		// diffuse
            float intensity = max(dot(toLight, normal), 0.f);

		// specular
            intensity += sIntensity * pow(max(dot(toLight, reflectionToCamera), 0.f), sShiny);

		// attenuation
            intensity = light[i].intensity * intensity * (1.f / (light[i].quadraticAtt * distance * distance + light[i].constantAtt));
            count += light[i].color * intensity * finalIntensity;
        }
		i++;
	}

	// ambient
    count = max(count, 0.2f);

	return count;
}
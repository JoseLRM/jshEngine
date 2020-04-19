#include "Camera.hlsli"
#include "Instance.hlsli"

float4 main(float3 pos : Position) : SV_POSITION
{
	return mul(mul(mul(float4(pos, 1.f), instance.tm), camera.vm), camera.pm);
}
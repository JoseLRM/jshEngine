#include "Camera.hlsli"

struct VS_OUT {
	float4 color : FragColor;
	float2 texCoord : FragTexCoord;
	uint16 texID : FragTextureID;
	float4 position : SV_Position;
};

VS_OUT main( float2 pos : Position, matrix tm : TM, float4 color : Color, float4 texCoord : TexCoord, uint16 texID : TextureID )
{
	VS_OUT vsout;
	vsout.color = color;
	vsout.position = mul(mul(mul(float4(pos, 0.f, 1.f), tm), camera.vm), camera.pm);
	vsout.texCoord = pos + 0.5f;
	vsout.texCoord = float2(texCoord.x, texCoord.y) + (float2(vsout.texCoord.x, 1.f - vsout.texCoord.y) *  texCoord.zw);
	vsout.texID = texID;
	return vsout;
}
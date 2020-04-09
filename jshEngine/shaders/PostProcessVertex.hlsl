
struct VS_OUT {

	float2 texCoord : PTexCoord;
	float4 position : SV_Position;

};

VS_OUT main(float2 position : Position)
{
	VS_OUT vsOut;
	vsOut.texCoord = position;
	vsOut.position = float4(position.x, position.y, 0.f, 1.f);
	return vsOut;
}

struct VS_OUT {

	float2 texCoord : TexCoord;
	float4 position : SV_Position;

};

VS_OUT main(float2 position : Position)
{
	VS_OUT vsOut;
	vsOut.texCoord.x = (position.x + 1.f) / 2.f;
	vsOut.texCoord.y = -(position.y - 1.f) / 2.f;
	vsOut.position = float4(position.x, position.y, 0.f, 1.f);
	return vsOut;
}
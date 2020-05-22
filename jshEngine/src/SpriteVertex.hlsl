
struct VS_IN
{
    float2 pos : Position;
    
    matrix tm : TM;
    float4 texCoord : TexCoord;
    float4 color : Color;
    bool t : Texture;
};

struct VS_OUT {
	float4 color : Color;
	float2 texCoord : TexCoord;
    bool t : Texture;
	float4 position : SV_Position;
};

VS_OUT main(VS_IN input)
{
	VS_OUT vsout;
	vsout.color = input.color;
    vsout.position = mul(float4(input.pos, 0.f, 1.f), input.tm);
    vsout.texCoord = input.pos + 0.5f;
    vsout.texCoord = float2(input.texCoord.x, input.texCoord.y) + (float2(vsout.texCoord.x, 1.f - vsout.texCoord.y) * input.texCoord.zw);
    vsout.t = input.t;
    
	return vsout;
}
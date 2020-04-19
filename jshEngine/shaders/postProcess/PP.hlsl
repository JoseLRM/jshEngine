Texture2D tex;
SamplerState samp;

float4 main(float2 texCoord : TexCoord) : SV_Target
{
	return tex.Sample(samp, texCoord);
}
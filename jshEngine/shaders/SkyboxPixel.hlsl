texture2D tex;
SamplerState samp;

float4 main(float2 tc : FragTexCoord) : SV_TARGET
{
	return tex.Sample(samp, tc);
}
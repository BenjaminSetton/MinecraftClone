
Texture2D quadTex;
SamplerState sampleClamp;

struct PixelIn
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD1;
};

float4 main(PixelIn input) : SV_TARGET
{
	return quadTex.Sample(sampleClamp, input.uv);
}

Texture2D quadTex;
SamplerState sampleClamp;

struct PixelIn
{

};

float4 main(PixelIn input) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
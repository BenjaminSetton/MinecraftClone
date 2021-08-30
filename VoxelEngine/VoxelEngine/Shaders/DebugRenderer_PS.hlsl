
struct PixelIn
{
    float4 pos : SV_POSITION;
    float4 color : TEXCOORD1;
};


float4 main(PixelIn input) : SV_TARGET
{
	return input.color;
}
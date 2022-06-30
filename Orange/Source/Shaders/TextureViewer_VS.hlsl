
struct VertexInput
{
    float3 pos : POSITION0;
    float2 uv : TEXCOORD0;
};


struct PixelInput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

// Pass-through vertex shader
PixelInput main(VertexInput input)
{
    PixelInput output;
    output.pos = float4(input.pos.xyz, 1.0f);
    output.uv = input.uv;
	return output;
}
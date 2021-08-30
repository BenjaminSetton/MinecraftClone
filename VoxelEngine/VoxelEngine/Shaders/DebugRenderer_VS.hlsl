
cbuffer WVP
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
};

struct VertexIn
{
    float3 pos : POSITION0;
    float4 color : TEXCOORD0;
};

struct VertexOut
{
    float4 pos : SV_POSITION;
    float4 color : TEXCOORD1;
};

VertexOut main(VertexIn input)
{
    VertexOut output;
    
    float4 inPos = float4(input.pos, 1.0f);
    output.pos = mul(inPos, world);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    
    output.color = input.color;
    
	return output;
}
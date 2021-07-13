
cbuffer WVP
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

struct VertexIn
{
    float3 pos : POSITION0;
    float3 norm : NORMAL0;
    float2 uv : TEXCOORD0;
};

struct VertexOut
{
    float4 pos : SV_Position;
    float3 norm : NORMAL0;
    float2 uv : TEXCOORD0;
};


VertexOut main(VertexIn input)
{
    VertexOut output;
    float4 outPos = float4(input.pos, 1.0f);
    
    output.pos = mul(outPos, worldMatrix);
    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);
    
    output.norm = mul(input.norm, (float3x3) worldMatrix);
    output.uv = input.uv;
    
	return output;
}
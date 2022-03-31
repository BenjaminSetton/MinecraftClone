
#include "BlockUVs.hlsli"

cbuffer WVP : register(b1)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4x4 lightViewMatrix;
    float4x4 lightProjectionMatrix;
};

struct VertexIn
{
    // per-vertex
    float3 lpos : POSITION0;
    uint vertexID : ID0;
    
    // per-instance
    float3 wpos : WORLDPOS0;
    uint blockType : BLOCKTYPE0;
    uint blockFaces : BLOCKFACES0;
};


struct VertexOut
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float4 lightPos : TEXCOORD1;
    uint blockFaces : BLOCKFACES1;
    uint vertexID : ID1;
    float3 worldPos : POS0;
};


VertexOut main(VertexIn input)
{
    VertexOut output;
    
    float4 outPos = float4(input.wpos + input.lpos, 1.0f);
    
    output.worldPos = outPos.xyz;
    
    output.pos = mul(outPos, worldMatrix);
    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);
    
    output.lightPos = mul(outPos, worldMatrix);
    output.lightPos = mul(output.lightPos, lightViewMatrix);
    output.lightPos = mul(output.lightPos, lightProjectionMatrix);
    
    output.vertexID = input.vertexID;
    
    // Use the appropriate UVs
    output.uv = blockUVs[input.blockType][input.vertexID].xy;
    
    output.blockFaces = input.blockFaces;
    
	return output;
}
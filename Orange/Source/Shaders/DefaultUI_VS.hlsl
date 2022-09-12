
cbuffer ProjectionMatrix
{
    float4x4 projMatrix;
};

struct VertexIn
{
    float2 coordinate : COORDINATE0;
    float2 size : SIZE0;
    float2 position : POSITION0;
    float4 color : COLOR0;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR1;
};

VertexOut main(VertexIn input)
{
    VertexOut output;
    
    output.uv = input.coordinate;
    float2 extendedPos = input.position + (input.coordinate * input.size);
    float4 transformedPos = mul(float4(extendedPos.x, extendedPos.y, 0.0, 1.0), projMatrix);
    output.position = transformedPos;
    output.position.z = 0.0;
    output.color = input.color;
    
    return output;
}

cbuffer ProjectionMatrix
{
    float4x4 projMatrix;
};

struct VertexIn
{
    float2 coordinate : COORDINATE0;
    float2 size : SIZE0;
    float2 position : POSITION0;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VertexOut main(VertexIn input)
{
    VertexOut output;
    
    output.uv = input.coordinate;
    float2 extendedPos = input.coordinate * input.size + input.position;
    float4 transformedPos = mul(float4(extendedPos.x, extendedPos.y, 0.0, 1.0), projMatrix);
    output.position = transformedPos;
    output.position.z = 0.0;
    
    return output;
}

Texture2D blockTexture;
SamplerState samp;

cbuffer LightBuffer
{
    float3 lightDir;
    float4 lightCol;
    float padding;
};

struct VertexOut
{
    float4 pos : SV_Position;
    float3 norm : NORMAL0;
    float2 uv : TEXCOORD0;
};

float4 main(VertexOut input) : SV_TARGET
{
    float4 ambientLight = { 0.15f, 0.15f, 0.15f, 1.0f };
    float4 color = ambientLight;
    
    // Sample the block texture
    float4 diffuse = { 0.0f, 0.6f, 0.0f, 1.0f }; //= blockTexture.Sample(samp, input.uv);
    
    float4 lightIntensity = saturate(dot(normalize(-lightDir), input.norm));
    
    // Calculate the final color
    color += lightCol * lightIntensity;
    color *= diffuse;
    
    return color;
}

Texture2D blockTexture;
Texture2D shadowMapTexture;
SamplerState sampWrap;
SamplerState sampClamp;

cbuffer LightBuffer
{
    float3 lightDir;
    float4 lightCol;
    float padding;
};

struct VertexOut
{
    float4 pos : SV_Position;
    float3 norm : NORMAL1;
    float2 uv : TEXCOORD1;
};

float4 main(VertexOut input) : SV_TARGET
{
    float4 ambientLight = { 0.15f, 0.15f, 0.15f, 1.0f };
    float4 color = ambientLight;
    
    // Sample the block texture
    float4 diffuse = blockTexture.Sample(sampWrap, input.uv);
    
    float4 lightIntensity = saturate(dot(normalize(-lightDir), input.norm));
    
    // Calculate the final color
    color += lightCol * lightIntensity;
    color *= diffuse;
    
    return color;
}
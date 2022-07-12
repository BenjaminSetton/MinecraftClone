
Texture2D<float> charTexture;
SamplerState sampClamp;

struct PixelIn
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(PixelIn input) : SV_TARGET
{
    float4 texData = float4(1.0f, 1.0f, 1.0f, charTexture.Sample(sampClamp, float2(input.uv.x, 1.0f - input.uv.y)));
    float3 charColor = float3(1.0, 1.0f, 1.0f);
    
    float4 finalColor = float4(charColor.xyz, 1.0f) * texData;
    return finalColor;
}
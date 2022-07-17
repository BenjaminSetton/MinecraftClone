
Texture2D<float> charTexture;
SamplerState sampClamp;

struct PixelIn
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 color : COLOR1;
};

float4 main(PixelIn input) : SV_TARGET
{
    float isCharVisible = charTexture.Sample(sampClamp, float2(input.uv.x, 1.0f - input.uv.y));
    
    float4 finalColor = float4(input.color, isCharVisible);
    return finalColor;
}
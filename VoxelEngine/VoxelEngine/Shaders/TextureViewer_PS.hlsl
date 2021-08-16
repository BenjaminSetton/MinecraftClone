
Texture2D tex;
SamplerState samp;

struct PixelInput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

float4 main(PixelInput input) : SV_TARGET
{
    float sampledCol = tex.Sample(samp, input.uv).x;
    float4 color = float4(sampledCol, sampledCol, sampledCol, 1.0f);
    return color;
}
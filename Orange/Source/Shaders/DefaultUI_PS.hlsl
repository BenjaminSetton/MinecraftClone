
Texture2D charTexture;
SamplerState sampClamp;

struct PixelIn
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR1;
};

float4 main(PixelIn input) : SV_TARGET
{
    //return float4(1.0, 1.0, 1.0, 1.0);
    float4 textureDiffuse = charTexture.Sample(sampClamp, float2(input.uv.x, 1.0f - input.uv.y));
    
    float4 finalColor = input.color;
    
    // Text rendering will always have an input.color.w of 1.0 so that we can render the transparency properly
    // The rest of the quad rendering (such as containers) will pass in a white texture?
    finalColor *= textureDiffuse;
    return finalColor;
}
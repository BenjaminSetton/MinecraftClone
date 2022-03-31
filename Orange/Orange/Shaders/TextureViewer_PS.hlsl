
Texture2D tex;
SamplerState samp;

struct PixelInput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

float4 main(PixelInput input) : SV_TARGET
{
    /*
        depthSample = 2.0 * depthSample - 1.0;
    float zLinear = 2.0 * zNear * zFar / (zFar + zNear - depthSample * (zFar - zNear));
    return zLinear;
    
    float zNear = 0.5;    // TODO: Replace by the zNear of your perspective projection
    float zFar  = 2000.0; // TODO: Replace by the zFar  of your perspective projection
    float depth = texture2D(colorImage, uv).x;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
*/
    float zNear = 0.1f;
    float zFar = 1000.0f;
    float depth = tex.Sample(samp, input.uv).x;
    float zLinear = (2.0f * zNear) / (zFar + zNear - depth * (zFar - zNear));
    return float4(zLinear, zLinear, zLinear, 1.0f);
}
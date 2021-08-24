
Texture2D blockTexture;
Texture2D shadowMapTexture;
SamplerState sampWrap;
SamplerState sampClamp;

cbuffer LightBuffer
{
    float3 lightDir[2];
    float4 lightCol[2];
    float lightAmbient[2];
};

struct VertexOut
{
    float4 pos : SV_Position;
    float3 norm : NORMAL0;
    float2 uv : TEXCOORD0;
    float4 lightPos : TEXCOORD1;
};

float4 main(VertexOut input) : SV_TARGET
{
    float4 color = { 0.0f, 0.0f, 0.0f, 1.0f };
    
    float4 shadowFactor = { 0.15f, 0.15f, 0.15f, 1.0f };
    
    float bias = 0.001f;
    
    // Sample the block texture
    float4 diffuse = blockTexture.Sample(sampWrap, input.uv);
    
    input.lightPos.xyz /= input.lightPos.w; // Re-homogenize the coordinates
    float2 shadowUV;
    shadowUV.x = input.lightPos.x * 0.5f + 0.5f;
    shadowUV.y = -input.lightPos.y * 0.5f + 0.5f;
    
    if (saturate(shadowUV.x) == shadowUV.x && saturate(shadowUV.y) == shadowUV.y) // if inside UV domain
    {
        float mapDepth = shadowMapTexture.Sample(sampClamp, shadowUV).x;
        float pointDepth = input.lightPos.z - bias;
        
        
        if (mapDepth < pointDepth) return shadowFactor * diffuse; // Pixel is shadowed
        // else pixel is not shadowed and we calculate the pixel color
    }
    
    // Calculate the light's intensity
    [unroll]
    for (int i = 0; i < 2; i++)
    {
        float4 lightIntensity = saturate(dot(normalize(-lightDir[i]), input.norm));
        //lightIntensity = max(0.45f, lightIntensity);
    
        // Calculate the final color
        color += lightCol[i] * lightIntensity;
        
    }
    
    color *= diffuse;
    
    return color;
}
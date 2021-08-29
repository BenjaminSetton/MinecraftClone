
Texture2D blockTexture;
Texture2D shadowMapTexture;
SamplerState sampWrap;
SamplerState sampClamp;

cbuffer LightBuffer
{
    float4 lightDir[2];
    float4 lightCol[2];
    float4 lightAmbient[2];
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
    
    float4 diffuseAmbient = { 0.10f, 0.10f, 0.10f, 1.0f };
    
    float bias = 0.001f;
    
    // Sample the block texture
    float4 diffuse = blockTexture.Sample(sampWrap, input.uv);
    
    float4 finalColor = diffuse * diffuseAmbient;
    
    input.lightPos.xyz /= input.lightPos.w; // Re-homogenize the coordinates
    float2 shadowUV;
    shadowUV.x = input.lightPos.x * 0.5f + 0.5f;
    shadowUV.y = -input.lightPos.y * 0.5f + 0.5f;
    
    if (saturate(shadowUV.x) == shadowUV.x && saturate(shadowUV.y) == shadowUV.y) // if inside UV domain
    {
        float mapDepth = shadowMapTexture.Sample(sampClamp, shadowUV).x;
        float pointDepth = input.lightPos.z - bias;
        
        
        if (mapDepth < pointDepth) return finalColor; // Pixel is shadowed
        // else pixel is not shadowed and we calculate the pixel color
    }
    
    
    // Calculate the light's intensity
    [unroll]
    for (int i = 0; i < 2; i++)
    {
        float4 lightIntensity = saturate(dot(normalize(-lightDir[i].xyz), input.norm));
    
        // Calculate the final color
        float4 lightContribution = lightCol[i] * lightIntensity + lightAmbient[i].x;
        finalColor += lightContribution;
        
    }
    
    finalColor *= diffuse;
    
    return finalColor;
}
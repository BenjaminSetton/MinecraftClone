
struct GeometryIn
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float4 lightPos : TEXCOORD1;
    uint blockFaces : BLOCKFACES1;
    uint vertexID : ID1;
};

struct GeometryOut
{
    float4 pos : SV_Position;
    float3 norm : NORMAL0;
    float2 uv : TEXCOORD0;
    float4 lightPos : TEXCOORD1;
};

[maxvertexcount(3)]
void main(triangle GeometryIn input[3], inout TriangleStream<GeometryOut> output)
{
    // Between 0-35
    uint faceIndex = input[0].vertexID / 6;
    //uint faceMask = 1 << faceIndex;
    uint renderFace = input[0].blockFaces & (1 << faceIndex);
    
    // Check if face should not be rendered. If not, return
    if (renderFace == 0) return;
    
    float3 ptOne = input[2].pos.xyz - input[0].pos.xyz;
    float3 ptTwo = input[1].pos.xyz - input[0].pos.xyz;
    
    // Creates flat shading, but since we're dealing with voxels
    // we don't need smooth shading anyway
    float3 triNorm = normalize(cross(ptTwo, ptOne));
    
	for (uint i = 0; i < 3; i++)
	{
        GeometryOut element;
        
        // Pass thru variables
		element.pos = input[i].pos;
        element.lightPos = input[i].lightPos;
        element.uv = input[i].uv;
        
        element.norm = triNorm;
        
		output.Append(element);
	}
}
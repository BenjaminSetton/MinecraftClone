
struct VertexIn
{
	float2 vertexPos	: POSITION0;
	float2 uv			: TEXCOORD0;
	float2 NDCPos		: NDCPOS0;
	float2 scale		: SCALE0;
};

struct PixelIn
{
	float4 pos	: SV_POSITION;
	float2 uv	: TEXCOORD1;
};


PixelIn main(VertexIn input)
{
	PixelIn output;

	// Scale and translate the NDC vertices, taking into account the window's aspect ratio to maintain quad's intended aspect ratio
	float2 finalPos = float2( input.vertexPos.x * input.scale.y, input.vertexPos.y * input.scale.x * input.scale.y );
	finalPos += input.NDCPos;
	
	// Copy the data to output struct and return it
	output.pos = float4(finalPos, 0.0f, 1.0f);
	output.uv = input.uv;

	return output;
}
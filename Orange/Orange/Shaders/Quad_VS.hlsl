
cbuffer ViewProjection
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};

struct VertexIn
{
	float3 pos : POSITION0;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;

	float4x4 transform : TRANSFORM;
};

struct PixelIn
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD1;
};


PixelIn main(VertexIn input)
{
	PixelIn output;

	output.pos = float4(input.pos, 1.0f);
	output.pos = mul(output.pos, input.transform);
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projectionMatrix);

	output.uv = input.uv;

	return output;
}

// If we need more blocks, we have to change this value
// I think the maximum is 4096 float4's, not sure about float2's
#define NUM_BLOCKS 4

// 2D array storing all the UV coordinates for each block type
cbuffer UV_COORDINATES
{
    float2 blockUVs[NUM_BLOCKS][36];
};
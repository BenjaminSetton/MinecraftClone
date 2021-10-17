#ifndef _BLOCK_UVS_H
#define _BLOCK_UVS_H

constexpr auto TEX_WIDTH = 1024;
constexpr auto TEX_HEIGHT = 512;

#include "Block.h"
#include <DirectXMath.h>

static DirectX::XMFLOAT2 dirt_uv[] =
{
	{}
};

static DirectX::XMFLOAT2 stone_uv[] =
{
	{}
};

static DirectX::XMFLOAT2 grass_uv[24] =
{
	{ 0.0f, 16.0f / TEX_HEIGHT },		// TLF(0)
	{ 16.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT },	// TRF(1)	// TOP
	{ 16.0f / TEX_WIDTH, 0.0f },			// TRB(2)
	{ 0.0f, 0.0f },				// TLB(3)

	{ 0.0f, 0.0625f },			// BLF(4)
	{ 0.03125f, 0.0625f },		// BRF(5)	// BOTTOM
	{ 16.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT }, // BRB(6)
	{ 0.0f, 16.0f / TEX_HEIGHT },		// BLB(7)

	{ 16.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT },// BLB(8)
	{ 32.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT }, // BLF(9)	// LEFT
	{ 32.0f / TEX_WIDTH, 0.0f },// TLF(10)
	{ 16.0f / TEX_WIDTH, 0.0f },// TLB(11)

	{ 32.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT }, // BRB(12)
	{ 16.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT }, // BRF(13)	// RIGHT
	{ 16.0f / TEX_WIDTH, 0.0f }, // TRF(14)
	{ 32.0f / TEX_WIDTH, 0.0f }, // TRB(15)

	{ 16.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT }, // BLF(16)
	{ 32.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT },// BRF(17)	// FRONT
	{ 32.0f / TEX_WIDTH, 0.0f },// TRF(18)
	{ 16.0f / TEX_WIDTH, 0.0f },// TLF(19)

	{ 32.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT },// BLB(20)
	{ 16.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT }, // BRB(21)	// BACK
	{ 16.0f / TEX_WIDTH, 0.0f },// TRB(22)
	{ 32.0f / TEX_WIDTH, 0.0f }// TLB(23)

};

static DirectX::XMFLOAT2 GetUVsForBlock(const BlockType& type, const uint32_t& index)
{
	switch (type)
	{
	case BlockType::Dirt:
	{
		break;
	}
	case BlockType::Stone:
	{
		break;
	}
	case BlockType::Grass:
	{
		return grass_uv[index];
	}
	case BlockType::Wood:
	{
		break;
	}
	default:
	{
		break;
	}
	}

	return { 0.0f, 0.0f };
}


#endif
#ifndef _BLOCK_H
#define _BLOCK_H

#include <DirectXMath.h>

struct BlockVertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 norm;
	DirectX::XMFLOAT2 uv;
};

/// NOTE!
// This is only a definition of how a block is created from neighboring vertices, as well as it's normals and UVs


static BlockVertex verts[24] =
{
	{ DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),    DirectX::XMFLOAT2(0.0f, 0.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),     DirectX::XMFLOAT2(0.0f, 1.0f) },
	{ DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),    DirectX::XMFLOAT2(1.0f, 1.0f) },

	{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
	{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 0.0f) },
	{ DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),   DirectX::XMFLOAT2(1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 1.0f) },

	{ DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 1.0f) },
	{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 0.0f) },
	{ DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),   DirectX::XMFLOAT2(0.0f, 0.0f) },

	{ DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),    DirectX::XMFLOAT2(1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),   DirectX::XMFLOAT2(0.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),    DirectX::XMFLOAT2(0.0f, 0.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),     DirectX::XMFLOAT2(1.0f, 0.0f) },

	{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),  DirectX::XMFLOAT2(1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),   DirectX::XMFLOAT2(1.0f, 0.0f) },
	{ DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),  DirectX::XMFLOAT2(0.0f, 0.0f) },

	{ DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f),   DirectX::XMFLOAT2(1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f),    DirectX::XMFLOAT2(0.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f),     DirectX::XMFLOAT2(0.0f, 0.0f) },
	{ DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f),    DirectX::XMFLOAT2(1.0f, 0.0f) },
};

static unsigned int indicies[36] =
{
	3, 1, 0, 2, 1, 3,
	6, 4, 5, 7, 4, 6,
	11, 9, 8, 10, 9, 11,
	14, 12, 13, 15, 12, 14,
	19, 17, 16, 18, 17, 19,
	22, 20, 21, 23, 20, 22
};

enum class BlockType 
{
	Air,
	Dirt,
	Stone,
	Grass,
	Wood
};


// I will create a Block class definition here but I am not yet going to use it
class Block
{
public:

	Block();
	Block(const Block&);
	~Block();

	void SetType(const BlockType type);
	const BlockType GetType();

private:

	// The block class stores as little as possible to reduce overhead, storing position is not necessary
	// We can store the type and use an "air" block as an inactive block
	BlockType m_type = BlockType::Air;
};

#endif
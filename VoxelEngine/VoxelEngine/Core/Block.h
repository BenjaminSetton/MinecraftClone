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
	{ DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),   DirectX::XMFLOAT2(1.0f, 0.0f) }, // TLF(0)
	{ DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),    DirectX::XMFLOAT2(0.0f, 0.0f) }, // TRF(1)
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),     DirectX::XMFLOAT2(0.0f, 1.0f) }, // TRB(2)
	{ DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),    DirectX::XMFLOAT2(1.0f, 1.0f) }, // TLB(3)

	{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) }, // BLF(4)
	{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 1.0f) }, // BRF(5)
	{ DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),   DirectX::XMFLOAT2(1.0f, 0.0f) }, // BRB(6)
	{ DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 0.0f) }, // BLB(7)

	{ DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 1.0f) }, // BLB(8)
	{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) }, // BLF(9)
	{ DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 0.0f) }, // TLF(10)
	{ DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),   DirectX::XMFLOAT2(0.0f, 0.0f) }, // TLB(11)

	{ DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),    DirectX::XMFLOAT2(1.0f, 1.0f) }, // BRB(12) 
	{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),   DirectX::XMFLOAT2(0.0f, 1.0f) }, // BRF(13)
	{ DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),    DirectX::XMFLOAT2(0.0f, 0.0f) }, // TRF(14)
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),     DirectX::XMFLOAT2(1.0f, 0.0f) }, // TRB(15)

	{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) }, // BLF(16)
	{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),  DirectX::XMFLOAT2(1.0f, 1.0f) }, // BRF(17)
	{ DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),   DirectX::XMFLOAT2(1.0f, 0.0f) }, // TRF(18)
	{ DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),  DirectX::XMFLOAT2(0.0f, 0.0f) }, // TLF(19)

	{ DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f),   DirectX::XMFLOAT2(1.0f, 1.0f) }, // BLB(20)
	{ DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f),    DirectX::XMFLOAT2(0.0f, 1.0f) }, // BRB(21)
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f),     DirectX::XMFLOAT2(0.0f, 0.0f) }, // TRB(22)
	{ DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f),    DirectX::XMFLOAT2(1.0f, 0.0f) }, // TLB(23)
};

static unsigned int indicies[36] =
{
	3, 1, 0, 2, 1, 3, // TOP FACE
	6, 5, 4, 7, 6, 4, // BOTTOM FACE
	11, 9, 8, 10, 9, 11, // LEFT FACE
	14, 12, 13, 15, 12, 14, // RIGHT FACE
	19, 17, 16, 18, 17, 19, // FRONT FACE
	22, 20, 21, 23, 20, 22 // BACK FACE
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
	Block(BlockType type);
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
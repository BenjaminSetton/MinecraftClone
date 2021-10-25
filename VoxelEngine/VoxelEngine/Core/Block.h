#ifndef _BLOCK_H
#define _BLOCK_H

#include <DirectXMath.h>
#include "../Utility/Utility.h"

struct BlockInstanceData
{
	DirectX::XMFLOAT3 worldPos;
	uint32_t blockType;
	uint32_t blockFaces;

};

struct BlockVertexData
{
	DirectX::XMFLOAT3 pos;
	uint32_t vertexIndex;
};

/// NOTE!
// This is only a definition of how a block is created from neighboring vertices, as well as it's normals and UVs


static BlockVertexData verts[36] =
{
	 // TOP
	 { DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 0  },	// TRB
	 { DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f), 1  },	// TRF
	 { DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), 2  },	// TLF
	 { DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), 3  },	// TLF
	 { DirectX::XMFLOAT3(0.0f, 1.0f, 1.0f), 4  },	// TLB
	 { DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 5  },	// TRB
	   									       
	  // BOTTOM						  	       
	 { DirectX::XMFLOAT3(1.0f, 0.0f, 1.0f), 6  },	// BRB
	 { DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), 7  },	// BRF
	 { DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 8  },	// BLF
	 { DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 9  },	// BLF
	 { DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), 10 },	// BLB
	 { DirectX::XMFLOAT3(1.0f, 0.0f, 1.0f), 11 },	// BRB
	   									      
	  // LEFT							      
	 { DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), 12 },	// TLF
	 { DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 13 },	// BLF
	 { DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), 14 },	// BLB
	 { DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), 15 },	// BLB
	 { DirectX::XMFLOAT3(0.0f, 1.0f, 1.0f), 16 },	// TLB
	 { DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), 17 },	// TLF
		 								       
	  // RIGHT							       
	 { DirectX::XMFLOAT3(1.0f, 0.0f, 1.0f), 18 },	// BRB 
	 { DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), 19 },	// BRF
	 { DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f), 20 },	// TRF
	 { DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f), 21 },	// TRF
	 { DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 22 },	// TRB
	 { DirectX::XMFLOAT3(1.0f, 0.0f, 1.0f), 23 },	// BRB 
		 								       
	  // FRONT							       
	 { DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f), 24 },	// TRF
	 { DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), 25 },	// BRF
	 { DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 26 },	// BLF
	 { DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), 27 },	// TLF
	 { DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f), 28 },	// TRF
	 { DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 29 },	// BLF
		 								       
	  // BACK							       
	 { DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), 30 },	// BLB
	 { DirectX::XMFLOAT3(1.0f, 0.0f, 1.0f), 31 },	// BRB
	 { DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 32 },	// TRB
	 { DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), 33 },	// BLB
	 { DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 34 },	// TRB
	 { DirectX::XMFLOAT3(0.0f, 1.0f, 1.0f), 35 },	// TLB
};

static unsigned int indicies[36] =
{
	3, 1, 0, 2, 1, 3,			// TOP FACE
	4, 5, 6, 4, 6, 7,			// BOTTOM FACE
	11, 9, 8, 10, 9, 11,		// LEFT FACE
	14, 12, 13, 15, 12, 14,		// RIGHT FACE
	19, 17, 16, 18, 17, 19,		// FRONT FACE
	22, 20, 21, 23, 20, 22		// BACK FACE
};

enum class BlockFace : uint8_t
{
	TOP = BIT(0),
	BOTTOM = BIT(1),
	LEFT = BIT(2),
	RIGHT = BIT(3),
	FRONT = BIT(4),
	BACK = BIT(5)
};

enum class BlockType : uint8_t
{
	Air = 0,
	Dirt,
	Stone,
	Grass,
	Wood
};


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
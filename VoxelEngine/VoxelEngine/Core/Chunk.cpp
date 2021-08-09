#include "../Misc/pch.h"

#include "Chunk.h"

#include "../Utility/Noise.h"

using namespace DirectX;

Chunk::Chunk(const XMFLOAT3 pos) : m_id(0), m_pos(pos), m_numFaces(0)
{
	// Ensures that a chunk will ALWAYS generate it's corresponding blocks
	InitializeChunk();
	InitializeBuffers();
}

Chunk::~Chunk()
{
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				Block* block = m_chunk[x][y][z];
				delete block;
			}
		}
	}
}

const uint32_t Chunk::GetID() { return m_id; }

const Block* Chunk::GetBlock(unsigned int x, unsigned int y, unsigned int z) { return m_chunk[x][y][z]; }

const DirectX::XMFLOAT3 Chunk::GetPosition() { return m_pos; }

const BlockVertex* Chunk::GetBlockFaces() { return &m_blockFaces[0]; }

const uint32_t Chunk::GetNumFaces() { return m_numFaces; }

void Chunk::InitializeChunk()
{
	//m_id = 0; // TODO: Replace with an actual UUID
	//m_pos = XMFLOAT3(0, 0, 0) // TODO: Relate the chunk ID with it's position

	// Populate the chunk array
	// For now we'll just initialize all blocks to "DIRT" blocks
	for(int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			float height = Noise2D::GenerateValue(x, z) * static_cast<float>(CHUNK_SIZE);
			for(int y = 0; y < CHUNK_SIZE; y++)
			{
				if(y < height) m_chunk[x][y][z] = new Block(BlockType::Dirt);
				else m_chunk[x][y][z] = new Block(BlockType::Air);
			}
		}
	}
}

void Chunk::InitializeBuffers()
{
	uint32_t index = 0;
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = CHUNK_SIZE - 1; y >= 0; y--)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				XMFLOAT3 blockPos = 
				{ static_cast<float>(x) + m_pos.x, 
				  static_cast<float>(y) + m_pos.y, 
				  static_cast<float>(z) + m_pos.z
				};

				// NOTE: appends faces that are not occluded (only render faces that can be seen)
				// RIGHT BACK AND TOP FACES RENDER TWICE

				// Only append faces if current block is not an air block
				if(m_chunk[x][y][z]->GetType() != BlockType::Air)
				{
					// left neighbor
					if (x - 1 < 0 || m_chunk[x - 1][y][z]->GetType() == BlockType::Air)
						AppendBlockFaceToArray(BlockFace::LEFT, index, blockPos);
					// right neighbor
					if (x + 1 > CHUNK_SIZE - 1 || m_chunk[x + 1][y][z]->GetType() == BlockType::Air)
						AppendBlockFaceToArray(BlockFace::RIGHT, index, blockPos);
					// top neighbor
					if (y + 1 > CHUNK_SIZE - 1 || m_chunk[x][y + 1][z]->GetType() == BlockType::Air)
						AppendBlockFaceToArray(BlockFace::TOP, index, blockPos);
					// bottom neighbor
					if (y - 1 < 0 || m_chunk[x][y - 1][z]->GetType() == BlockType::Air)
						AppendBlockFaceToArray(BlockFace::BOTTOM, index, blockPos);
					// front neighbor
					if (z - 1 < 0 || m_chunk[x][y][z - 1]->GetType() == BlockType::Air)
						AppendBlockFaceToArray(BlockFace::FRONT, index, blockPos);
					// back neighbor
					if (z + 1 > CHUNK_SIZE - 1 || m_chunk[x][y][z + 1]->GetType() == BlockType::Air)
						AppendBlockFaceToArray(BlockFace::BACK, index, blockPos);
				}

			}
		}
	}
}

void Chunk::AppendBlockFaceToArray(const BlockFace face, uint32_t& currIndex, const XMFLOAT3& blockPos)
{
	uint32_t startIndexArray = 0;

	switch (face)
	{
	case BlockFace::BACK:
	{
		startIndexArray = static_cast<uint32_t>(BlockFace::BACK);
		break;
	}
	case BlockFace::FRONT:
	{
		startIndexArray = static_cast<uint32_t>(BlockFace::FRONT);
		break;
	}
	case BlockFace::LEFT:
	{
		startIndexArray = static_cast<uint32_t>(BlockFace::LEFT);
		break;
	}
	case BlockFace::RIGHT:
	{
		startIndexArray = static_cast<uint32_t>(BlockFace::RIGHT);
		break;
	}
	case BlockFace::TOP:
	{
		startIndexArray = static_cast<uint32_t>(BlockFace::TOP);
		break;
	}
	case BlockFace::BOTTOM:
	{
		startIndexArray = static_cast<uint32_t>(BlockFace::BOTTOM);
		break;
	}
	}

	// Append the 6 vertices to the array (an entire face)
	for(uint32_t i = 0; i < 6; i++)
	{
		BlockVertex currVert = verts[indicies[startIndexArray++]];
		currVert.pos = { currVert.pos.x + blockPos.x, currVert.pos.y + blockPos.y, currVert.pos.z + blockPos.z };
		m_blockFaces[currIndex++] = currVert;
	}

	m_numFaces++;

}

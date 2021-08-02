#include "../Misc/pch.h"

#include "Chunk.h"

using namespace DirectX;

Chunk::Chunk() : m_active(true), m_id(0), m_pos(XMFLOAT3(-5.0f, 0, 0)), m_numBlocksToRender(0)
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

void Chunk::SetActive(const bool active) { m_active = active; }

const bool Chunk::GetActive() { return m_active; }

const DirectX::XMFLOAT3 Chunk::GetPosition() { return m_pos; }

const DirectX::XMFLOAT3* Chunk::GetBlockPositions() { return &m_blockPositions[0]; }

const uint16_t Chunk::GetNumBlocksToRender() { return m_numBlocksToRender; }

void Chunk::InitializeChunk()
{
	m_active = true;
	//m_id = 0; // TODO: Replace with an actual UUID
	//m_pos = XMFLOAT3(0, 0, 0) // TODO: Relate the chunk ID with it's position

	// Populate the chunk array
	// For now we'll just initialize all blocks to "DIRT" blocks
	for(int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				Block* newBlock = new Block(BlockType::Dirt);
				m_chunk[x][y][z] = newBlock;
			}
		}
	}
}

void Chunk::InitializeBuffers()
{
	uint16_t index = 0;
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				XMFLOAT3 blockPos = 
				{ static_cast<float>(x) + m_pos.x, 
				  static_cast<float>(-y) + m_pos.y, 
				  static_cast<float>(z) + m_pos.z
				};

				// left neighbor
				if (x - 1 >= 0 && m_chunk[x - 1][y][z]->GetType() != BlockType::Air) continue;
				// right neighbor
				if (x + 1 < CHUNK_SIZE - 1 && m_chunk[x + 1][y][z]->GetType() != BlockType::Air) continue;
				// top neighbor
				if (y + 1 < CHUNK_SIZE - 1 && m_chunk[x][y + 1][z]->GetType() != BlockType::Air) continue;
				// bottom neighbor
				if (y - 1 >= 0 && m_chunk[x][y - 1][z]->GetType() != BlockType::Air) continue;
				// front neighbor
				if (z - 1 >= 0 && m_chunk[x][y][z - 1]->GetType() != BlockType::Air) continue;
				// back neighbor
				if (z + 1 < CHUNK_SIZE - 1 && m_chunk[x][y][z + 1]->GetType() != BlockType::Air) continue;

				// It would be good to only render blocks that are not occluded

				// TODO: Check neighbors inside m_chunk to see if they are NOT air blocks
				//		 If a block is surrounded by 4 non-transparent blocks, don't render
				m_blockPositions[index++] = blockPos;
				m_numBlocksToRender++;

			}
		}
	}
}

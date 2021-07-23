#include "../Misc/pch.h"

#include "Chunk.h"

using namespace DirectX;

Chunk::Chunk()
{
	// Ensures that a chunk will ALWAYS generate it's corresponding blocks
	InitializeChunk();
	InitializeBuffers();
}

Chunk::~Chunk()
{
	m_active = false;
	m_chunkID = -1;
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

const uint32_t Chunk::GetChunkID() { return m_chunkID; }

const Block* Chunk::GetBlock(uint8_t x, uint8_t y, uint8_t z) { return m_chunk[x][y][z]; }

void Chunk::SetActive(const bool active) { m_active = active; }

const bool Chunk::GetActive() { return m_active; }

void Chunk::InitializeChunk()
{
	m_active = true;
	m_chunkID = 0; // TODO: Replace with an actual UUID

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
	unsigned int numBlocksPerChunk = pow(CHUNK_SIZE, 3);
	m_chunkVertices.reserve(24 * numBlocksPerChunk);
	m_chunkIndicies.reserve(36 * numBlocksPerChunk);

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				DirectX::XMFLOAT3 offset = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z) };
				//BlockVertex 
			}
		}
	}
}

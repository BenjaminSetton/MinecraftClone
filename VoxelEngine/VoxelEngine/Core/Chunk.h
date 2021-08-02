#ifndef _CHUNK_H
#define _CHUNK_H

#define CHUNK_SIZE 16

#include "Block.h"

class Chunk
{
public:

	Chunk();
	Chunk(const Chunk& other) = default; // I don't know why you would even do this, but I do it just in case
	~Chunk();


	const uint32_t GetID();

	const Block* GetBlock(unsigned int x, unsigned int y, unsigned int z);

	void SetActive(const bool active);
	const bool GetActive();

	const DirectX::XMFLOAT3 GetPosition();

	const DirectX::XMFLOAT3* GetBlockPositions();

	const uint16_t GetNumBlocksToRender();

private:

	// TODO: Think about generating a chunk based on a seed
	void InitializeChunk();

	void InitializeBuffers();

private:

	bool m_active;

	// TODO: Actually use this :)
	uint32_t m_id;

	uint16_t m_numBlocksToRender;

	DirectX::XMFLOAT3 m_pos;
	
	// Currently defines a 3D array of 16x16x16 blocks
	Block* m_chunk[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

	// Used for D3D11 instance buffer
	// NOTE: This equates to 49KB per chunk (roughly)
	DirectX::XMFLOAT3 m_blockPositions[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];

};

#endif
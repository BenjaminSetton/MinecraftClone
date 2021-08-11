#ifndef _CHUNK_H
#define _CHUNK_H

#define CHUNK_SIZE 16

#include "Block.h"

class Chunk
{
public:

	Chunk(const DirectX::XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f });
	Chunk(const Chunk& other) = default; // I don't know why you would even do this, but I do it just in case
	~Chunk();


	const uint32_t GetID();

	const Block* GetBlock(unsigned int x, unsigned int y, unsigned int z);

	const DirectX::XMFLOAT3 GetPosition();

	BlockVertex* GetBlockFaces();

	const uint32_t GetNumFaces();

private:

	// TODO: Think about generating a chunk based on a seed
	void InitializeChunk();

	void InitializeBuffers();

private:

	void AppendBlockFaceToArray(const BlockFace face, uint32_t& currIndex, const DirectX::XMFLOAT3& blockPos);

private:

	// TODO: Actually use this :)
	uint32_t m_id;

	uint32_t m_numFaces;

	// The chunk's position stored in world space, not chunk space
	DirectX::XMFLOAT3 m_pos;
	
	// Currently defines a 3D array of 16x16x16 blocks
	Block* m_chunk[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

	// Used for D3D11 buffer
	// NOTE: This equates to 786,432 bytes per chunk
	BlockVertex m_blockFaces[6 * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];

};

#endif
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


	const uint32_t GetChunkID();

	const Block* GetBlock(uint8_t x, uint8_t y, uint8_t z);

	void SetActive(const bool active);
	const bool GetActive();

private:

	// TODO: Think about generating a chunk based on a seed
	void Initialize();

private:

	bool m_active;
	uint32_t m_chunkID;

	// Currently defines a 3D array of 16x16x16 blocks
	Block* m_chunk[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

};

#endif
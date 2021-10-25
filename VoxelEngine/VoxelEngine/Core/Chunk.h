#ifndef _CHUNK_H
#define _CHUNK_H

#include "Block.h"
#include <d3d11.h>

// Macros
constexpr int32_t CHUNK_SIZE = 16;
constexpr int32_t DOUBLE_CHUNK_SIZE = (CHUNK_SIZE << 1);

class Chunk
{
public:

	friend class ChunkManager;

	Chunk(const DirectX::XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f });
	Chunk(const Chunk& other) = default; // I don't know why you would even do this, but I do it just in case
	~Chunk();

	Block* GetBlock(unsigned int x, unsigned int y, unsigned int z);

	// Returns chunks' position in CHUNK SPACE
	const DirectX::XMFLOAT3 GetPosition();

	const uint32_t GetFaceCount();

	void DrawChunkBorder();

	const uint32_t GetVertexBufferStartIndex();
	void SetVertexBufferStartIndex(const uint32_t startIndex);

	const uint32_t GetVertexCount();
	void SetVertexCount(const uint32_t vertexCount);

private:

	void InitializeChunk();

	// TODO: Work on optimizing this function!!
	void InitializeVertexBuffer();

	void ShutdownVertexBuffer();

private:

	// The chunk's position stored in CHUNK SPACE
	DirectX::XMFLOAT3 m_pos;
	
	// Currently defines a 3D array of 16x16x16 blocks
	Block m_chunk[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

	// Variables used for ChunkBufferManager
	uint32_t m_vertexBufferStartIndex;
	uint32_t m_blockCount;

};

#endif
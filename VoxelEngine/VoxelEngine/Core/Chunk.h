#ifndef _CHUNK_H
#define _CHUNK_H

#define CHUNK_SIZE 16

#include "Block.h"
#include <d3d11.h>

class Chunk
{
public:

	friend class ChunkManager;

	Chunk(const DirectX::XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f });
	Chunk(const Chunk& other) = default; // I don't know why you would even do this, but I do it just in case
	~Chunk();


	const uint32_t GetID();

	Block* GetBlock(unsigned int x, unsigned int y, unsigned int z);

	// Returns chunks' position in WORLD SPACE
	const DirectX::XMFLOAT3 GetPosition();

	const uint32_t GetFaceCount();

	const uint32_t GetVertexCount();

	ID3D11Buffer* GetBuffer();

private:

	// TODO: Think about generating a chunk based on a seed
	void InitializeChunk();

	void InitializeVertexBuffer();

	void CreateVertexBuffer();

private:

	void AppendBlockFaceToArray(const BlockFace& face, const BlockType& type, uint32_t& index, const DirectX::XMFLOAT3& blockPos, 
		BlockVertex* blockArray);

	void ResetFaces();

private:

	// TODO: Actually use this :)
	uint32_t m_id;

	uint32_t m_numFaces;

	// The chunk's position stored in CHUNK SPACE
	DirectX::XMFLOAT3 m_pos;
	
	// Currently defines a 3D array of 16x16x16 blocks
	Block m_chunk[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

	// D3D11 Vertex buffer
	ID3D11Buffer* m_buffer;

};

#endif
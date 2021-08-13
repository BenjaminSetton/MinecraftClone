#ifndef _CHUNKMANAGER_H
#define _CHUNKMANAGER_H

#include <vector>

#include "Chunk.h"

constexpr uint32_t MAX_VERTS_PER_CHUNK = 
	6/*verts per face*/ * 6/*faces per block*/ * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 0.5;


// This is a work in progress!!
class ChunkManager
{
public:

	static void Initialize(const DirectX::XMFLOAT3 playerPosWS);

	static void Shutdown();

	static void Update(const DirectX::XMFLOAT3 pos);

	static Chunk* LoadChunk(const DirectX::XMFLOAT3 chunkCS);

	static void UnloadChunk(Chunk* chunk);
	static void UnloadChunk(const uint16_t& index);

	static const uint16_t GetNumActiveChunks();

	static Chunk* GetChunkAtIndex(const uint16_t index);

	// Returns chunk at "pos" CHUNK SPACE
	static Chunk* GetChunkAtPos(const DirectX::XMFLOAT3 pos);

	static const uint32_t GetNumVertices();

	static const std::vector<BlockVertex>& GetVertices();

private:

	// Helper methods. Consider moving to a Utility library
	static DirectX::XMFLOAT3 WorldToChunkSpace(const DirectX::XMFLOAT3& pos);
	static DirectX::XMFLOAT3 ChunkToWorldSpace(const DirectX::XMFLOAT3& pos);

	static void ResetChunkMemory(const uint16_t index);

	static void PopulateVertexBuffer(Chunk* chunk);

private:
	
	// Stores active chunks in CHUNK SPACE
	static std::vector<Chunk*> m_activeChunks;

	// NOTE! Temporarily stored here
	// Consider moving to another "settings" or "game" class
	static uint16_t m_renderDist;

	static std::vector<BlockVertex> m_vertices;

};

#endif
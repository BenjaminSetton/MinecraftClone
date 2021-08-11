#ifndef _CHUNKMANAGER_H
#define _CHUNKMANAGER_H

#include <vector>

#include "Chunk.h"


// This static class just manages chunks the most naive way possible.
// This is a work in progress!!
class ChunkManager
{
public:

	static void Update(const DirectX::XMFLOAT3 pos);

	static void LoadChunk(const DirectX::XMFLOAT3 chunkWorldPos);

	static void UnloadChunk(Chunk* chunk);
	static void UnloadChunk(const uint16_t& index);

	static const uint16_t GetNumActiveChunks();

	static Chunk* GetChunkAt(const uint16_t index);

private:

	// Helper methods. Consider moving to a Utility library
	static DirectX::XMFLOAT3 WorldToChunkSpace(const DirectX::XMFLOAT3& pos);
	static DirectX::XMFLOAT3 ChunkToWorldSpace(const DirectX::XMFLOAT3& pos);

private:

	// TODO: Look into storing the chunks in an unordered map
	static std::vector<Chunk*> m_activeChunks;

	// An array of Chunks 
	static Chunk* m_activeChunk;

	// NOTE! Temporarily stored here
	// Consider moving to another "settings" or "game" class
	static uint16_t m_renderDist;

	// The maximum number of chunks is dictated by (2 * m_renderDist + 1)^2, considering
	// just the x and z axis since there are currently no vertical chunks
};

#endif
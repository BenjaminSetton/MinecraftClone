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

	// Helper method. Consider moving to a Utility library
	static DirectX::XMFLOAT3 WorldToChunkSpace(const DirectX::XMFLOAT3& pos);
	static DirectX::XMFLOAT3 ChunkToWorldSpace(const DirectX::XMFLOAT3& pos);

private:

	static std::vector<Chunk*> m_activeChunks;

	// Temporary RENDER_DIST. Consider moving to another "settings" or "game" class
	static uint16_t m_renderDist;

	// ChunkManager could store a vector of BlockVertex objects which would store all the vertices
	// of the active chunks (this would mean that all the data would be duplicated)
};

#endif
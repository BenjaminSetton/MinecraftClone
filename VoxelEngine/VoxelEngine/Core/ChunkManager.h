#ifndef _CHUNKMANAGER_H
#define _CHUNKMANAGER_H

#include <vector>

#include "Chunk.h"


// This singleton just manages chunks the most naive way possible.
// This is a work in progress!!
class ChunkManager
{
public:

	ChunkManager& Get();

	static void LoadChunk(Chunk* chunk);
	static void UnloadChunk(Chunk* chunk);

private:

	static ChunkManager m_instance;
	static std::vector<Chunk*> m_activeChunks;
};

#endif
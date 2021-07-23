#include "../Misc/pch.h"

#include "ChunkManager.h"

// Static variable definitions

ChunkManager ChunkManager::m_instance;
std::vector<Chunk*> ChunkManager::m_activeChunks = std::vector<Chunk*>();


// Method implementations

ChunkManager& ChunkManager::Get() { return m_instance; }

void ChunkManager::LoadChunk(Chunk* chunk) { m_activeChunks.push_back(chunk); }

void ChunkManager::UnloadChunk(Chunk* chunk)
{
	///////////////////////////////////////
	///
	///		This is trash :)!!!!
	/// 
	///////////////////////////////////////

	// TODO: 
	// - Consider loading/unloading chunks on a separate thread
	// - Consider deleting un-modified chunks from memory (they can be loaded in again through a seed)
	// - Modified chunks should be considered as "inactive" when unloaded, removed from
	//   the m_activeChunks vector and serialized into a txt file
	uint16_t index = 0;
	for(auto& iter : m_activeChunks)
	{
		// For now I will just set the chunk as inactive and remove it from the vector
		if (iter == chunk)
		{
			iter->SetActive(false);
			m_activeChunks.erase(m_activeChunks.begin() + index);
		}
		index++;
	}
}

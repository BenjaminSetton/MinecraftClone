#include "../Misc/pch.h"

#include "ChunkManager.h"

// Static variable definitions

std::vector<Chunk*> ChunkManager::m_activeChunks = std::vector<Chunk*>();
uint16_t ChunkManager::m_renderDist = 5;

using namespace DirectX;

// Method implementations

void ChunkManager::Update(const DirectX::XMFLOAT3 playerPos)
{
	// Chunk coord
	XMFLOAT3 playerPosChunkSpace = WorldToChunkSpace(playerPos);

	// Loop through all active chunks
	int iter = 0;
	for(auto& chunk : m_activeChunks)
	{
		XMFLOAT3 chunkPosChunkSpace = WorldToChunkSpace(chunk->GetPosition());

		float chunkDist =
			(chunkPosChunkSpace.x - playerPosChunkSpace.x) * (chunkPosChunkSpace.x - playerPosChunkSpace.x) +
			(chunkPosChunkSpace.y - playerPosChunkSpace.y) * (chunkPosChunkSpace.y - playerPosChunkSpace.y) +
			(chunkPosChunkSpace.z - playerPosChunkSpace.z) * (chunkPosChunkSpace.z - playerPosChunkSpace.z);

		// 1. Unload chunks if they are too far away from "player"
		if (chunkDist >= m_renderDist * m_renderDist)
			UnloadChunk(iter--);

		iter++;
	}

	// 2. Load chunks if they are inside render distance
	bool chunkExists = false;
	for(uint16_t x = 0; x < m_renderDist; x++)
	{
		for(uint16_t z = 0; z < m_renderDist; z++)
		{
			// A coordinate in chunk space
			XMFLOAT3 newChunkPosCS = { playerPosChunkSpace.x + x, 0, playerPosChunkSpace.z + z };
			// If this new chunk is not already active, allocate a new chunk
			chunkExists = false;
			for (auto currChunk : m_activeChunks)
			{
				XMFLOAT3 currChunkPos = WorldToChunkSpace(currChunk->GetPosition());
				if
					(
						currChunkPos.x == newChunkPosCS.x &&
						currChunkPos.y == newChunkPosCS.y &&
						currChunkPos.z == newChunkPosCS.z
						)
				{
					chunkExists = true;
					break;
				}
			}
			if(!chunkExists)
			{
				XMFLOAT3 chunkWorldSpace = ChunkToWorldSpace(newChunkPosCS);
				LoadChunk(chunkWorldSpace);
			}
		}
	}



}

void ChunkManager::LoadChunk(const XMFLOAT3 chunkWorldPos) 
{
	Chunk* chunk = new Chunk(chunkWorldPos);
	m_activeChunks.push_back(chunk); 
}

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
			Chunk* chunk = m_activeChunks[index];
			delete chunk;
			m_activeChunks.erase(m_activeChunks.begin() + index);
		}
		index++;
	}
}


void ChunkManager::UnloadChunk(const uint16_t& index)
{
	delete m_activeChunks[index];
	m_activeChunks.erase(m_activeChunks.begin() + index);
}

const uint16_t ChunkManager::GetNumActiveChunks() { return m_activeChunks.size(); }

Chunk* ChunkManager::GetChunkAt(const uint16_t index)
{
	if (index < m_activeChunks.size())
		return m_activeChunks[index];
	else
		return nullptr;
}

XMFLOAT3 ChunkManager::WorldToChunkSpace(const XMFLOAT3& pos)
{
	return { (float)((int)pos.x / CHUNK_SIZE), (float)((int)pos.y / CHUNK_SIZE), (float)((int)pos.z / CHUNK_SIZE) };
}

XMFLOAT3 ChunkManager::ChunkToWorldSpace(const XMFLOAT3& pos)
{
	// NOTE! pos should contain whole numbers only
	return { pos.x * CHUNK_SIZE, pos.y * CHUNK_SIZE, pos.z * CHUNK_SIZE };
}

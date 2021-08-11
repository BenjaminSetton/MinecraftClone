#include "../Misc/pch.h"

#include "ChunkManager.h"
#include "../Utility/Utility.h"
#include "../../imgui/imgui.h"


// Static variable definitions

std::vector<Chunk*> ChunkManager::m_activeChunks = std::vector<Chunk*>();
uint16_t ChunkManager::m_renderDist = 4;
Chunk* ChunkManager::m_activeChunk = new Chunk[pow(2 * m_renderDist + 1, 2)];

using namespace DirectX;

// Method implementations

void ChunkManager::Update(const DirectX::XMFLOAT3 playerPos)
{
	VX_PROFILE_FUNC();

	ImGui::Begin("Debug Panel");
	ImGui::Text("(PRE-UNLOAD) Number of chunks: %d", m_activeChunks.size());

	// Chunk coord
	XMFLOAT3 playerPosChunkSpace = WorldToChunkSpace(playerPos);

	// 1. Unload chunks outside of render distance
	int iter = 0;
	for (auto& chunk : m_activeChunks)
	{
		XMFLOAT3 chunkPosChunkSpace = WorldToChunkSpace(chunk->GetPosition());

		XMFLOAT3 chunkDistFromPlayer =
		{
			abs(chunkPosChunkSpace.x - playerPosChunkSpace.x),
			abs(chunkPosChunkSpace.y - playerPosChunkSpace.y),
			abs(chunkPosChunkSpace.z - playerPosChunkSpace.z),
		};

		// 1. Unload chunks if they are too far away from "player"
		if (chunkDistFromPlayer.x > m_renderDist || chunkDistFromPlayer.z > m_renderDist)
			UnloadChunk(iter--);

		iter++;
	}

	ImGui::Text("(POST-UNLOAD) Number of chunks: %d", m_activeChunks.size());

	// 2. Load chunks if they are inside render distance
	bool chunkExists = false;
	for(int16_t x = -m_renderDist; x <= m_renderDist; x++)
	{
		for(int16_t z = -m_renderDist; z <= m_renderDist; z++)
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

	ImGui::Text("(POST-LOAD) Number of chunks: %d", m_activeChunks.size());

	ImGui::End();

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

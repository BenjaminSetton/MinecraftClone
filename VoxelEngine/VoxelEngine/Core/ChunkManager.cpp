#include "../Misc/pch.h"

#include "ChunkManager.h"
#include "../Utility/Utility.h"
#include "../../imgui/imgui.h"
#include "Chunk.h"


///
/// TODO:
/// 
///		Consider loading/unloading chunks in a separate thread
///		Consider using an unordered map to speed up chunk searching
/// 

using namespace DirectX;

// Static variable definitions

std::vector<Chunk*> ChunkManager::m_activeChunks = std::vector<Chunk*>();
uint16_t ChunkManager::m_renderDist = 8;


void ChunkManager::Initialize(const XMFLOAT3 playerPosWS)
{
	m_activeChunks.reserve(pow(2 * m_renderDist + 1, 2));

	XMFLOAT3 playerPosCS = WorldToChunkSpace(playerPosWS);

	// Load all of the initial chunks
	uint32_t iter = 0;
	for (int16_t x = -m_renderDist; x <= m_renderDist; x++)
	{
		for (int16_t z = -m_renderDist; z <= m_renderDist; z++)
		{
			// A coordinate in chunk space
			XMFLOAT3 newChunkPosCS = { playerPosCS.x + x, 0, playerPosCS.z + z };

			// Create a new chunk
			m_activeChunks.emplace_back(new Chunk(newChunkPosCS));
			iter++;
		}
	}

	{
		VX_PROFILE_SCOPE_MSG_MODE("Initial Chunk Loading", 1);
		// Initialize all the chunks' buffers
		// Consider adding multi-threading to speed up load time
		for (uint16_t iter = 0; iter < m_activeChunks.size(); iter++) m_activeChunks[iter]->InitializeVertexBuffer();
	}
}

void ChunkManager::Shutdown()
{
	m_activeChunks.clear();
}

// Method implementations

void ChunkManager::Update(const DirectX::XMFLOAT3 playerPos)
{
	VX_PROFILE_FUNC();

	std::vector<Chunk*> newChunkList;

	// Chunk coord
	XMFLOAT3 playerPosChunkSpace = WorldToChunkSpace(playerPos);


	// 1. Unload chunks outside of render distance
	for (uint32_t i = 0; i < m_activeChunks.size(); i++)
	{
		Chunk* chunk = m_activeChunks[i];

		XMFLOAT3 chunkPosChunkSpace = chunk->GetPosition();

		XMFLOAT3 chunkDistFromPlayer =
		{
			abs(chunkPosChunkSpace.x - playerPosChunkSpace.x),
			abs(chunkPosChunkSpace.y - playerPosChunkSpace.y),
			abs(chunkPosChunkSpace.z - playerPosChunkSpace.z),
		};

		// 1. Unload chunks if they are too far away from "player"
		if (chunkDistFromPlayer.x > m_renderDist || chunkDistFromPlayer.z > m_renderDist)
			UnloadChunk(i--);

	}
	

	// 2. Load chunks if they are inside render distance

	// Z-axis chunk checking (includes corner chunks)
	for (int16_t x = -m_renderDist; x <= m_renderDist; x += 2 * m_renderDist)
	{
		for (int16_t z = -m_renderDist; z <= m_renderDist; z++)
		{
			// A coordinate in chunk space
			XMFLOAT3 newChunkPosCS = { playerPosChunkSpace.x + x, 0, playerPosChunkSpace.z + z };

			// If this new chunk is not already active, allocate a new chunk
			if (GetChunkAtPos(newChunkPosCS) != nullptr) continue;
			else
			{
				Chunk* newChunk = LoadChunk(newChunkPosCS);
				newChunkList.push_back(newChunk);
			}

		}
	}

	// X-axis chunk checking (excludes corner chunks)
	for (int16_t z = -m_renderDist; z <= m_renderDist; z += 2 * m_renderDist) 
	{
		for (int16_t x = -m_renderDist + 1; x < m_renderDist; x++)
		{
			// A coordinate in chunk space
			XMFLOAT3 newChunkPosCS = { playerPosChunkSpace.x + x, 0, playerPosChunkSpace.z + z };

			// If this new chunk is not already active, allocate a new chunk
			if (GetChunkAtPos(newChunkPosCS) != nullptr) continue;
			else
			{
				Chunk* newChunk = LoadChunk(newChunkPosCS);
				newChunkList.push_back(newChunk);
			}

		}
	}
	


	// 3. Force all new chunks and their neighbors to initialize their buffers
	for (uint16_t i = 0; i < newChunkList.size(); i++)
	{
		Chunk* newChunk = newChunkList[i];
		XMFLOAT3 chunkPosCS = newChunk->GetPosition();

		// Left neighbor
		Chunk* leftNeighbor = GetChunkAtPos({ chunkPosCS.x - 1, chunkPosCS.y, chunkPosCS.z });
		if (leftNeighbor) leftNeighbor->InitializeVertexBuffer();


		// Right neighbor
		Chunk* rightNeighbor = GetChunkAtPos({ chunkPosCS.x + 1, chunkPosCS.y, chunkPosCS.z });
		if (rightNeighbor) rightNeighbor->InitializeVertexBuffer();

		// Top neighbor
		Chunk* topNeighbor = GetChunkAtPos({ chunkPosCS.x, chunkPosCS.y + 1, chunkPosCS.z });
		if (topNeighbor) topNeighbor->InitializeVertexBuffer();

		// Bottom neighbor
		Chunk* bottomNeighbor = GetChunkAtPos({ chunkPosCS.x, chunkPosCS.y - 1, chunkPosCS.z });
		if (bottomNeighbor) bottomNeighbor->InitializeVertexBuffer();

		// Front neighbor
		Chunk* frontNeighbor = GetChunkAtPos({ chunkPosCS.x, chunkPosCS.y, chunkPosCS.z - 1 });
		if (frontNeighbor) frontNeighbor->InitializeVertexBuffer();

		// Back neighbor
		Chunk* backNeighbor = GetChunkAtPos({ chunkPosCS.x, chunkPosCS.y, chunkPosCS.z + 1 });
		if (backNeighbor) backNeighbor->InitializeVertexBuffer();

		// Current chunk
		newChunk->InitializeVertexBuffer();
	}
	

	ImGui::Begin("Debug Panel");
	ImGui::Text("Player Position (Chunk Space): %i, %i, %i", (int)playerPosChunkSpace.x, 0, (int)playerPosChunkSpace.z);
	ImGui::Text("Active Chunks: %i", m_activeChunks.size());
	ImGui::Text("Render Distance: %i", m_renderDist);
	ImGui::End();
}

Chunk* ChunkManager::LoadChunk(const XMFLOAT3 chunkCS) 
{
	Chunk* chunk = new Chunk(chunkCS);
	m_activeChunks.emplace_back(chunk); 

	return chunk;
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

			// Delete chunk instance
			delete chunk;
			m_activeChunks.erase(m_activeChunks.begin() + index);
		}
		index++;
	}
}


void ChunkManager::UnloadChunk(const uint16_t& index)
{
	Chunk* chunk = m_activeChunks[index];

	// Delete chunk instance
	delete chunk;
	m_activeChunks.erase(m_activeChunks.begin() + index);

}

const uint16_t ChunkManager::GetNumActiveChunks() { return m_activeChunks.size(); }


Chunk* ChunkManager::GetChunkAtIndex(const uint16_t index)
{
	if (index < m_activeChunks.size())
		return m_activeChunks[index];
	else
		return nullptr;
}

Chunk* ChunkManager::GetChunkAtPos(const DirectX::XMFLOAT3 posCS)
{
	int16_t index = -1;
	for(uint16_t iter = 0; iter < m_activeChunks.size(); iter++)
	{
		XMFLOAT3 currChunkPosCS = m_activeChunks[iter]->GetPosition();
		if
		(
			posCS.x == currChunkPosCS.x &&
			posCS.y == currChunkPosCS.y &&
			posCS.z == currChunkPosCS.z
		)
		{
			index = iter;
			break;
		}
	}

	if (index == -1) return nullptr;
	else return m_activeChunks[index];
}

std::vector<Chunk*>& ChunkManager::GetChunkVector() { return m_activeChunks; }

XMFLOAT3 ChunkManager::WorldToChunkSpace(const XMFLOAT3& pos)
{
	return { (float)((int)pos.x / CHUNK_SIZE), (float)((int)pos.y / CHUNK_SIZE), (float)((int)pos.z / CHUNK_SIZE) };
}

XMFLOAT3 ChunkManager::ChunkToWorldSpace(const XMFLOAT3& pos)
{
	// NOTE! pos should contain whole numbers only
	return { pos.x * CHUNK_SIZE, pos.y * CHUNK_SIZE, pos.z * CHUNK_SIZE };
}

void ChunkManager::ResetChunkMemory(const uint16_t index)
{
	memset(&m_activeChunks[index], 0, sizeof(Chunk));
}

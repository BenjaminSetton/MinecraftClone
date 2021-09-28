#include "../Misc/pch.h"

#include "ChunkManager.h"
#include "../Utility/Utility.h"
#include "../../imgui/imgui.h"
#include "FrustumCulling.h"
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
bool ChunkManager::m_runUpdater = true;
XMFLOAT3 ChunkManager::m_playerPos = { 0.0f, 0.0f, 0.0f };

std::vector<XMFLOAT3> ChunkManager::m_newChunkList = std::vector<XMFLOAT3>();
std::vector<uint32_t> ChunkManager::m_deletedChunkList = std::vector<uint32_t>();
std::thread* ChunkManager::m_updaterThread = nullptr;
std::mutex ChunkManager::m_canAccessVec;


void ChunkManager::Initialize(const XMFLOAT3 playerPosWS)
{
	// Isn't strictly necessary b/c it's inited as a static variable
	m_runUpdater = true;

	m_playerPos = playerPosWS;

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

			// Create a new chunk if within view frustum
			if (FrustumCulling::CalculateChunkPosAgainstFrustum(ChunkToWorldSpace(newChunkPosCS)))
			{
				m_activeChunks.emplace_back(new Chunk(newChunkPosCS));
				iter++;
			}
		}
	}

	{
		VX_PROFILE_SCOPE_MSG_MODE("Initial Chunk Loading", 1);
		// Initialize all the chunks' buffers
		// Consider adding multi-threading to speed up load time
		for (uint16_t iter = 0; iter < m_activeChunks.size(); iter++) m_activeChunks[iter]->InitializeVertexBuffer();
	}


	// Start the updater thread
	m_updaterThread = new std::thread(UpdaterEntryPoint);
}

void ChunkManager::Shutdown()
{
	m_runUpdater = false;
	if(m_updaterThread->joinable())
	{
		m_updaterThread->join();
	}
	else
	{
		VX_ASSERT(false, "Fatal error joining thread");
	}
	delete m_updaterThread;

	m_activeChunks.clear();
}

// Method implementations

void ChunkManager::Update()
{
	//VX_PROFILE_FUNC();

	// Chunk coord
	XMFLOAT3 playerPosChunkSpace = WorldToChunkSpace(m_playerPos);


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
		if (chunkDistFromPlayer.x > m_renderDist || chunkDistFromPlayer.z > m_renderDist
			|| !FrustumCulling::CalculateChunkPosAgainstFrustum(ChunkToWorldSpace(chunkPosChunkSpace)))
		{
			m_deletedChunkList.push_back(i);
		}

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
				// If chunk is within frustum
				if (FrustumCulling::CalculateChunkPosAgainstFrustum(ChunkToWorldSpace(newChunkPosCS)));
					m_newChunkList.push_back(newChunkPosCS);
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
				// If chunk is within frustum
				if(FrustumCulling::CalculateChunkPosAgainstFrustum(ChunkToWorldSpace(newChunkPosCS)));
					m_newChunkList.push_back(newChunkPosCS);
			}

		}
	}

	// 3. Delete / unload out-of-render-distance chunks
	uint32_t indexCorrection = 0;
	m_canAccessVec.lock();
	for (auto chunkIndex : m_deletedChunkList)
	{
		UnloadChunk(chunkIndex - indexCorrection++);
	}

	// 4. Load new chunks and force all their neighbors to initialize or re-initialize their buffers
	for (uint16_t i = 0; i < m_newChunkList.size(); i++)
	{
		// If "New Chunk" is not in frustum view, don't LoadChunk
		Chunk* newChunk = LoadChunk(m_newChunkList[i]);
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
	m_canAccessVec.unlock();

	// Clear the temp new/deleted vectors
	m_newChunkList.clear();
	m_deletedChunkList.clear();
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
	delete m_activeChunks[index];
	m_activeChunks.erase(m_activeChunks.begin() + index);
}

const uint16_t ChunkManager::GetNumActiveChunks()
{
	std::lock_guard<std::mutex> guard(m_canAccessVec);
	return m_activeChunks.size();
}


Chunk* ChunkManager::GetChunkAtIndex(const uint16_t index)
{
	std::lock_guard<std::mutex> guard(m_canAccessVec);
	if (index < m_activeChunks.size())
	{
		return m_activeChunks[index];
	}
	else
	{
		return nullptr;
	}
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

std::vector<Chunk*>& ChunkManager::GetChunkVector() 
{
	return m_activeChunks;
}

void ChunkManager::UpdaterEntryPoint()
{
	// "Infinite" while loop; only break out if ChunkManager is shut down...
	while(m_runUpdater)
	{
		Update();
	}
}

void ChunkManager::SetPlayerPos(DirectX::XMFLOAT3 playerPos) { m_playerPos = playerPos; }

void ChunkManager::CheckOutChunkVector()
{
	m_canAccessVec.lock();
}

void ChunkManager::ReturnChunkVector()
{
	m_canAccessVec.unlock();
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

void ChunkManager::ResetChunkMemory(const uint16_t index)
{
	memset(&m_activeChunks[index], 0, sizeof(Chunk));
}

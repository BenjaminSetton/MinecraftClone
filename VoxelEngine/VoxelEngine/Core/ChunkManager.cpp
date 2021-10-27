#include "../Misc/pch.h"

#include "ChunkManager.h"
#include "../Utility/Utility.h"
#include "../../imgui/imgui.h"
#include "Chunk.h"
#include "../Utility/ImGuiLayer.h"

#include "../Utility/Noise.h"

using namespace DirectX;

// Static variable definitions
SortedPool<Chunk> ChunkManager::m_activeChunks = SortedPool<Chunk>((2 * RENDER_DIST + 1) * (2 * RENDER_DIST + 1) * (2 * RENDER_DIST + 1));
bool ChunkManager::m_runThreads = true;
XMFLOAT3 ChunkManager::m_playerPos = { 0.0f, 0.0f, 0.0f };
std::vector<XMFLOAT3> ChunkManager::m_newChunkList = std::vector<XMFLOAT3>();
std::vector<XMFLOAT3> ChunkManager::m_deletedChunkList = std::vector<XMFLOAT3>();
std::thread* ChunkManager::m_updaterThread = nullptr;
std::mutex ChunkManager::m_canAccessVec;
bool ChunkManager::m_isShuttingDown = false;

#define ALLOW_HARD_CODED_MAX_INIT_THREADS 1
#define USE_DEFAULT_SEED 1
#define USE_SEED_BASED_ON_SYSTEM_TIME 1

constexpr int CHUNK_GENERATION_SEED = 12346;

#if ALLOW_HARD_CODED_MAX_INIT_THREADS == 1
const uint32_t g_maxNumThreadsForInit = 5;
#else
uint32_t g_maxNumThreadsForInit;
#endif


std::unordered_map<uint64_t, Chunk*> ChunkManager::m_chunkMap = std::unordered_map<uint64_t, Chunk*>();
std::unordered_map<uint64_t, uint32_t> ChunkManager::m_poolMap = std::unordered_map<uint64_t, uint32_t>();


void ChunkManager::Initialize(const XMFLOAT3 playerPosWS)
{
	// Isn't strictly necessary b/c it's inited as a static variable
	m_runThreads = true;

	Renderer_Data::renderDist = RENDER_DIST;

#if USE_DEFAULT_SEED == 0
#if USE_SEED_BASED_ON_SYSTEM_TIME == 0
	Noise2D::SetSeed(CHUNK_GENERATION_SEED);
#else
	Noise2D::SetSeed(static_cast<int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
#endif // USE_SEED_BASED_ON_SYSTEM_TIME
#else
	Noise2D::SetSeed(CHUNK_GENERATION_SEED);
#endif // USE_DEFAULT_SEED

	m_playerPos = playerPosWS;

	XMFLOAT3 playerPosCS = WorldToChunkSpace(playerPosWS);

#if ALLOW_HARD_CODED_MAX_INIT_THREADS == 0
	// hardware_concurrency() will only work for Windows builds...not particularly important
	// right now but should keep this in mind!
	g_maxNumThreadsForInit = std::thread::hardware_concurrency();
#endif

	uint32_t desiredDepthSlicesPerThread, actualDepthSlicesPerThread;
	desiredDepthSlicesPerThread = actualDepthSlicesPerThread = 2;
	uint32_t numThreadsToRun = floor((2 * RENDER_DIST + 1) / desiredDepthSlicesPerThread);

	// If we need more threads to run desired depth slices per thread, cap at map threads and recalculate
	// actualDepthSlicesPerThread to reflect change in numThreadsToRun
	if (numThreadsToRun > g_maxNumThreadsForInit)
	{
		numThreadsToRun = g_maxNumThreadsForInit;
		actualDepthSlicesPerThread = floor((2 * RENDER_DIST + 1) / numThreadsToRun);
	}


	{
		VX_PROFILE_SCOPE_MODE("Chunk Loading", 1);


		// [MULTI-THREADED]		Load all of the initial chunks
		VX_LOG_INFO("%i Chunk initializer threads launched", numThreadsToRun);
		std::vector<std::thread*> chunkLoaderThreads(numThreadsToRun);
		for (int16_t threadID = 0; threadID < numThreadsToRun; threadID++)
		{
			int32_t startingChunk = threadID * actualDepthSlicesPerThread - RENDER_DIST;
			int32_t numChunksToInit = threadID == numThreadsToRun - 1 ? actualDepthSlicesPerThread + ((2 * RENDER_DIST + 1) - numThreadsToRun * actualDepthSlicesPerThread) : actualDepthSlicesPerThread;
			std::thread* currThread = new std::thread(InitChunksMultithreaded, startingChunk, numChunksToInit, playerPosCS);
			chunkLoaderThreads[threadID] = currThread;

			VX_LOG_INFO("Thread %i initialized %i chunks from %i to %i", threadID, numChunksToInit, startingChunk, startingChunk + numChunksToInit);
		}

		// Join all initer threads before proceeding
		for (auto thread : chunkLoaderThreads) thread->join();
		chunkLoaderThreads.clear();

	}

	{
		VX_PROFILE_SCOPE_MODE("Chunk Vertex Buffers", 1);

		// [MULTI-THREADED]		Initialize all the chunks' vertex buffers
		VX_LOG_INFO("%i vertex buffer initializer threads launched", numThreadsToRun);
		std::vector<std::thread*> vertexBufferThreads(numThreadsToRun);
		uint32_t numIndicesPerChunk = m_activeChunks.Size() / numThreadsToRun;
		for (int16_t threadID = 0; threadID < numThreadsToRun; threadID++)
		{
			uint32_t startingIndex = threadID * numIndicesPerChunk;
			uint32_t numIndiciesToInit = threadID == numThreadsToRun - 1 ? m_activeChunks.Size() - startingIndex : numIndicesPerChunk;
			std::thread* currThread = new std::thread(InitChunkVertexBuffersMultithreaded, startingIndex, numIndiciesToInit);
			vertexBufferThreads[threadID] = currThread;
			VX_LOG_INFO("Thread %i initialized %i indicies from %i to %i", threadID, numIndiciesToInit, startingIndex, startingIndex + numIndiciesToInit);
		}

		// Join all initer threads before proceeding
		for (auto thread : vertexBufferThreads) thread->join();
		vertexBufferThreads.clear();
	}

	// Start the updater thread
	m_updaterThread = new std::thread(UpdaterEntryPoint);

}

void ChunkManager::Shutdown()
{
	m_isShuttingDown = true;

	m_runThreads = false;
	if(m_updaterThread->joinable())
	{
		m_updaterThread->join();
	}
	else
	{
		VX_ASSERT(false, "Fatal error joining updater thread");
	}
	delete m_updaterThread;

	m_chunkMap.clear();
	m_poolMap.clear();

	m_activeChunks.Clear();

	m_newChunkList.clear();
	m_deletedChunkList.clear();

}

// Method implementations

void ChunkManager::Update()
{
	// Keep track of the previous chunk pos of the player to know how many chunks to check
	// this frame!
	static XMFLOAT3 prevPosChunkSpace = WorldToChunkSpace(m_playerPos);

	VX_PROFILE_OUT(&ChunkManager_Data::updateTimer);

	XMFLOAT3 playerPosChunkSpace = WorldToChunkSpace(m_playerPos);

	{
		VX_PROFILE_OUT(&ChunkManager_Data::deletionLoop);

		// We have to unload however many chunks go out of range when
		// prevPos != currentPos in the axis where the difference happens
		//bool tempCheck = CheckForOutOfRangeChunks(playerPosChunkSpace, prevPosChunkSpace);

		// 1. Unload chunks outside of render distance
		for (uint32_t i = 0; i < m_activeChunks.Size(); i++)
		{
			if (!m_activeChunks[i]) continue;
			Chunk* chunk = m_activeChunks[i];

			XMFLOAT3 chunkPosChunkSpace = chunk->GetPosition();

			XMFLOAT3 chunkDistFromPlayer =
			{
				abs(chunkPosChunkSpace.x - playerPosChunkSpace.x),
				abs(chunkPosChunkSpace.y - playerPosChunkSpace.y),
				abs(chunkPosChunkSpace.z - playerPosChunkSpace.z),
			};

			// 1. Unload chunks if they are too far away from "player"
			if (chunkDistFromPlayer.x > RENDER_DIST || chunkDistFromPlayer.y > RENDER_DIST || chunkDistFromPlayer.z > RENDER_DIST)
			{
				m_deletedChunkList.push_back(chunk->GetPosition());
			}

		}
	}

	int debug_timesLooped = 0;
	{
		VX_PROFILE_OUT(&ChunkManager_Data::creationLoop);
		// 2. Load chunks if they are inside render distance

		// Z-axis chunk checking (includes corner chunks)
		for (int16_t x = -RENDER_DIST; x <= RENDER_DIST; x += 2 * RENDER_DIST)
		{
			for (int16_t y = -RENDER_DIST; y <= RENDER_DIST; y++)
			{
				for (int16_t z = -RENDER_DIST; z <= RENDER_DIST; z++)
				{
					debug_timesLooped++;
					// A coordinate in chunk space
					XMFLOAT3 newChunkPosCS = { playerPosChunkSpace.x + x, playerPosChunkSpace.y + y, playerPosChunkSpace.z + z };

					// If this new chunk is not already active, allocate a new chunk
					if (GetChunkAtPos(newChunkPosCS) != nullptr) continue;
					else
					{
						m_newChunkList.push_back(newChunkPosCS);
					}

				}
			}
		}

		// X-axis chunk checking (excludes corner chunks)
		for (int16_t z = -RENDER_DIST; z <= RENDER_DIST; z += 2 * RENDER_DIST)
		{
			for (int16_t y = -RENDER_DIST; y <= RENDER_DIST; y++)
			{
				for (int16_t x = -RENDER_DIST + 1; x < RENDER_DIST; x++)
				{
					debug_timesLooped++;
					// A coordinate in chunk space
					XMFLOAT3 newChunkPosCS = { playerPosChunkSpace.x + x, playerPosChunkSpace.y + y, playerPosChunkSpace.z + z };

					// If this new chunk is not already active, allocate a new chunk
					if (GetChunkAtPos(newChunkPosCS) != nullptr) continue;
					else
					{
						m_newChunkList.push_back(newChunkPosCS);
					}
				}
			}
		}

		// Y-axis chunk checking (no need to check the outer XZ plane outline)
		for (int16_t y = -RENDER_DIST; y <= RENDER_DIST; y += 2 * RENDER_DIST)
		{
			for (int16_t x = -RENDER_DIST + 1; x < RENDER_DIST; x++)
			{
				for (int16_t z = -RENDER_DIST + 1; z < RENDER_DIST; z++)
				{
					debug_timesLooped++;
					// A coordinate in chunk space
					XMFLOAT3 newChunkPosCS = { playerPosChunkSpace.x + x, playerPosChunkSpace.y + y, playerPosChunkSpace.z + z };

					// If this new chunk is not already active, allocate a new chunk
					if (GetChunkAtPos(newChunkPosCS) != nullptr) continue;
					else
					{
						m_newChunkList.push_back(newChunkPosCS);
					}
				}
			}
		}
	}

	{
		VX_PROFILE_OUT(&ChunkManager_Data::deletingChunks);
		// 3. Delete / unload out-of-render-distance chunks

		for (const auto& chunkPos : m_deletedChunkList)
		{
			// Use the map to map chunk position to index inside pool
			uint32_t index = m_poolMap[GetHashKeyFromChunkPosition(chunkPos)];
			UnloadChunk(index);
		}
	}
	

	{
		VX_PROFILE_OUT(&ChunkManager_Data::creatingChunks);

		// 4. Load new chunks and force all their neighbors to initialize or re-initialize their buffers
		for (uint16_t i = 0; i < m_newChunkList.size(); i++)
		{
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
	}
	
	VX_ASSERT(m_activeChunks.Size() == pow((2 * RENDER_DIST + 1), 3));

	// Clear the temporary vectors
	m_newChunkList.clear();
	m_deletedChunkList.clear();

	// Store the current pos and the previous pos
	prevPosChunkSpace = playerPosChunkSpace;

}

Chunk* ChunkManager::LoadChunk(const XMFLOAT3 chunkCS) 
{
	Chunk chunk(chunkCS);
	Chunk* chunkPtr = m_activeChunks.Insert_Move(std::move(chunk));

	uint64_t hashKey = GetHashKeyFromChunkPosition(chunkCS);
	m_chunkMap[hashKey] = chunkPtr;

	m_poolMap[hashKey] = m_activeChunks.GetIndexFromPointer(chunkPtr);

	return chunkPtr;
}

void ChunkManager::UnloadChunk(Chunk* chunk)
{
	///////////////////////////////////////
	///
	///		This is trash :)!!!!
	/// 
	///////////////////////////////////////

	// TODO: 
	// - Consider deleting un-modified chunks from memory (they can be loaded in again through a seed)
	// - Modified chunks should be considered as "inactive" when unloaded, removed from
	//   the m_activeChunks vector and serialized

	for(uint32_t i = 0; i < m_activeChunks.Size(); i++)
	{
		Chunk* currChunk = m_activeChunks[i];
		// For now I will just set the chunk as inactive and remove it from the vector
		if (currChunk == chunk)
		{
			// Erase chunk instance from vector
			UnloadChunk(i);
		}
	}
}


void ChunkManager::UnloadChunk(const uint32_t& index)
{
	// Consider swapping chunk with last one from vector, and ZeroMemory the chunk
	// When all the chunks have been unloaded (i.e. swapped to the last available chunk)
	// all the zero'd chunks will be deleted from the vector
	m_chunkMap.erase(GetHashKeyFromChunkPosition(m_activeChunks[index]->GetPosition()));
	Chunk* chunkPtr = m_activeChunks.Remove(index);
	m_poolMap[GetHashKeyFromChunkPosition(chunkPtr->GetPosition())] = m_activeChunks.GetIndexFromPointer(chunkPtr);
}

const uint16_t ChunkManager::GetNumActiveChunks()
{
	//std::lock_guard<std::mutex> guard(m_canAccessVec);
	return m_activeChunks.Size();
}


Chunk* ChunkManager::GetChunkAtIndex(const uint16_t index)
{
	//std::lock_guard<std::mutex> guard(m_canAccessVec);
	if (index < m_activeChunks.Size())
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
	uint64_t hashKey = GetHashKeyFromChunkPosition(posCS);
	auto val = m_chunkMap.find(hashKey);

	if (val == m_chunkMap.end()) return nullptr;
	else return val->second;
}

SortedPool<Chunk>& ChunkManager::GetChunkPool()
{
	return m_activeChunks;
}

void ChunkManager::UpdaterEntryPoint()
{
	// "Infinite" while loop; only break out if ChunkManager is shut down...
	while(m_runThreads)
	{
		Update();
	}
}

void ChunkManager::SetPlayerPos(DirectX::XMFLOAT3 playerPos) { m_playerPos = playerPos; }

XMFLOAT3 ChunkManager::WorldToChunkSpace(const XMFLOAT3& pos)
{
	XMFLOAT3 convertedPos = { (float)((int32_t)pos.x / CHUNK_SIZE), (float)((int32_t)pos.y / CHUNK_SIZE), (float)((int32_t)pos.z / CHUNK_SIZE) };
	
	// Adjust for negative coordinates
	convertedPos.x = pos.x < 0 ? --convertedPos.x : convertedPos.x;
	convertedPos.y = pos.y < 0 ? --convertedPos.y : convertedPos.y;
	convertedPos.z = pos.z < 0 ? --convertedPos.z : convertedPos.z;
	return convertedPos;
}

XMFLOAT3 ChunkManager::ChunkToWorldSpace(const XMFLOAT3& pos)
{
	// NOTE! pos should contain whole numbers only
	return { pos.x * CHUNK_SIZE, pos.y * CHUNK_SIZE, pos.z * CHUNK_SIZE };
}

void ChunkManager::ResetChunkMemory(const uint16_t index)
{
	//memset(&m_activeChunks[index], 0, sizeof(Chunk));
	m_activeChunks.Remove(index);
}

uint64_t ChunkManager::GetHashKeyFromChunkPosition(const DirectX::XMFLOAT3& chunkPos)
{
	// (z + size) * (size << 1)^2 + (y + size) * (size << 1) + (x + size)
	return (static_cast<double>(chunkPos.z) + CHUNK_SIZE) * DOUBLE_CHUNK_SIZE * DOUBLE_CHUNK_SIZE
		+ (static_cast<double>(chunkPos.y) + CHUNK_SIZE) * DOUBLE_CHUNK_SIZE 
		+ (static_cast<double>(chunkPos.x) + CHUNK_SIZE);
}

bool ChunkManager::CheckForOutOfRangeChunks(const XMFLOAT3& currentPlayerPosCS, const XMFLOAT3& prevPlayerPosCS)
{
	bool chunksWereDeleted = false;

	// prev pos.x = 2 | currpos.x = 4
	// render dist = 2

	// unload chunks in currpos.x - sign * (renderDist + difference) = 5 - 2 - 3 = 0

	if (prevPlayerPosCS.x != currentPlayerPosCS.x)
	{
		int32_t difference = prevPlayerPosCS.x - currentPlayerPosCS.x;
		difference = min(difference, RENDER_DIST);
		int8_t sign = difference > 0 ? 1 : -1;

		// Loop through all the new chunks
		for (int32_t x = 0; x < abs(difference); x++)
		{
			for (int32_t y = -RENDER_DIST; y <= RENDER_DIST; y++)
			{
				for (int32_t z = -RENDER_DIST; z <= RENDER_DIST; z++)
				{
					float newXPos = currentPlayerPosCS.x - sign * (RENDER_DIST + difference);
					XMFLOAT3 chunkPos = { newXPos, static_cast<float>(y), static_cast<float>(z) };
					Chunk* chunkToDelete = m_chunkMap[GetHashKeyFromChunkPosition(chunkPos)];

					if(chunkToDelete)
					{
						m_deletedChunkList.push_back(chunkToDelete->GetPosition());
						chunksWereDeleted = true;
					}
				}
			}
		}
	}

	int chunksDeleted = 0;
	if (prevPlayerPosCS.y != currentPlayerPosCS.y)
	{
		int32_t difference = prevPlayerPosCS.y - currentPlayerPosCS.y;
		difference = min(difference, RENDER_DIST);
		int8_t sign = difference > 0 ? 1 : -1;

		// Loop through all the new chunks
		for (int32_t y = 0; y < abs(difference); y++)
		{
			for (int32_t x = -RENDER_DIST; x < RENDER_DIST; x++)
			{
				for (int32_t z = -RENDER_DIST; z < RENDER_DIST; z++)
				{
					float newYPos = currentPlayerPosCS.y - sign * (RENDER_DIST + difference);
					XMFLOAT3 chunkPos = { static_cast<float>(x), newYPos, static_cast<float>(z) };
					Chunk* chunkToDelete = m_chunkMap[GetHashKeyFromChunkPosition(chunkPos)];

					if(chunkToDelete)
					{
						m_deletedChunkList.push_back(chunkToDelete->GetPosition());
						chunksWereDeleted = true;
						chunksDeleted++;
					}
				}
			}
		}
	}

	if (prevPlayerPosCS.z != currentPlayerPosCS.z)
	{
		int32_t difference = prevPlayerPosCS.z - currentPlayerPosCS.z;
		difference = min(difference, RENDER_DIST);
		int8_t sign = difference > 0 ? 1 : -1;

		// Loop through all the new chunks
		for (int32_t z = 0; z < abs(difference); z++)
		{
			for (int32_t x = -RENDER_DIST; x < RENDER_DIST; x++)
			{
				for (int32_t y = -RENDER_DIST; y < RENDER_DIST; y++)
				{
					float newZPos = currentPlayerPosCS.z - sign * (RENDER_DIST + difference);
					XMFLOAT3 chunkPos = { static_cast<float>(x), static_cast<float>(y), newZPos };
					Chunk* chunkToDelete = m_chunkMap[GetHashKeyFromChunkPosition(chunkPos)];

					if(chunkToDelete)
					{
						m_deletedChunkList.push_back(chunkToDelete->GetPosition());
						chunksWereDeleted = true;
					}
				}
			}
		}
	}

	return chunksWereDeleted;
}

void ChunkManager::InitChunksMultithreaded(const int32_t& startChunk, const int32_t& numChunksToInit, const XMFLOAT3& playerPosCS)
{
	for(int32_t x = startChunk; x < startChunk + numChunksToInit; x++)
	{
		for (int32_t y = -RENDER_DIST; y <= RENDER_DIST; y++)
		{
			for (int32_t z = -RENDER_DIST; z <= RENDER_DIST; z++)
			{
				// A coordinate in chunk space
				XMFLOAT3 newChunkPosCS = { playerPosCS.x + x, playerPosCS.y + y, playerPosCS.z + z };

				LoadChunkMultithreaded(newChunkPosCS);
			}
		}
	}
}

void ChunkManager::InitChunkVertexBuffersMultithreaded(const uint32_t& startIndex, const uint32_t& numChunksToInit)
{
	for(uint32_t index = startIndex; index < startIndex + numChunksToInit; index++)
	{
		// Sanity check
		VX_ASSERT(index < m_activeChunks.Size());

		Chunk* currChunk = m_activeChunks[index];

		// Sanity check
		VX_ASSERT(currChunk);

		m_canAccessVec.lock();
		currChunk->InitializeVertexBuffer();
		m_canAccessVec.unlock();
	}
}

Chunk* ChunkManager::LoadChunkMultithreaded(const DirectX::XMFLOAT3 chunkCS)
{

	uint64_t hashKey = GetHashKeyFromChunkPosition(chunkCS);

	Chunk chunk(chunkCS);

	m_canAccessVec.lock();
	Chunk* chunkPtr = m_activeChunks.Insert_Move(std::move(chunk));
	if (m_chunkMap.size() > 0 && m_chunkMap.find(hashKey) != m_chunkMap.end()) VX_ASSERT(false);
	m_chunkMap[hashKey] = chunkPtr;
	m_poolMap[hashKey] = m_activeChunks.GetIndexFromPointer(chunkPtr);
	m_canAccessVec.unlock();

	return chunkPtr;
}

const bool ChunkManager::IsShuttingDown() { return m_isShuttingDown; }
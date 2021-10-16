#include "../Misc/pch.h"

#include "ChunkManager.h"
#include "../Utility/Utility.h"
#include "../../imgui/imgui.h"
#include "Chunk.h"

#include "../Utility/Noise.h"

using namespace DirectX;

// Static variable definitions
std::vector<std::shared_ptr<Chunk>> ChunkManager::m_activeChunks = std::vector<std::shared_ptr<Chunk>>();
bool ChunkManager::m_runUpdater = true;
XMFLOAT3 ChunkManager::m_playerPos = { 0.0f, 0.0f, 0.0f };
std::vector<XMFLOAT3> ChunkManager::m_newChunkList = std::vector<XMFLOAT3>();
std::vector<uint32_t> ChunkManager::m_deletedChunkList = std::vector<uint32_t>();
std::thread* ChunkManager::m_updaterThread = nullptr;
std::mutex ChunkManager::m_canAccessVec;

#define ALLOW_HARD_CODED_MAX_INIT_THREADS 1
#define USE_DEFAULT_SEED 0
#define USE_SEED_BASED_ON_SYSTEM_TIME 1

constexpr int CHUNK_GENERATION_SEED = 12346;

#if ALLOW_HARD_CODED_MAX_INIT_THREADS == 1
const uint32_t g_maxNumThreadsForInit = 5;
#else
uint32_t g_maxNumThreadsForInit;
#endif


std::unordered_map<uint64_t, std::weak_ptr<Chunk>> ChunkManager::m_chunkMap = std::unordered_map<uint64_t, std::weak_ptr<Chunk>>();


void ChunkManager::Initialize(const XMFLOAT3 playerPosWS)
{
	// Isn't strictly necessary b/c it's inited as a static variable
	m_runUpdater = true;

#if USE_DEFAULT_SEED == 0
#if USE_SEED_BASED_ON_SYSTEM_TIME == 0
	Noise2D::SetSeed(CHUNK_GENERATION_SEED);
#else
	Noise2D::SetSeed(static_cast<int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
#endif // USE_SEED_BASED_ON_SYSTEM_TIME
#endif // USE_DEFAULT_SEED

	m_playerPos = playerPosWS;

	m_activeChunks.reserve(pow(2 * RENDER_DIST + 1, 2));

	XMFLOAT3 playerPosCS = WorldToChunkSpace(playerPosWS);

#if ALLOW_HARD_CODED_MAX_INIT_THREADS == 0
	// hardware_concurrency() will only work for Windows builds...not particularly important
	// right now but should keep this in mind!
	g_maxNumThreadsForInit = std::thread::hardware_concurrency();
#endif

	{
		VX_PROFILE_SCOPE_MSG_MODE("Initial Chunk Loading", 1);

		uint32_t desiredDepthSlicesPerThread, actualDepthSlicesPerThread;
		desiredDepthSlicesPerThread = actualDepthSlicesPerThread = 2;
		uint32_t numThreadsToRun = floor((2 * RENDER_DIST + 1) / desiredDepthSlicesPerThread);

		// If we need more threads to run desired depth slices per thread, cap at map threads and recalculate
		// actualDepthSlicesPerThread to reflect change in numThreadsToRun
		if(numThreadsToRun > g_maxNumThreadsForInit)
		{
			numThreadsToRun = g_maxNumThreadsForInit;
			actualDepthSlicesPerThread = floor((2 * RENDER_DIST + 1) / numThreadsToRun);
		}


		// [MULTI-THREADED]		Load all of the initial chunks
		std::vector<std::thread*> chunkLoaderThreads(numThreadsToRun);
		for (int16_t threadID = 0; threadID < numThreadsToRun; threadID++)
		{
			int32_t startingChunk = threadID * actualDepthSlicesPerThread - RENDER_DIST;
			int32_t numChunksToInit = threadID == numThreadsToRun - 1 ? actualDepthSlicesPerThread + ((2 * RENDER_DIST + 1) - numThreadsToRun * actualDepthSlicesPerThread) : actualDepthSlicesPerThread;
			std::thread* currThread = new std::thread(InitChunksMultithreaded, startingChunk, numChunksToInit, playerPosCS);
			chunkLoaderThreads[threadID] = currThread;
		}

		// Join all initer threads before proceeding
		for (auto thread : chunkLoaderThreads) thread->join();
		chunkLoaderThreads.clear();

		// [MULTI-THREADED]		Initialize all the chunks' vertex buffers
		std::vector<std::thread*> vertexBufferThreads(numThreadsToRun);
		uint32_t numIndicesPerChunk = m_activeChunks.size() / numThreadsToRun;
		for (int16_t threadID = 0; threadID < numThreadsToRun; threadID++)
		{
			uint32_t startingIndex = threadID * numIndicesPerChunk;
			uint32_t numIndiciesToInit = threadID == numThreadsToRun - 1 ? m_activeChunks.size() - startingIndex : numIndicesPerChunk;
			std::thread* currThread = new std::thread(InitChunkVertexBuffersMultithreaded, startingIndex, numIndiciesToInit);
			vertexBufferThreads[threadID] = currThread;
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

	m_chunkMap.clear();
	m_activeChunks.clear();
}

// Method implementations

void ChunkManager::Update()
{
	//VX_PROFILE_FUNC();

	// Chunk coord
	XMFLOAT3 playerPosChunkSpace = WorldToChunkSpace(m_playerPos);


	//
	//	! DEBUG
	//
#pragma region _DEBUG
#define TEST_RATE 100
	for(uint32_t i = 0; i < TEST_RATE; i++)
	{
		const char* newArray = new char[1000];

		// STALL
		for(uint32_t j = 0; j < TEST_RATE * 2; j++)
		{
			// Do something
		}

		delete[] newArray;
	}
#pragma endregion //_DEBUG

	// 1. Unload chunks outside of render distance
	for (uint32_t i = 0; i < m_activeChunks.size(); i++)
	{
		std::shared_ptr<Chunk> chunk = m_activeChunks[i];

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
			m_deletedChunkList.push_back(i);
		}

	}


	// 2. Load chunks if they are inside render distance

	// Z-axis chunk checking (includes corner chunks)
	for (int16_t x = -RENDER_DIST; x <= RENDER_DIST; x += 2 * RENDER_DIST)
	{
		for (int16_t y = -RENDER_DIST; y <= RENDER_DIST; y++)
		{
			for (int16_t z = -RENDER_DIST; z <= RENDER_DIST; z++)
			{
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
		std::shared_ptr<Chunk> newChunk = LoadChunk(m_newChunkList[i]);
		XMFLOAT3 chunkPosCS = newChunk->GetPosition();

		// Left neighbor
		std::shared_ptr<Chunk> leftNeighbor = GetChunkAtPos({ chunkPosCS.x - 1, chunkPosCS.y, chunkPosCS.z });
		if (leftNeighbor) leftNeighbor->InitializeVertexBuffer();

		// Right neighbor
		std::shared_ptr<Chunk> rightNeighbor = GetChunkAtPos({ chunkPosCS.x + 1, chunkPosCS.y, chunkPosCS.z });
		if (rightNeighbor) rightNeighbor->InitializeVertexBuffer();

		// Top neighbor
		std::shared_ptr<Chunk> topNeighbor = GetChunkAtPos({ chunkPosCS.x, chunkPosCS.y + 1, chunkPosCS.z });
		if (topNeighbor) topNeighbor->InitializeVertexBuffer();

		// Bottom neighbor
		std::shared_ptr<Chunk> bottomNeighbor = GetChunkAtPos({ chunkPosCS.x, chunkPosCS.y - 1, chunkPosCS.z });
		if (bottomNeighbor) bottomNeighbor->InitializeVertexBuffer();

		// Front neighbor
		std::shared_ptr<Chunk> frontNeighbor = GetChunkAtPos({ chunkPosCS.x, chunkPosCS.y, chunkPosCS.z - 1 });
		if (frontNeighbor) frontNeighbor->InitializeVertexBuffer();

		// Back neighbor
		std::shared_ptr<Chunk> backNeighbor = GetChunkAtPos({ chunkPosCS.x, chunkPosCS.y, chunkPosCS.z + 1 });
		if (backNeighbor) backNeighbor->InitializeVertexBuffer();

		// Current chunk
		newChunk->InitializeVertexBuffer();
	}
	m_canAccessVec.unlock();

	// Clear the temp new/deleted vectors
	m_newChunkList.clear();
	m_deletedChunkList.clear();
}

std::shared_ptr<Chunk> ChunkManager::LoadChunk(const XMFLOAT3 chunkCS) 
{
	std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>(chunkCS);
	m_activeChunks.emplace_back(chunk);

	uint64_t hashKey = GetHashKeyFromChunkPosition(chunkCS);
	m_chunkMap[hashKey] = static_cast<std::weak_ptr<Chunk>>(chunk);

	return chunk;
}

void ChunkManager::UnloadChunk(std::shared_ptr<Chunk> chunk)
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
	uint16_t index = 0;
	for(auto& iter : m_activeChunks)
	{
		// For now I will just set the chunk as inactive and remove it from the vector
		if (iter == chunk)
		{
			// Erase chunk instance from vector
			UnloadChunk(index++);
		}
	}
}


void ChunkManager::UnloadChunk(const uint16_t& index)
{
	m_chunkMap.erase(GetHashKeyFromChunkPosition(m_activeChunks[index]->GetPosition()));
	m_activeChunks.erase(m_activeChunks.begin() + index);
}

const uint16_t ChunkManager::GetNumActiveChunks()
{
	//std::lock_guard<std::mutex> guard(m_canAccessVec);
	return m_activeChunks.size();
}


std::shared_ptr<Chunk> ChunkManager::GetChunkAtIndex(const uint16_t index)
{
	//std::lock_guard<std::mutex> guard(m_canAccessVec);
	if (index < m_activeChunks.size())
	{
		return m_activeChunks[index];
	}
	else
	{
		return nullptr;
	}
}

std::shared_ptr<Chunk> ChunkManager::GetChunkAtPos(const DirectX::XMFLOAT3 posCS)
{
	uint64_t hashKey = GetHashKeyFromChunkPosition(posCS);
	auto val = m_chunkMap.find(hashKey);

	if (val == m_chunkMap.end()) return nullptr;
	else return static_cast<std::shared_ptr<Chunk>>(val->second);
}

std::vector<std::shared_ptr<Chunk>> ChunkManager::GetChunkVector()
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
	memset(&m_activeChunks[index], 0, sizeof(Chunk));
}

uint64_t ChunkManager::GetHashKeyFromChunkPosition(const DirectX::XMFLOAT3& chunkPos)
{
	// (z + size) * (size << 1)^2 + (y + size) * (size << 1) + (x + size)
	return (static_cast<double>(chunkPos.z) + CHUNK_SIZE) * DOUBLE_CHUNK_SIZE * DOUBLE_CHUNK_SIZE
		+ (static_cast<double>(chunkPos.y) + CHUNK_SIZE) * DOUBLE_CHUNK_SIZE 
		+ (static_cast<double>(chunkPos.x) + CHUNK_SIZE);
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
		VX_ASSERT(index < m_activeChunks.size());

		std::shared_ptr<Chunk> currChunk = m_activeChunks[index];

		// Sanity check
		VX_ASSERT(currChunk);

		currChunk->InitializeVertexBuffer();
	}
}

std::shared_ptr<Chunk> ChunkManager::LoadChunkMultithreaded(const DirectX::XMFLOAT3 chunkCS)
{
	std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>(chunkCS);
	uint64_t hashKey = GetHashKeyFromChunkPosition(chunkCS);

	m_canAccessVec.lock();
	m_activeChunks.emplace_back(chunk);

	if (m_chunkMap.size() > 0 && m_chunkMap.find(hashKey) != m_chunkMap.end()) VX_ASSERT(false);
	//VX_ASSERT(m_chunkMap.find(hashKey) != m_chunkMap.end() && m_chunkMap.size() > 0);
	m_chunkMap[hashKey] = static_cast<std::weak_ptr<Chunk>>(chunk);
	m_canAccessVec.unlock();

	return chunk;
}

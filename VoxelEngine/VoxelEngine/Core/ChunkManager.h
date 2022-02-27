#ifndef _CHUNKMANAGER_H
#define _CHUNKMANAGER_H

#include <vector>
#include <thread>
#include <mutex>
#include <unordered_map>

#include "../Utility/SortedPool.h"

#include "Chunk.h"

constexpr int32_t RENDER_DIST = 5;


// This is a work in progress!!
class ChunkManager
{
public:

	static void Initialize(const DirectX::XMFLOAT3 playerPosWS);

	static void Shutdown();

	static Chunk* LoadChunk(const DirectX::XMFLOAT3 chunkCS);

	static void UnloadChunk(Chunk* chunk);
	static void UnloadChunk(const uint32_t& index);

	static const uint16_t GetNumActiveChunks();

	static Chunk* GetChunkAtIndex(const uint16_t index);

	// Returns chunk at "pos" CHUNK SPACE
	static Chunk* GetChunkAtPos(const DirectX::XMFLOAT3 pos);

	static SortedPool<Chunk>& GetChunkPool();

	static void UpdaterEntryPoint();

	static void SetPlayerPos(DirectX::XMFLOAT3 playerPos);

	// MULTI-THREADED METHODS
	static void InitChunksMultithreaded(const int32_t &startChunk, const int32_t& numChunksToInit, const DirectX::XMFLOAT3& playerPosCS);

	static void InitChunkVertexBuffersMultithreaded(const uint32_t& startIndex, const uint32_t& numChunksToInit);

	static Chunk* LoadChunkMultithreaded(const DirectX::XMFLOAT3 chunkCS);

	static const bool IsShuttingDown();

private:

	static void Update();

	static void ResetChunkMemory(const uint16_t index);

	// CHECKFLAG:
	// 0 - deleting
	// 1 - creating
	static const int32_t CheckForChunksToLoadOrUnload(const DirectX::XMFLOAT3& currentPlayerPosCS, const DirectX::XMFLOAT3& prevPlayerPosCS, const int checkFlag);

private:
	
	// Stores active chunks in CHUNK SPACE
	static SortedPool<Chunk> m_activeChunks;

	// Dictates whether the updater thread is free to run Update()
	static std::mutex m_canAccessVec;
	static std::thread* m_updaterThread;
	static bool m_runThreads;

	static std::vector<DirectX::XMFLOAT3> m_newChunkList;
	static std::vector<DirectX::XMFLOAT3> m_deletedChunkList;

	static DirectX::XMFLOAT3 m_playerPos;

	// Speeds up position lookup for Chunk*'s
	static std::unordered_map<uint64_t, Chunk*> m_chunkMap;
	static std::unordered_map<uint64_t, uint32_t> m_poolMap;

	static bool m_isShuttingDown;

};

#endif
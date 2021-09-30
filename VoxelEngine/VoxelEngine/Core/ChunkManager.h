#ifndef _CHUNKMANAGER_H
#define _CHUNKMANAGER_H

#include <vector>
#include <thread>
#include <mutex>

#include "Chunk.h"


// This is a work in progress!!
class ChunkManager
{
public:

	static void Initialize(const DirectX::XMFLOAT3 playerPosWS);

	static void Shutdown();

	static Chunk* LoadChunk(const DirectX::XMFLOAT3 chunkCS);

	static void UnloadChunk(Chunk* chunk);
	static void UnloadChunk(const uint16_t& index);

	static const uint16_t GetNumActiveChunks();

	static Chunk* GetChunkAtIndex(const uint16_t index);

	// Returns chunk at "pos" CHUNK SPACE
	static Chunk* GetChunkAtPos(const DirectX::XMFLOAT3 pos);

	// For EXTERNAL use only, don't use this method internally!!
	static std::vector<Chunk*>& GetChunkVector();

	static void UpdaterEntryPoint();

	static void SetPlayerPos(DirectX::XMFLOAT3 playerPos);

	static void CheckOutChunkVector();

	static void ReturnChunkVector();

	// Helper methods. Consider moving to a Utility library
	static DirectX::XMFLOAT3 WorldToChunkSpace(const DirectX::XMFLOAT3& pos);
	static DirectX::XMFLOAT3 ChunkToWorldSpace(const DirectX::XMFLOAT3& pos);

private:

	static void Update();

	static void ResetChunkMemory(const uint16_t index);

private:
	
	// Stores active chunks in CHUNK SPACE
	static std::vector<Chunk*> m_activeChunks;

	// NOTE! Temporarily stored here
	// Consider moving to another "settings" or "game" class
	static uint16_t m_renderDist;

	// Dictates whether the updater thread is free to run Update()
	static std::mutex m_canAccessVec;
	static std::thread* m_updaterThread;
	static bool m_runUpdater;

	static std::vector<DirectX::XMFLOAT3> m_newChunkList;
	static std::vector<uint32_t> m_deletedChunkList;

	static DirectX::XMFLOAT3 m_playerPos;

};

#endif
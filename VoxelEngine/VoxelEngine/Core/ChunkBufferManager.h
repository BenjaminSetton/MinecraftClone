#ifndef _CHUNKBUFFERMANAGER_H
#define _CHUNKBUFFERMANAGER_H

#include <d3d11.h>
#include <DirectXMath.h>

// This class is responsible for managing buffers
// used for rendering by shader classes
class ChunkBufferManager
{
public:

	// The ChunkManager has to add/remove chunks, so the appropriate
	// vectors have to be filled in here
	friend class ChunkManager;

	static void Initialize();
	static void Shutdown();

	static void UpdateBuffers();

private:

	static std::vector<DirectX::XMFLOAT3> m_blockPositions;

	// Uses instancing
	static std::unique_ptr<ID3D11Buffer> m_blockVertexBuffer;
	static std::unique_ptr<ID3D11Buffer> m_blockPositionBuffer;

	static std::vector<std::shared_ptr<Chunk>> m_chunksToRemove;
	static std::vector<std::shared_ptr<Chunk>> m_chunksToAdd;

};

#endif
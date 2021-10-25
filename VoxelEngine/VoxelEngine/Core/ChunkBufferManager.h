#ifndef _CHUNKBUFFERMANAGER_H
#define _CHUNKBUFFERMANAGER_H

#include <d3d11.h>
#include <DirectXMath.h>

class Chunk;

// This class is responsible for managing buffers
// used for rendering by shader classes
class ChunkBufferManager
{
public:

	// The Chunk class has to add/remove chunk positions, so the appropriate
	// vectors have to be filled in here

	static void Initialize();
	static void Shutdown();

	static void UpdateBuffers();

	static ID3D11Buffer* GetVertexBuffer();
	static ID3D11Buffer* GetInstanceBuffer();

	static std::vector<BlockInstanceData>& GetVertexArray();

private: 

	static std::vector<BlockInstanceData> m_vertices;

	static ID3D11Buffer* m_blockVertexBuffer;
	static ID3D11Buffer* m_blockInstanceBuffer;

};

#endif
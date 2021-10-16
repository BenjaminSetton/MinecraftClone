#include "../Misc/pch.h"

#include "ChunkBufferManager.h"
#include "D3D.h"
#include "../Utility/Utility.h"

#include "ChunkManager.h"

constexpr int32_t NUM_BLOCK_POSITIONS = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * (RENDER_DIST + 1) * (RENDER_DIST + 1) * (RENDER_DIST + 1);

std::unique_ptr<ID3D11Buffer> ChunkBufferManager::m_blockVertexBuffer = nullptr;
std::unique_ptr<ID3D11Buffer> ChunkBufferManager::m_blockPositionBuffer = nullptr;

std::vector<std::shared_ptr<Chunk>> ChunkBufferManager::m_chunksToRemove = std::vector<std::shared_ptr<Chunk>>();
std::vector<std::shared_ptr<Chunk>> ChunkBufferManager::m_chunksToAdd = std::vector<std::shared_ptr<Chunk>>();

void ChunkBufferManager::Initialize()
{
	HRESULT hr;

	// Create the block vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = ARRAYSIZE(verts) * sizeof(BlockVertex);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	vertexBufferData.pSysMem = verts;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	ID3D11Buffer* pBuffer = m_blockVertexBuffer.get();
	hr = D3D::GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &pBuffer);
	VX_ASSERT(!FAILED(hr));


	// Create the block position vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = NUM_BLOCK_POSITIONS;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	pBuffer = m_blockPositionBuffer.get();
	hr = D3D::GetDevice()->CreateBuffer(&vertexBufferDesc, nullptr, &pBuffer);
	VX_ASSERT(!FAILED(hr));
}

void ChunkBufferManager::Shutdown()
{
	if (m_blockVertexBuffer.get()) m_blockVertexBuffer.get()->Release();

	if(m_blockPositionBuffer.get()) m_blockPositionBuffer.get()->Release();
}

void ChunkBufferManager::UpdateBuffers()
{
	// Remove deleted chunks from vector
	for(auto chunk : m_chunksToRemove)
	{
		m_blockPositions[chunk->]
	}
	// Add new chunks to vector
	// Clear added/deleted chunks vector
	// Update the dynamic vertex buffer

	// Use map/unmap for dynamic vertex buffers
	VX_ASSERT(m_blockPositions.size() < NUM_BLOCK_POSITIONS);

}
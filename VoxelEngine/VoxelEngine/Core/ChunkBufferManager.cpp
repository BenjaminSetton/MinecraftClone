#include "../Misc/pch.h"

#include "ChunkBufferManager.h"
#include "D3D.h"
#include "../Utility/Utility.h"

#include "Chunk.h"					// for CHUNK_SIZE
#include "ChunkManager.h"			// for RENDER_DIST

using namespace DirectX;

constexpr int32_t NUM_BLOCK_VERTS = (6 * 6 * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * (RENDER_DIST + 1) * (RENDER_DIST + 1) * (RENDER_DIST + 1)) * 0.1f;

std::vector<BlockVertex> ChunkBufferManager::m_vertices = std::vector<BlockVertex>();

ID3D11Buffer* ChunkBufferManager::m_blockVertexBuffer = nullptr;

void ChunkBufferManager::Initialize()
{
	HRESULT hr;

	// Create the block vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = NUM_BLOCK_VERTS * sizeof(BlockVertex);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//ID3D11Buffer* pBuffer = m_blockVertexBuffer.get();
	hr = D3D::GetDevice()->CreateBuffer(&vertexBufferDesc, nullptr, &m_blockVertexBuffer);
	VX_ASSERT(!FAILED(hr));
}

void ChunkBufferManager::Shutdown()
{
	if (m_blockVertexBuffer) m_blockVertexBuffer->Release();
	m_vertices.clear();
}

void ChunkBufferManager::UpdateBuffers()
{
	if (m_vertices.size() <= 0) return;

	VX_ASSERT(m_vertices.size() < NUM_BLOCK_VERTS);

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Update the dynamic vertex buffer
	D3D::GetDeviceContext()->Map(m_blockVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	{
		VX_PROFILE_SCOPE("[UPDATE] Updating mapped resource");
		int64_t numBytes = (int64_t)sizeof(BlockVertex) * m_vertices.size();
		ImGui::Begin("Timing Panel");
		ImGui::Text("Number of bytes copied: %i", numBytes);
		ImGui::End();
		memcpy(mappedResource.pData, &m_vertices[0], numBytes);
	}
	D3D::GetDeviceContext()->Unmap(m_blockVertexBuffer, 0);

	

}

ID3D11Buffer* ChunkBufferManager::GetVertexBuffer() { return m_blockVertexBuffer; }

std::vector<BlockVertex>& ChunkBufferManager::GetVertexArray() { return m_vertices; }
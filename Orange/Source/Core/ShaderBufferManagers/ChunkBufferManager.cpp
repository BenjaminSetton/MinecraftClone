#include "../../Misc/pch.h"

#include "ChunkBufferManager.h"
#include "../D3D.h"
#include "../../Utility/Utility.h"

#include "../Chunk.h"					// for CHUNK_SIZE
#include "../ChunkManager.h"			// for RENDER_DIST

using namespace DirectX;

constexpr int32_t NUM_BLOCK_VERTS = static_cast<int32_t>((CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * (RENDER_DIST + 1) * (RENDER_DIST + 1) * (RENDER_DIST + 1)) * 0.75f);

std::vector<BlockInstanceData> ChunkBufferManager::m_vertices = std::vector<BlockInstanceData>();

ID3D11Buffer* ChunkBufferManager::m_blockVertexBuffer = nullptr;
ID3D11Buffer* ChunkBufferManager::m_blockInstanceBuffer = nullptr;

void ChunkBufferManager::Initialize()
{
	HRESULT hr;

	// Create the block vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = ARRAYSIZE(verts) * sizeof(decltype(*verts));
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	vertexBufferData.pSysMem = verts;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	hr = D3D::GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_blockVertexBuffer);
	OG_ASSERT(!FAILED(hr));


	// Create the block instance buffer
	D3D11_BUFFER_DESC instanceBufferDesc;
	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = NUM_BLOCK_VERTS * sizeof(BlockInstanceData);
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	hr = D3D::GetDevice()->CreateBuffer(&instanceBufferDesc, nullptr, &m_blockInstanceBuffer);
	OG_ASSERT(!FAILED(hr));
}

void ChunkBufferManager::Shutdown()
{
	if (m_blockVertexBuffer)
	{
		m_blockVertexBuffer->Release();
		m_blockVertexBuffer = nullptr;
	}

	if (m_blockInstanceBuffer) 
	{
		m_blockInstanceBuffer->Release();
		m_blockInstanceBuffer = nullptr;
	}

	m_vertices.clear();
}

void ChunkBufferManager::UpdateBuffers()
{
	if (m_vertices.size() <= 0) return;

	OG_ASSERT(m_vertices.size() < NUM_BLOCK_VERTS);

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Update the dynamic vertex buffer
	if(m_vertices.size() > 0)
	{
		D3D::GetDeviceContext()->Map(m_blockInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		{
			OG_PROFILE_SCOPE("[UPDATE] Updating mapped resource");
			int64_t numBytes = (int64_t)sizeof(decltype(m_vertices[0])) * m_vertices.size();
			//ImGui::Begin("Timing Panel");
			//ImGui::Text("Number of bytes copied: %i", numBytes);
			//ImGui::End();
			memcpy(mappedResource.pData, &m_vertices[0], numBytes);
		}
		D3D::GetDeviceContext()->Unmap(m_blockInstanceBuffer, 0);
	}

	

}

ID3D11Buffer* ChunkBufferManager::GetVertexBuffer() { return m_blockVertexBuffer; }

ID3D11Buffer* ChunkBufferManager::GetInstanceBuffer() { return m_blockInstanceBuffer; }

std::vector<BlockInstanceData>& ChunkBufferManager::GetVertexArray() { return m_vertices; }
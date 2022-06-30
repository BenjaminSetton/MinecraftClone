#include "../../Misc/pch.h"

#include "QuadNDCBufferManager.h"
#include "../D3D.h"
#include "../../Utility/Utility.h"

using namespace DirectX;

std::vector<QuadNDCVertexData> QuadNDCBufferManager::m_vertexData = std::vector<QuadNDCVertexData>();
ID3D11Buffer* QuadNDCBufferManager::m_vertexBuffer = nullptr;

static const int NUM_QUAD_VERTICES = 1000;

void QuadNDCBufferManager::Initialize()
{
	HRESULT hr;

	D3D11_BUFFER_DESC instanceBufferDesc;
	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = NUM_QUAD_VERTICES * sizeof(QuadNDCVertexData);
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	hr = D3D::GetDevice()->CreateBuffer(&instanceBufferDesc, nullptr, &m_vertexBuffer);
	OG_ASSERT(!FAILED(hr));
}

void QuadNDCBufferManager::Shutdown()
{
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	m_vertexData.clear();
}

void QuadNDCBufferManager::UpdateBuffers()
{
	if (m_vertexData.size() <= 0) return;

	OG_ASSERT(m_vertexData.size() < NUM_QUAD_VERTICES);

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Update the dynamic vertex buffer
	if (m_vertexData.size() > 0)
	{
		D3D::GetDeviceContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		{
			int64_t numBytes = (int64_t)sizeof(decltype(m_vertexData[0])) * m_vertexData.size();
			ImGui::Begin("Timing Panel");
			ImGui::Text("Number of bytes copied: %i", numBytes);
			ImGui::End();
			memcpy(mappedResource.pData, &m_vertexData[0], numBytes);
		}
		D3D::GetDeviceContext()->Unmap(m_vertexBuffer, 0);
	}



}

void QuadNDCBufferManager::Clear() { m_vertexData.clear(); }

ID3D11Buffer* QuadNDCBufferManager::GetVertexBuffer() { return m_vertexBuffer; }

std::vector<QuadNDCVertexData>& QuadNDCBufferManager::GetVertexData() { return m_vertexData; }

void QuadNDCBufferManager::PushVertex(const QuadNDCVertexData& quadData) { m_vertexData.push_back(quadData); }
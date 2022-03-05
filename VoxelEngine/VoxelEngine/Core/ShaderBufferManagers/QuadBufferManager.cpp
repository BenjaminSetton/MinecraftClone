#include "../../Misc/pch.h"

#include "QuadBufferManager.h"
#include "../D3D.h"
#include "../../Utility/Utility.h"

using namespace DirectX;

std::vector<QuadInstanceData> QuadBufferManager::m_instanceData = std::vector<QuadInstanceData>();

ID3D11Buffer* QuadBufferManager::m_vertexBuffer = nullptr;
ID3D11Buffer* QuadBufferManager::m_instanceBuffer = nullptr;


static const QuadVertexData quadData[6] =
{
	{ XMFLOAT3(-0.5f, 0.5f, 0.0f),	XMFLOAT3(0, 0, -1.0f), XMFLOAT2(0.0f, 0.0f) }, // TL
	{ XMFLOAT3(0.5f, 0.5f, 0.0f),	XMFLOAT3(0, 0, -1.0f), XMFLOAT2(1.0f, 0.0f) }, // TR
	{ XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT3(0, 0, -1.0f), XMFLOAT2(0.0f, 1.0f) }, // BL

	{ XMFLOAT3(0.5f, 0.5f, 0.0f),	XMFLOAT3(0, 0, -1.0f), XMFLOAT2(1.0f, 0.0f) }, // TR
	{ XMFLOAT3(0.5f, -0.5f, 0.0f),	XMFLOAT3(0, 0, -1.0f), XMFLOAT2(1.0f, 1.0f) }, // BR
	{ XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT3(0, 0, -1.0f), XMFLOAT2(0.0f, 1.0f) }, // BL
};

static const int NUM_QUAD_INSTANCES = 100;

void QuadBufferManager::Initialize()
{
	HRESULT hr;

	// Create the block vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = ARRAYSIZE(quadData) * sizeof(decltype(*quadData));
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	vertexBufferData.pSysMem = quadData;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	hr = D3D::GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer);
	VX_ASSERT(!FAILED(hr));


	// Create the block instance buffer
	D3D11_BUFFER_DESC instanceBufferDesc;
	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = NUM_QUAD_INSTANCES * sizeof(BlockInstanceData);
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	hr = D3D::GetDevice()->CreateBuffer(&instanceBufferDesc, nullptr, &m_instanceBuffer);
	VX_ASSERT(!FAILED(hr));
}

void QuadBufferManager::Shutdown()
{
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	if (m_instanceBuffer)
	{
		m_instanceBuffer->Release();
		m_instanceBuffer = nullptr;
	}

	m_instanceData.clear();
}

void QuadBufferManager::UpdateBuffers()
{
	if (m_instanceData.size() <= 0) return;

	VX_ASSERT(m_instanceData.size() < NUM_QUAD_INSTANCES);

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Update the dynamic vertex buffer
	if (m_instanceData.size() > 0)
	{
		D3D::GetDeviceContext()->Map(m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		{
			int64_t numBytes = (int64_t)sizeof(decltype(m_instanceData[0])) * m_instanceData.size();
			ImGui::Begin("Timing Panel");
			ImGui::Text("Number of bytes copied: %i", numBytes);
			ImGui::End();
			memcpy(mappedResource.pData, &m_instanceData[0], numBytes);
		}
		D3D::GetDeviceContext()->Unmap(m_instanceBuffer, 0);
	}



}

void QuadBufferManager::Clear() { m_instanceData.clear(); }

ID3D11Buffer* QuadBufferManager::GetVertexBuffer() { return m_vertexBuffer; }

ID3D11Buffer* QuadBufferManager::GetInstanceBuffer() { return m_instanceBuffer; }

std::vector<QuadInstanceData>& QuadBufferManager::GetInstanceData() { return m_instanceData; }

void QuadBufferManager::PushQuad(const QuadInstanceData& quadData) 
{
	QuadInstanceData transposeTransform = quadData;
	transposeTransform.transform = XMMatrixTranspose(transposeTransform.transform);
	m_instanceData.push_back(transposeTransform);
}
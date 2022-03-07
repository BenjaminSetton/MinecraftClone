#include "../Misc/pch.h"
#include "DefaultBlockShader.h"
#include "D3D.h"

#include "../Utility/Utility.h"
#include "DayNightCycle.h"

// ImGui Debug
#include "../Utility/ImGuiLayer.h"

#include "Block.h"
#include "BlockUVs.h"
#include "../Core/ShaderBufferManagers/ChunkBufferManager.h"

using namespace DirectX;

void DefaultBlockShader::CreateObjects(const WCHAR* vsFilename, const WCHAR* gsFilename, const WCHAR* psFilename) 
{
	ID3D11Device* device = D3D::GetDevice();

	// Create the shaders
	CreateShaders(vsFilename, gsFilename, psFilename);

	// Create the rest of the objects
	CreateD3DObjects();
}

void DefaultBlockShader::Initialize(XMMATRIX camViewMatrix, XMMATRIX lightViewMatrix)
{
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* matrixBufferPtr;
	LightBuffer* lightBufferPtr;

	m_camPM = D3D::GetProjectionMatrix();
	m_lightPM = D3D::GetOrthoMatrix();

#pragma region WVP_MATRICES
	// Lock the matrix constant buffer so it can be written to.
	hr = context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VX_ASSERT(!FAILED(hr));
	// Get a pointer to the data in the constant buffer.
	matrixBufferPtr = (MatrixBuffer*)mappedResource.pData;
	// Copy the matrices into the constant buffer.
	matrixBufferPtr->worldMatrix = XMMatrixIdentity();
	matrixBufferPtr->viewMatrix = XMMatrixTranspose(camViewMatrix);
	matrixBufferPtr->projectionMatrix = XMMatrixTranspose(m_camPM);
	matrixBufferPtr->lightViewMatrix = XMMatrixTranspose(camViewMatrix);
	matrixBufferPtr->lightProjectionMatrix = XMMatrixTranspose(m_lightPM);
	// Unlock the matrix constant buffer.
	context->Unmap(m_matrixBuffer, 0);
#pragma endregion

#pragma region LIGHT_MATRIX
	// Lock the matrix constant buffer so it can be written to.
	hr = context->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VX_ASSERT(!FAILED(hr));
	// Get a pointer to the data in the constant buffer.
	lightBufferPtr = (LightBuffer*)mappedResource.pData;
	// Copy the matrices into the constant buffer.
	XMFLOAT3 sunDir = DayNightCycle::GetLightDirection(DayNightCycle::CelestialBody::SUN);
	lightBufferPtr->lightDir[0] = { sunDir.x, sunDir.y, sunDir.z, 0.0f };
	XMFLOAT3 moonDir = DayNightCycle::GetLightDirection(DayNightCycle::CelestialBody::MOON);
	lightBufferPtr->lightDir[1] = { moonDir.x, moonDir.y, moonDir.z, 0.0f };
	lightBufferPtr->lightCol[0] = DayNightCycle::GetLightColor(DayNightCycle::CelestialBody::SUN);
	lightBufferPtr->lightCol[1] = DayNightCycle::GetLightColor(DayNightCycle::CelestialBody::MOON);
	lightBufferPtr->lightAmbient[0].x = DayNightCycle::GetLightAmbient(DayNightCycle::CelestialBody::SUN);
	lightBufferPtr->lightAmbient[1].x = DayNightCycle::GetLightAmbient(DayNightCycle::CelestialBody::MOON);
	// Unlock the matrix constant buffer.
	context->Unmap(m_lightBuffer, 0);
#pragma endregion

}

void DefaultBlockShader::Render(ID3D11ShaderResourceView* const* srvs)
{
	BlockShader_Data::debugVerts = 0;
	BlockShader_Data::numDrawCalls = 0;

	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	BindObjects(srvs);

	BindVertexBuffers();

	uint32_t size = static_cast<uint32_t>(ChunkBufferManager::GetVertexArray().size());
	BlockShader_Data::debugVerts = size;
	BlockShader_Data::numDrawCalls = 1;

	context->DrawInstanced(ARRAYSIZE(verts), size, 0, 0);
}

void DefaultBlockShader::Shutdown()
{
	if (m_samplerWrap)
	{
		m_samplerWrap->Release();
		m_samplerWrap = nullptr;
	}

	if (m_samplerClamp)
	{
		m_samplerClamp->Release();
		m_samplerClamp = nullptr;
	}

	if (m_UVBuffer)
	{
		m_UVBuffer->Release();
		m_UVBuffer = nullptr;
	}

	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = nullptr;
	}

	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	if (m_inputLayout)
	{
		m_inputLayout->Release();
		m_inputLayout = nullptr;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	if(m_geometryShader)
	{
		m_geometryShader->Release();
		m_geometryShader = nullptr;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}

}

void DefaultBlockShader::CreateD3DObjects()
{
	ID3D11Device* device = D3D::GetDevice();

	HRESULT hr;

	// Setup the description of the matrix dynamic constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the matrix constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	VX_ASSERT(!FAILED(hr));

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBuffer);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the light constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	VX_ASSERT(!FAILED(hr));

	// Setup the description of the UV constant buffer
	D3D11_BUFFER_DESC uvBufferDesc;
	uvBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	uvBufferDesc.ByteWidth = (36 * sizeof(DirectX::XMFLOAT4)) * NUM_BLOCKS;
	uvBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	uvBufferDesc.CPUAccessFlags = 0;
	uvBufferDesc.MiscFlags = 0;
	uvBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA uvBufferData;
	uvBufferData.pSysMem = uvs;
	uvBufferData.SysMemPitch = 0;
	uvBufferData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&uvBufferDesc, &uvBufferData, &m_UVBuffer);
	VX_ASSERT(!FAILED(hr));

	// Create a wrap texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	hr = device->CreateSamplerState(&samplerDesc, &m_samplerWrap);
	VX_ASSERT(!FAILED(hr));

	// Create a clamp texture sampler state description.
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	hr = device->CreateSamplerState(&samplerDesc, &m_samplerClamp);
	VX_ASSERT(!FAILED(hr));
	
}

void DefaultBlockShader::CreateShaders(const WCHAR* vsFilename, const WCHAR* gsFilename, const WCHAR* psFilename)
{
	ID3D11Device* device = D3D::GetDevice();

	HRESULT hr;
	ID3D10Blob* VSBlob;
	ID3D10Blob* GSBlob;
	ID3D10Blob* PSBlob;

	// Compile the vertex shader
	hr = D3DCompileFromFile(vsFilename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS
#ifdef _DEBUG 
		| D3DCOMPILE_DEBUG
#endif
		, 0, &VSBlob, nullptr);
	VX_ASSERT(!FAILED(hr));

	// Compile the geometry shader
	hr = D3DCompileFromFile(gsFilename, nullptr, nullptr, "main", "gs_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS 
#ifdef _DEBUG 
		| D3DCOMPILE_DEBUG
#endif
		, 0, &GSBlob, nullptr);
	VX_ASSERT(!FAILED(hr));

	// Compile the pixel shader
	hr = D3DCompileFromFile(psFilename, nullptr, nullptr, "main", "ps_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS 
#ifdef _DEBUG 
		| D3DCOMPILE_DEBUG
#endif
		, 0, &PSBlob, nullptr);
	VX_ASSERT(!FAILED(hr));

	// Create the shaders
	hr = device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, &m_vertexShader);
	VX_ASSERT(!FAILED(hr));
	hr = device->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), nullptr, &m_geometryShader);
	VX_ASSERT(!FAILED(hr));
	hr = device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, &m_pixelShader);
	VX_ASSERT(!FAILED(hr));

	// Create the input element description
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		// Per-vertex data
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ID", 0, DXGI_FORMAT_R32_UINT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		// Per-instance data
		{ "WORLDPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "BLOCKTYPE", 0, DXGI_FORMAT_R32_UINT, 1, 12, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "BLOCKFACES", 0, DXGI_FORMAT_R32_UINT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	// Create the vertex input layout.
	hr = device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(), &m_inputLayout);
	VX_ASSERT(!FAILED(hr));

	// Release the blobs as they are not needed anymore
	VSBlob->Release();
	GSBlob->Release();
	PSBlob->Release();
	VSBlob = nullptr;
	GSBlob = nullptr;
	PSBlob = nullptr;
}

void DefaultBlockShader::BindVertexBuffers()
{
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	ID3D11Buffer* buffers[] = { ChunkBufferManager::GetVertexBuffer(), ChunkBufferManager::GetInstanceBuffer() };
	unsigned int stride[] = { sizeof(BlockVertexData), sizeof(BlockInstanceData) };
	unsigned int offset[] = { 0, 0 };
	context->IASetVertexBuffers(0, ARRAYSIZE(buffers), buffers, stride, offset);
}

void DefaultBlockShader::BindObjects(ID3D11ShaderResourceView* const* srvs)
{
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	// Set the back buffer and the depth buffer
	ID3D11RenderTargetView* backBuffer = D3D::GetBackBuffer();
	context->OMSetDepthStencilState(D3D::GetDepthStencilState(), 1);
	context->OMSetRenderTargets(1, &backBuffer, D3D::GetDepthStencilView());

	// Now set the matrix constant buffer in the vertex shader with the updated values.
	ID3D11Buffer* buffers[] = { m_UVBuffer, m_matrixBuffer };
	context->VSSetConstantBuffers(0, ARRAYSIZE(buffers), buffers);

	// Set the light constant buffer in the pixel shader
	context->PSSetConstantBuffers(0, 1, &m_lightBuffer);

	// Bind the block texture and shadow map
	context->PSSetShaderResources(0, 2, srvs);

	// Set the vertex input layout.
	context->IASetInputLayout(m_inputLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	context->VSSetShader(m_vertexShader, nullptr, 0);
	context->GSSetShader(m_geometryShader, nullptr, 0);
	context->PSSetShader(m_pixelShader, nullptr, 0);

	// Set the sampler state in the pixel shader.
	ID3D11SamplerState* samplers[] = { m_samplerWrap, m_samplerClamp };
	context->PSSetSamplers(0, ARRAYSIZE(samplers), samplers);

	// Set the blend state
	context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void DefaultBlockShader::UpdateViewMatrices(DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX lightViewMatrix)
{
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* matrixBufferPtr;

	// Lock the matrix constant buffer so it can be written to.
	hr = context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VX_ASSERT(!FAILED(hr));

	matrixBufferPtr = (MatrixBuffer*)mappedResource.pData;
	matrixBufferPtr->worldMatrix = XMMatrixIdentity();
	matrixBufferPtr->viewMatrix = XMMatrixTranspose(viewMatrix);
	matrixBufferPtr->projectionMatrix = XMMatrixTranspose(m_camPM);
	matrixBufferPtr->lightViewMatrix = XMMatrixTranspose(lightViewMatrix);
	matrixBufferPtr->lightProjectionMatrix = XMMatrixTranspose(m_lightPM);

	context->Unmap(m_matrixBuffer, 0);
}

void DefaultBlockShader::UpdateLightMatrix()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	LightBuffer* lightBufferPtr;
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	XMFLOAT3 lightDir = DayNightCycle::GetLightDirection(DayNightCycle::CelestialBody::SUN);
	XMFLOAT4 lightColor = DayNightCycle::GetLightColor(DayNightCycle::CelestialBody::SUN);

	// Lock the matrix constant buffer so it can be written to.
	HRESULT hr = context->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VX_ASSERT(!FAILED(hr));
	// Get a pointer to the data in the constant buffer.
	lightBufferPtr = (LightBuffer*)mappedResource.pData;
	// Copy the matrices into the constant buffer.
	XMFLOAT3 sunDir = DayNightCycle::GetLightDirection(DayNightCycle::CelestialBody::SUN);
	lightBufferPtr->lightDir[0] = { sunDir.x, sunDir.y, sunDir.z, 0.0f };
	XMFLOAT3 moonDir = DayNightCycle::GetLightDirection(DayNightCycle::CelestialBody::MOON);
	lightBufferPtr->lightDir[1] = { moonDir.x, moonDir.y, moonDir.z, 0.0f };
	lightBufferPtr->lightCol[0] = DayNightCycle::GetLightColor(DayNightCycle::CelestialBody::SUN);
	lightBufferPtr->lightCol[1] = DayNightCycle::GetLightColor(DayNightCycle::CelestialBody::MOON);
	lightBufferPtr->lightAmbient[0].x = DayNightCycle::GetLightAmbient(DayNightCycle::CelestialBody::SUN);
	lightBufferPtr->lightAmbient[1].x = DayNightCycle::GetLightAmbient(DayNightCycle::CelestialBody::MOON);
	// Unlock the matrix constant buffer.
	context->Unmap(m_lightBuffer, 0);
}

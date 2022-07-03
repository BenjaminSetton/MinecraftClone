#include "../Misc/pch.h"

#include "Block.h"
#include "BlockUVs.h"
#include "../Core/ShaderBufferManagers/ChunkBufferManager.h"
#include "D3D.h"
#include "DayNightCycle.h"
#include "DefaultBlockShader.h"
#include "EditorLayer.h"
#include "Panels/MainViewportPanel.h"
#include "../Utility/ImGuiLayer.h" // ImGui Debug
#include "../Utility/MathTypes.h"
#include "../Utility/Utility.h"


using namespace DirectX;
using namespace Orange;

RenderToTexture DefaultBlockShader::m_viewportTextureData = { nullptr, nullptr, nullptr };

void DefaultBlockShader::CreateObjects(const WCHAR* vsFilename, const WCHAR* gsFilename, const WCHAR* psFilename) 
{
	// Create the shaders
	CreateShaders(vsFilename, gsFilename, psFilename);

	// Create the rest of the objects
	CreateD3DObjects();
}

void DefaultBlockShader::Initialize(XMMATRIX camViewMatrix)
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
	OG_ASSERT(!FAILED(hr));
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
	OG_ASSERT(!FAILED(hr));
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
	XMFLOAT4 skyColor = DayNightCycle::GetSkyColor();
	const FLOAT colors[4] = { skyColor.x, skyColor.y, skyColor.z, skyColor.w };

	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	// Reset the RTV
	context->ClearRenderTargetView(m_viewportTextureData.renderTargetView, colors);
	context->ClearDepthStencilView(m_viewportTextureData.depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

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

	if (m_viewportTextureData.texture)
	{
		m_viewportTextureData.texture->Release();
		m_viewportTextureData.texture = nullptr;
	}

	if (m_viewportTextureData.shaderResourceView)
	{
		m_viewportTextureData.shaderResourceView->Release();
		m_viewportTextureData.shaderResourceView = nullptr;
	}

	if (m_viewportTextureData.renderTargetView)
	{
		m_viewportTextureData.renderTargetView->Release();
		m_viewportTextureData.renderTargetView = nullptr;
	}

	if (m_viewportTextureData.depthStencilState)
	{
		m_viewportTextureData.depthStencilState->Release();
		m_viewportTextureData.depthStencilState = nullptr;
	}

	if (m_viewportTextureData.depthStencilView)
	{
		m_viewportTextureData.depthStencilView->Release();
		m_viewportTextureData.depthStencilView = nullptr;
	}

	if (m_viewportTextureData.depthTexture)
	{
		m_viewportTextureData.depthTexture->Release();
		m_viewportTextureData.depthTexture = nullptr;
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
	OG_ASSERT(!FAILED(hr));

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
	OG_ASSERT(!FAILED(hr));

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
	OG_ASSERT(!FAILED(hr));

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
	OG_ASSERT(!FAILED(hr));

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
	OG_ASSERT(!FAILED(hr));
	
	CreateRTTObjects();

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
	OG_ASSERT(!FAILED(hr));

	// Compile the geometry shader
	hr = D3DCompileFromFile(gsFilename, nullptr, nullptr, "main", "gs_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS 
#ifdef _DEBUG 
		| D3DCOMPILE_DEBUG
#endif
		, 0, &GSBlob, nullptr);
	OG_ASSERT(!FAILED(hr));

	// Compile the pixel shader
	hr = D3DCompileFromFile(psFilename, nullptr, nullptr, "main", "ps_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS 
#ifdef _DEBUG 
		| D3DCOMPILE_DEBUG
#endif
		, 0, &PSBlob, nullptr);
	OG_ASSERT(!FAILED(hr));

	// Create the shaders
	hr = device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, &m_vertexShader);
	OG_ASSERT(!FAILED(hr));
	hr = device->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), nullptr, &m_geometryShader);
	OG_ASSERT(!FAILED(hr));
	hr = device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, &m_pixelShader);
	OG_ASSERT(!FAILED(hr));

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
	OG_ASSERT(!FAILED(hr));

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
	ID3D11RenderTargetView* rttRTV = GetRenderToTextureRTV();
	ID3D11DepthStencilView* rttDSV = GetDepthStencilView();
	ID3D11DepthStencilState* rttDSS = GetDepthStencilState();
	context->OMSetDepthStencilState(rttDSS, 1);

	// !!!!
	// TODO: Rethink this because we only want to render to a texture now, since we can't see the back-buffer we present
	// !!!!
	context->OMSetRenderTargets(1, &rttRTV, rttDSV);

	// Now set the matrix constant buffer in the vertex shader with the updated values.
	ID3D11Buffer* buffers[] = { m_UVBuffer, m_matrixBuffer };
	context->VSSetConstantBuffers(0, ARRAYSIZE(buffers), buffers);

	// Set the light constant buffer in the pixel shader
	context->PSSetConstantBuffers(0, 1, &m_lightBuffer);

	// Bind the block texture and shadow map
	context->PSSetShaderResources(0, 1, srvs);

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

void DefaultBlockShader::CreateRTTObjects()
{
	ID3D11Device* device = D3D::GetDevice();
	Panel* mainPanel = EditorLayer::GetPanel(EditorLayer::PanelLocation::CENTER);

	Vec2 viewportDimensions = { 0, 0 };
	if(mainPanel) viewportDimensions = mainPanel->GetDimensions();

	HRESULT hr;

	// Set up the texture that we use to create the RTV and the SRV
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width = static_cast<UINT>(viewportDimensions.x);
	textureDesc.Height = static_cast<UINT>(viewportDimensions.y);
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	hr = device->CreateTexture2D(&textureDesc, NULL, &m_viewportTextureData.texture);
	OG_ASSERT(!FAILED(hr));

	// Create the RTV
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	hr = device->CreateRenderTargetView(m_viewportTextureData.texture, &renderTargetViewDesc, &m_viewportTextureData.renderTargetView);
	OG_ASSERT(!FAILED(hr));

	// Create the SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(m_viewportTextureData.texture, &shaderResourceViewDesc, &m_viewportTextureData.shaderResourceView);
	OG_ASSERT(!FAILED(hr));

	// Set the main viewport panel's texture data to be the new SRV
	MainViewportPanel* viewportPanel = static_cast<MainViewportPanel*>(mainPanel->GetComponent(PanelComponentType::MAIN_VIEWPORT_PANEL));
	OG_ASSERT(viewportPanel != nullptr);
	viewportPanel->SetTexture(static_cast<void*>(m_viewportTextureData.shaderResourceView), viewportDimensions);


	// Set up the description of the depth buffer texture resource.
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = static_cast<UINT>(viewportDimensions.x);
	depthBufferDesc.Height = static_cast<UINT>(viewportDimensions.y);
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer
	hr = device->CreateTexture2D(&depthBufferDesc, NULL, &m_viewportTextureData.depthTexture);
	OG_ASSERT(!FAILED(hr));

	// Set up the description of the stencil state.
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	hr = device->CreateDepthStencilState(&depthStencilDesc, &m_viewportTextureData.depthStencilState);
	OG_ASSERT(!FAILED(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	hr = device->CreateDepthStencilView(m_viewportTextureData.depthTexture, &depthStencilViewDesc, &m_viewportTextureData.depthStencilView);
	OG_ASSERT(!FAILED(hr));

}

void DefaultBlockShader::UpdateViewMatrices(DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX lightViewMatrix)
{
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* matrixBufferPtr;

	// Lock the matrix constant buffer so it can be written to.
	hr = context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	OG_ASSERT(!FAILED(hr));

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
	OG_ASSERT(!FAILED(hr));
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

ID3D11ShaderResourceView* DefaultBlockShader::GetRenderToTextureSRV()
{
	return m_viewportTextureData.shaderResourceView;
}

ID3D11RenderTargetView* DefaultBlockShader::GetRenderToTextureRTV()
{
	return m_viewportTextureData.renderTargetView;
}

ID3D11DepthStencilState* DefaultBlockShader::GetDepthStencilState()
{
	return m_viewportTextureData.depthStencilState;
}

ID3D11DepthStencilView* DefaultBlockShader::GetDepthStencilView()
{
	return m_viewportTextureData.depthStencilView;
}

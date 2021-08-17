#include "../Misc/pch.h"
#include "ShadowShader.h"
#include "../Utility/Utility.h"

#include "D3D.h"
#include "DayNightCycle.h"

using namespace DirectX;

void ShadowShader::CreateObjects(const WCHAR* vsFilename, const WCHAR* psFilename)
{
	ID3D11Device* device = D3D::GetDevice();

	// Create the shaders
	CreateShaders(vsFilename, psFilename);

	// Create the rest of the objects
	CreateD3DObjects();
}

void ShadowShader::Initialize(const uint32_t width, const uint32_t height)
{
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	HRESULT hr;
	MatrixBuffer* matrixBufferPtr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	BlockVertex* vertexBufferPtr;
	XMFLOAT3 lightDirection = DayNightCycle::GetLightDirection();

	XMFLOAT4 camPos = { -4.0f, 50.0f, -10.0f, 1.0f};
	XMMATRIX viewMatrix = 
			XMMatrixLookAtLH
			(
				XMLoadFloat4(&camPos), 
				{camPos.x + lightDirection.x, camPos.y + lightDirection.y, camPos.z + lightDirection.z, 1.0f},
				{ 0.0f, 1.0f, 0.0f, 1.0f }
			);

	m_lightViewMatrix = viewMatrix;

	CreateDepthBuffer(width, height);

#pragma region WVP_MATRICES
	// Lock the matrix constant buffer so it can be written to.
	hr = context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VX_ASSERT(!FAILED(hr));
	// Get a pointer to the data in the constant buffer.
	matrixBufferPtr = (MatrixBuffer*)mappedResource.pData;
	// Copy the matrices into the constant buffer.
	matrixBufferPtr->worldMatrix = XMMatrixIdentity();
	matrixBufferPtr->viewMatrix = XMMatrixTranspose(viewMatrix);
	matrixBufferPtr->orthoMatrix = XMMatrixTranspose(D3D::GetOrthoMatrix());
	// Unlock the matrix constant buffer.
	context->Unmap(m_matrixBuffer, 0);
#pragma endregion

	BindObjects();


}

void ShadowShader::Render()
{
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	BindObjects();

	for (auto chunk : ChunkManager::GetChunkVector())
	{
		// Update the vertex buffer
		BindVertexBuffer(chunk);

		// Render the chunk
		context->Draw(chunk->GetVertexCount(), 0);
	}
}

void ShadowShader::Shutdown()
{
	if(m_depthState)
	{
		m_depthState->Release();
		m_depthState = nullptr;
	}

	if (m_depthView)
	{
		m_depthView->Release();
		m_depthView = nullptr;
	}

	if (m_shadowMapSRV)
	{
		m_shadowMapSRV->Release();
		m_shadowMapSRV = nullptr;
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

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}


}

void ShadowShader::CreateD3DObjects()
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

}

void ShadowShader::CreateShaders(const WCHAR* vsFilename, const WCHAR* psFilename)
{
	ID3D11Device* device = D3D::GetDevice();

	HRESULT hr;
	ID3D10Blob* VSBlob;
	ID3D10Blob* PSBlob;

	// Compile the vertex shader
	hr = D3DCompileFromFile(vsFilename, nullptr, nullptr, "main", "vs_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG, 0, &VSBlob, nullptr);
	VX_ASSERT(!FAILED(hr));

	// Compile the pixel shader
	hr = D3DCompileFromFile(psFilename, nullptr, nullptr, "main", "ps_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG, 0, &PSBlob, nullptr);
	VX_ASSERT(!FAILED(hr));

	// Create the shaders
	hr = device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, &m_vertexShader);
	VX_ASSERT(!FAILED(hr));
	hr = device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, &m_pixelShader);
	VX_ASSERT(!FAILED(hr));

	// Create the input element description
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// Create the vertex input layout.
	hr = device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(), &m_inputLayout);
	VX_ASSERT(!FAILED(hr));

	// Release the blobs as they are not needed anymore
	VSBlob->Release();
	PSBlob->Release();
	VSBlob = nullptr;
	PSBlob = nullptr;
}


void ShadowShader::CreateDepthBuffer(const uint32_t width, const uint32_t height)
{
	ID3D11Device* device = D3D::GetDevice();
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	HRESULT hr;

	// Release the previous objects, if necessary
	if (m_shadowTex) { m_shadowTex->Release(); m_shadowTex = nullptr; }
	if (m_depthState) { m_depthState->Release(); m_depthState = nullptr; }
	if (m_depthView) { m_depthView->Release(); m_depthView = nullptr; }
	if (m_shadowMapSRV) { m_shadowMapSRV->Release(); m_shadowMapSRV = nullptr; }

	// Set up the description of the depth buffer.
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	hr = device->CreateTexture2D(&depthBufferDesc, NULL, &m_shadowTex);
	VX_ASSERT(!FAILED(hr));

	// Initialize the description of the stencil state.
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	hr = device->CreateDepthStencilState(&depthStencilDesc, &m_depthState);
	VX_ASSERT(!FAILED(hr));

	// Initialize the depth stencil view.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	hr = device->CreateDepthStencilView(m_shadowTex, &depthStencilViewDesc, &m_depthView);
	VX_ASSERT(!FAILED(hr));

	// Create the shadow map SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(m_shadowTex, &srvDesc, &m_shadowMapSRV);
	VX_ASSERT(!FAILED(hr));
}

void ShadowShader::BindVertexBuffer(Chunk* chunk)
{
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	ID3D11Buffer* buffers[] = { chunk->GetBuffer() };
	unsigned int stride[] = { sizeof(BlockVertex) };
	unsigned int offset[] = { 0 };
	context->IASetVertexBuffers(0, 1, buffers, stride, offset);
}

void ShadowShader::BindObjects()
{
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	context->ClearDepthStencilView(m_depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Set the depth stencil state.
	context->OMSetDepthStencilState(m_depthState, 1);
	context->OMSetRenderTargets(0, nullptr, m_depthView);

	// Now set the matrix constant buffer in the vertex shader with the updated values.
	context->VSSetConstantBuffers(0, 1, &m_matrixBuffer);

	// Set the vertex input layout.
	context->IASetInputLayout(m_inputLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	context->VSSetShader(m_vertexShader, nullptr, 0);
	context->PSSetShader(m_pixelShader, nullptr, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ShadowShader::UpdateLightMatrix()
{
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* matrixBufferPtr;


	// Create the new view matrix
	XMFLOAT3 lightDirection = DayNightCycle::GetLightDirection();
	XMFLOAT4 camPos = { -4.0f, 50.0f, -10.0f, 1.0f };
	XMMATRIX viewMatrix =
		XMMatrixLookAtLH
		(
			XMLoadFloat4(&camPos),
			{ camPos.x + lightDirection.x, camPos.y + lightDirection.y, camPos.z + lightDirection.z, 1.0f },
			{ 0.0f, 1.0f, 0.0f, 1.0f }
	);

	m_lightViewMatrix = viewMatrix;

	// Lock the matrix constant buffer so it can be written to.
	hr = context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VX_ASSERT(!FAILED(hr));

	matrixBufferPtr = (MatrixBuffer*)mappedResource.pData;
	matrixBufferPtr->worldMatrix = XMMatrixIdentity();
	matrixBufferPtr->viewMatrix = XMMatrixTranspose(viewMatrix);
	matrixBufferPtr->orthoMatrix = XMMatrixTranspose(D3D::GetOrthoMatrix());

	context->Unmap(m_matrixBuffer, 0);
}

ID3D11ShaderResourceView* ShadowShader::GetShadowMap() { return m_shadowMapSRV; }

const DirectX::XMMATRIX ShadowShader::GetLightViewMatrix() { return m_lightViewMatrix; }

#include "../Misc/pch.h"
#include "QuadShader.h"
#include "D3D.h"

#include "../Utility/Utility.h"

// ImGui Debug
#include "../Utility/ImGuiLayer.h"

#include "../Core/ShaderBufferManagers/QuadBufferManager.h"

using namespace DirectX;

void QuadShader::CreateObjects(const WCHAR* vsFilename, const WCHAR* psFilename)
{
	ID3D11Device* device = D3D::GetDevice();

	// Create the shaders
	CreateShaders(vsFilename, psFilename);

	// Create the rest of the objects
	CreateD3DObjects();
}

void QuadShader::Initialize()
{
	//////////////////////
	//	THIS IS PROBABLY NOT NECESSARY
	//////////////////////


//	ID3D11DeviceContext* context = D3D::GetDeviceContext();
//
//	HRESULT hr;
//	D3D11_MAPPED_SUBRESOURCE mappedResource;
//	MatrixBuffer* matrixBufferPtr;
//	BlockInstanceData* vertexBufferPtr = nullptr;
//
//#pragma region WVP_MATRICES
//	// Lock the matrix constant buffer so it can be written to.
//	hr = context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//	VX_ASSERT(!FAILED(hr));
//	// Get a pointer to the data in the constant buffer.
//	matrixBufferPtr = (MatrixBuffer*)mappedResource.pData;
//	// Copy the matrices into the constant buffer.
//	matrixBufferPtr->viewMatrix = XMMatrixIdentity();
//	matrixBufferPtr->projectionMatrix = XMMatrixIdentity();
//	// Unlock the matrix constant buffer.
//	context->Unmap(m_matrixBuffer, 0);
//#pragma endregion

}

void QuadShader::Render()
{

	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	QuadBufferManager::UpdateBuffers();

	BindObjects();

	BindVertexBuffers();

	uint32_t size = static_cast<uint32_t>(QuadBufferManager::GetInstanceData().size());

	context->DrawInstanced(6, size, 0, 0);

	// Clear the quad buffers
	QuadBufferManager::Clear();
}

void QuadShader::Shutdown()
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

void QuadShader::CreateD3DObjects()
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
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
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

void QuadShader::CreateShaders(const WCHAR* vsFilename, const WCHAR* psFilename)
{
	ID3D11Device* device = D3D::GetDevice();

	HRESULT hr;
	ID3D10Blob* VSBlob;
	ID3D10Blob* PSBlob;

	// Compile the vertex shader
	hr = D3DCompileFromFile(vsFilename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS
#ifdef _DEBUG 
		| D3DCOMPILE_DEBUG
#endif
		, 0, &VSBlob, nullptr);
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
	hr = device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, &m_pixelShader);
	VX_ASSERT(!FAILED(hr));

	// Create the input element description
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		// Per-vertex data
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 0,								D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,   0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	  0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },

		// Per-instance data
		{ "TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA, 1 },
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

void QuadShader::BindVertexBuffers()
{
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	ID3D11Buffer* buffers[] = { QuadBufferManager::GetVertexBuffer(), QuadBufferManager::GetInstanceBuffer() };
	unsigned int stride[] = { sizeof(QuadVertexData), sizeof(QuadInstanceData) };
	unsigned int offset[] = { 0, 0 };
	context->IASetVertexBuffers(0, ARRAYSIZE(buffers), buffers, stride, offset);
}

void QuadShader::BindObjects()
{
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	// Set the back buffer and the depth buffer
	ID3D11RenderTargetView* backBuffer = D3D::GetBackBuffer();
	context->OMSetDepthStencilState(D3D::GetDepthStencilState(), 1);
	context->OMSetRenderTargets(1, &backBuffer, D3D::GetDepthStencilView());

	// Now set the matrix constant buffer in the vertex shader with the updated values.
	ID3D11Buffer* buffer[] = { m_matrixBuffer };
	context->VSSetConstantBuffers(0, 1, buffer);

	// Bind the quad texture
	context->PSSetShaderResources(0, 1, &m_quadTexture);

	// Set the vertex input layout.
	context->IASetInputLayout(m_inputLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	context->VSSetShader(m_vertexShader, nullptr, 0);
	context->PSSetShader(m_pixelShader, nullptr, 0);

	// Set the sampler state in the pixel shader.
	ID3D11SamplerState* samplers[] = { m_samplerClamp };
	context->PSSetSamplers(0, ARRAYSIZE(samplers), samplers);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void QuadShader::UpdateViewMatrix(DirectX::XMMATRIX viewMatrix)
{
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* matrixBufferPtr;

	// Lock the matrix constant buffer so it can be written to.
	hr = context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VX_ASSERT(!FAILED(hr));

	matrixBufferPtr = (MatrixBuffer*)mappedResource.pData;
	matrixBufferPtr->viewMatrix = XMMatrixTranspose(viewMatrix);
	matrixBufferPtr->projectionMatrix = XMMatrixTranspose(D3D::GetProjectionMatrix());

	context->Unmap(m_matrixBuffer, 0);
}

void QuadShader::SetQuadTexture(ID3D11ShaderResourceView* quadTexture) { m_quadTexture = quadTexture; }
#include "../Misc/pch.h"
#include "DefaultBlockShader.h"

#include "../Utility/Utility.h"

using namespace DirectX;

void DefaultBlockShader::CreateObjects(ID3D11Device* device, const WCHAR* vsFilename, const WCHAR* psFilename) 
{

	// Create the shaders
	CreateShaders(device, vsFilename, psFilename);

	// Create the rest of the objects
	CreateD3DObjects(device);
}

void DefaultBlockShader::Initialize(ID3D11DeviceContext* context, DirectX::XMMATRIX WM, DirectX::XMMATRIX VM,
	DirectX::XMMATRIX PM, DirectX::XMFLOAT3 lightDir, DirectX::XMFLOAT4 lightCol, ID3D11ShaderResourceView* srv)
{

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* matrixBufferPtr;
	LightBuffer* lightBufferPtr;
	BlockVertex* vertexBufferPtr;

	m_projection = XMMatrixTranspose(PM);

#pragma region WVP_MATRICES
	// Lock the matrix constant buffer so it can be written to.
	hr = context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VX_ASSERT(!FAILED(hr));
	// Get a pointer to the data in the constant buffer.
	matrixBufferPtr = (MatrixBuffer*)mappedResource.pData;
	// Copy the matrices into the constant buffer.
	matrixBufferPtr->worldMatrix = XMMatrixIdentity();
	matrixBufferPtr->viewMatrix = XMMatrixTranspose(VM);
	matrixBufferPtr->projectionMatrix = m_projection;
	// Unlock the matrix constant buffer.
	context->Unmap(m_matrixBuffer, 0);
#pragma endregion

#pragma LIGHT_MATRIX
	// Lock the matrix constant buffer so it can be written to.
	hr = context->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VX_ASSERT(!FAILED(hr));
	// Get a pointer to the data in the constant buffer.
	lightBufferPtr = (LightBuffer*)mappedResource.pData;
	// Copy the matrices into the constant buffer.
	lightBufferPtr->lightDir = lightDir;
	lightBufferPtr->lightCol = lightCol;
	lightBufferPtr->padding = 0.0f;
	// Unlock the matrix constant buffer.
	context->Unmap(m_lightBuffer, 0);
#pragma endregion

	// Now set the matrix constant buffer in the vertex shader with the updated values.
	context->VSSetConstantBuffers(0, 1, &m_matrixBuffer);

	// Set the light constant buffer in the pixel shader
	context->PSSetConstantBuffers(0, 1, &m_lightBuffer);

	// Bind the SRV (textures) to the pixel shader Texture2D slot
	context->PSSetShaderResources(0, 1, &srv);

	// Set the vertex input layout.
	context->IASetInputLayout(m_inputLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	context->VSSetShader(m_vertexShader, NULL, 0);
	context->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	context->PSSetSamplers(0, 1, &m_sampler);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void DefaultBlockShader::Render(ID3D11DeviceContext* context)
{
	{
		VX_PROFILE_SCOPE("UpdateVertexBuffer");
		// Update the vertex buffer
		UpdateVertexBuffer(context);
	}

	{
		VX_PROFILE_SCOPE("Draw");
		// Render the chunk
		uint32_t numVerts = ChunkManager::GetVertices().size();
		context->Draw(numVerts, 0);
	}
}

void DefaultBlockShader::Shutdown()
{
	// Release the sampler state.
	if (m_sampler)
	{
		m_sampler->Release();
		m_sampler = nullptr;
	}

	// Release the matrix constant buffer.
	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = nullptr;
	}

	// Release the matrix constant buffer.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	// Release the layout.
	if (m_inputLayout)
	{
		m_inputLayout->Release();
		m_inputLayout = nullptr;
	}

	// Release the pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	// Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}

	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
}

void DefaultBlockShader::CreateD3DObjects(ID3D11Device* device)
{
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

	// Create the matrix constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	VX_ASSERT(!FAILED(hr));

	// Create a texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	hr = device->CreateSamplerState(&samplerDesc, &m_sampler);
	VX_ASSERT(!FAILED(hr));


	// Create the vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(BlockVertex) * ChunkManager::GetVertices().capacity();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	hr = device->CreateBuffer(&vertexBufferDesc, nullptr, &m_vertexBuffer);
	VX_ASSERT(!FAILED(hr));
	
}

void DefaultBlockShader::CreateShaders(ID3D11Device* device, const WCHAR* vsFilename, const WCHAR* psFilename)
{
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

		// Per-instance data
		{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
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


void DefaultBlockShader::UpdateVertexBuffer(ID3D11DeviceContext* context)
{
	{
		VX_PROFILE_SCOPE("UpdateSubresource");
		context->UpdateSubresource(m_vertexBuffer, 0, nullptr, &ChunkManager::GetVertices()[0], 0, 0);
	}

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	ID3D11Buffer* buffers[] = { m_vertexBuffer };
	unsigned int stride[] = { sizeof(BlockVertex) };
	unsigned int offset[] = { 0 };
	context->IASetVertexBuffers(0, ARRAYSIZE(buffers), buffers, stride, offset);
}

void DefaultBlockShader::UpdateViewMatrix(ID3D11DeviceContext* context, DirectX::XMMATRIX viewMatrix)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* matrixBufferPtr;

	// Lock the matrix constant buffer so it can be written to.
	hr = context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VX_ASSERT(!FAILED(hr));

	matrixBufferPtr = (MatrixBuffer*)mappedResource.pData;
	matrixBufferPtr->worldMatrix = XMMatrixIdentity();
	matrixBufferPtr->viewMatrix = XMMatrixTranspose(viewMatrix);
	matrixBufferPtr->projectionMatrix = m_projection;

	context->Unmap(m_matrixBuffer, 0);
}
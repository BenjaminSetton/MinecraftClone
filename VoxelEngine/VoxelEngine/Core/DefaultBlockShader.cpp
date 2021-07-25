#include "../Misc/pch.h"
#include "DefaultBlockShader.h"

#include "Chunk.h"
#include "../Utility/Utility.h"

using namespace DirectX;

void DefaultBlockShader::CreateObjects(ID3D11Device* device, const WCHAR* vsFilename, const WCHAR* psFilename) 
{
	// Create the shaders
	CreateShaders(device, vsFilename, psFilename);

	// Create the rest of the objects
	CreateD3DObjects(device);
}

void DefaultBlockShader::Render(ID3D11DeviceContext* context, unsigned int indexCount,
	XMMATRIX WM, XMMATRIX VM, XMMATRIX PM, XMFLOAT3 lightDir,
	XMFLOAT4 lightCol, ID3D11ShaderResourceView* srv)
{
	// Set the shader parameters
	SetShaderParameters(context, WM, VM, PM, lightDir, lightCol, srv);

	// Render the model
	context->DrawIndexedInstanced(36, pow(CHUNK_SIZE, 3), 0, 0, 0);
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

	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}
}

void DefaultBlockShader::CreateD3DObjects(ID3D11Device* device)
{
	// MEGA TEMPORARY
	Chunk* tempChunk = new Chunk();


	HRESULT hr;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_BUFFER_DESC instancePosBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexBufferData;
	D3D11_SUBRESOURCE_DATA indexBufferData;
	D3D11_SUBRESOURCE_DATA instancePosBufferData;
	D3D11_SAMPLER_DESC samplerDesc;

	// Setup the description of the matrix dynamic constant buffer that is in the vertex shader.
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
	samplerDesc = {};
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


	// Create the vertex and index buffers
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(BlockVertex) * 24;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexBufferData.pSysMem = verts;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer);
	VX_ASSERT(!FAILED(hr));


	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * 36;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexBufferData.pSysMem = &indicies;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	// Create the index buffer.
	hr = device->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_indexBuffer);
	VX_ASSERT(!FAILED(hr));


	instancePosBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instancePosBufferDesc.ByteWidth = sizeof(XMFLOAT3) * pow(CHUNK_SIZE, 3);
	instancePosBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instancePosBufferDesc.CPUAccessFlags = 0;
	instancePosBufferDesc.MiscFlags = 0;
	instancePosBufferDesc.StructureByteStride = 0;

	instancePosBufferData.pSysMem = tempChunk->GetBlockPositions();
	instancePosBufferData.SysMemPitch = 0;
	instancePosBufferData.SysMemSlicePitch = 0;

	// Create the instance position buffer
	hr = device->CreateBuffer(&instancePosBufferDesc, &instancePosBufferData, &m_instancePosBuffer);
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


void DefaultBlockShader::SetShaderParameters(ID3D11DeviceContext* context, DirectX::XMMATRIX WM, DirectX::XMMATRIX VM, 
	DirectX::XMMATRIX PM, DirectX::XMFLOAT3 lightDir, DirectX::XMFLOAT4 lightCol, ID3D11ShaderResourceView* srv)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* matrixBufferPtr;
	LightBuffer* lightBufferPtr; 

	// Transpose the matrices to prepare them for the shader.
	WM = XMMatrixTranspose(WM);
	VM = XMMatrixTranspose(VM);
	PM = XMMatrixTranspose(PM);


	// Lock the matrix constant buffer so it can be written to.
	hr = context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VX_ASSERT(!FAILED(hr));
	// Get a pointer to the data in the constant buffer.
	matrixBufferPtr = (MatrixBuffer*)mappedResource.pData;
	// Copy the matrices into the constant buffer.
	matrixBufferPtr->worldMatrix = WM;
	matrixBufferPtr->viewMatrix = VM;
	matrixBufferPtr->projectionMatrix = PM;
	// Unlock the matrix constant buffer.
	context->Unmap(m_matrixBuffer, 0);

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

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	ID3D11Buffer* buffers[] = { m_vertexBuffer, m_instancePosBuffer };
	unsigned int stride[] = { sizeof(BlockVertex), sizeof(XMFLOAT3) };
	unsigned int offset[] = { 0, 0 };
	context->IASetVertexBuffers(0, ARRAYSIZE(buffers), buffers, stride, offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
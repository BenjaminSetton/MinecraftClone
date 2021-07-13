#include "../Misc/pch.h"
#include "DefaultBlockShader.h"

#include "Block.h"

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
	context->DrawIndexed(indexCount, 0, 0);
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
	HRESULT hr;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexBufferData;
	D3D11_SUBRESOURCE_DATA indexBufferData;
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
	assert(!FAILED(hr));

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBuffer);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the matrix constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	assert(!FAILED(hr));

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	hr = device->CreateSamplerState(&samplerDesc, &m_sampler);
	assert(!FAILED(hr));


	// Create the vertex and index buffers

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(BlockVertex) * 24;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexBufferData.pSysMem = &verts;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer);
	assert(!FAILED(hr));

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * 36;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexBufferData.pSysMem = &indicies;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	// Create the index buffer.
	hr = device->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_indexBuffer);
	assert(!FAILED(hr));
	
}

void DefaultBlockShader::CreateShaders(ID3D11Device* device, const WCHAR* vsFilename, const WCHAR* psFilename)
{
	HRESULT hr;
	ID3D10Blob* VSBlob;
	ID3D10Blob* PSBlob;
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[3];

	// Compile the vertex shader
	hr = D3DCompileFromFile(vsFilename, nullptr, nullptr, "main", "vs_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0, &VSBlob, nullptr);
	assert(!FAILED(hr));

	// Compile the pixel shader
	hr = D3DCompileFromFile(psFilename, nullptr, nullptr, "main", "ps_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0, &PSBlob, nullptr);
	assert(!FAILED(hr));

	// Create the shaders
	hr = device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, &m_vertexShader);
	assert(!FAILED(hr));
	hr = device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, &m_pixelShader);
	assert(!FAILED(hr));

	// Create the input element description
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[0].InputSlot = 0;
	inputElementDesc[0].AlignedByteOffset = 0;
	inputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[0].InstanceDataStepRate = 0;

	inputElementDesc[1].SemanticName = "NORMAL";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[1].InputSlot = 0;
	inputElementDesc[1].AlignedByteOffset = 0;
	inputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[1].InstanceDataStepRate = 0;

	inputElementDesc[2].SemanticName = "TEXCOORD";
	inputElementDesc[2].SemanticIndex = 0;
	inputElementDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[2].InputSlot = 0;
	inputElementDesc[2].AlignedByteOffset = 0;
	inputElementDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[2].InstanceDataStepRate = 0;

	// Create the vertex input layout.
	hr = device->CreateInputLayout(inputElementDesc, 3, VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(), &m_inputLayout);
	assert(!FAILED(hr));

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
	unsigned int stride = sizeof(BlockVertex);
	unsigned int offset = 0;

	// Transpose the matrices to prepare them for the shader.
	WM = XMMatrixTranspose(WM);
	VM = XMMatrixTranspose(VM);
	PM = XMMatrixTranspose(PM);


	// Lock the matrix constant buffer so it can be written to.
	hr = context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	assert(!FAILED(hr));
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
	assert(!FAILED(hr));
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
	context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
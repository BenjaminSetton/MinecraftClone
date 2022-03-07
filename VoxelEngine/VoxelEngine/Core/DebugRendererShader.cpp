#include "../Misc/pch.h"
#include "DebugRendererShader.h"
#include "../Utility/DebugRenderer.h"

#include "D3D.h"

#include "../Utility/Utility.h"

using namespace DirectX;

void DebugRendererShader::CreateObjects(const WCHAR* vsFilename, const WCHAR* psFilename)
{
	ID3D11Device* device = D3D::GetDevice();

	// Create the shaders
	CreateShaders(vsFilename, psFilename);

	// Create the rest of the objects
	CreateD3DObjects();
}

void DebugRendererShader::Initialize(XMMATRIX camViewMatrix)
{
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* matrixBufferPtr;

#pragma region WVP_MATRICES
	// Lock the matrix constant buffer so it can be written to.
	hr = context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VX_ASSERT(!FAILED(hr));
	// Get a pointer to the data in the constant buffer.
	matrixBufferPtr = (MatrixBuffer*)mappedResource.pData;
	// Copy the matrices into the constant buffer.
	matrixBufferPtr->world = XMMatrixIdentity();
	matrixBufferPtr->view = XMMatrixTranspose(camViewMatrix);
	matrixBufferPtr->proj = XMMatrixTranspose(D3D::GetProjectionMatrix());
	// Unlock the matrix constant buffer.
	context->Unmap(m_matrixBuffer, 0);
#pragma endregion

}

void DebugRendererShader::Render()
{
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	BindObjects();

	// Update the vertex buffer
	UpdateVertexBuffer();

	// Render the lines
	int numVerts = static_cast<int>(DebugRenderer::GetVertexCount());
	context->Draw(numVerts, 0);

	// Clear the previous lines
	//DebugLine::ClearLines();
}

void DebugRendererShader::Shutdown()
{

	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
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

void DebugRendererShader::CreateD3DObjects()
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


	// Setup the description of the matrix dynamic vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(DebugRenderer::ColoredVertex) * DebugRenderer::GetVertexCapacity());
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Create the matrix constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = device->CreateBuffer(&vertexBufferDesc, NULL, &m_vertexBuffer);
	VX_ASSERT(!FAILED(hr));
}

void DebugRendererShader::CreateShaders(const WCHAR* vsFilename, const WCHAR* psFilename)
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
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

void DebugRendererShader::UpdateVertexBuffer()
{
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	// Update the vertex buffer
	const DebugRenderer::ColoredVertex* data = DebugRenderer::GetLineVertices();
	context->UpdateSubresource(m_vertexBuffer, 0, nullptr, data, (UINT)0, (UINT)0);

	// Bind the vertex buffer
	ID3D11Buffer* buffers[] = { m_vertexBuffer };
	unsigned int stride[] = { sizeof(DebugRenderer::ColoredVertex) };
	unsigned int offset[] = { 0 };
	context->IASetVertexBuffers(0, ARRAYSIZE(buffers), buffers, stride, offset);
}

void DebugRendererShader::BindObjects()
{
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	// Set the back buffer and the depth buffer
	ID3D11RenderTargetView* backBuffer = D3D::GetBackBuffer();
	context->OMSetDepthStencilState(D3D::GetDepthStencilState(), 1);
	context->OMSetRenderTargets(1, &backBuffer, D3D::GetDepthStencilView());

	// Now set the matrix constant buffer in the vertex shader with the updated values.
	context->VSSetConstantBuffers(0, 1, &m_matrixBuffer);

	// Set the vertex input layout.
	context->IASetInputLayout(m_inputLayout);

	// Set the vertex and pixel shaders that will be used to render this line.
	context->VSSetShader(m_vertexShader, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(m_pixelShader, nullptr, 0);

	// Set the blend state
	context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case lines.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

void DebugRendererShader::UpdateViewMatrix(DirectX::XMMATRIX viewMatrix)
{
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* matrixBufferPtr;

	// Lock the matrix constant buffer so it can be written to.
	hr = context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VX_ASSERT(!FAILED(hr));

	matrixBufferPtr = (MatrixBuffer*)mappedResource.pData;
	matrixBufferPtr->world = XMMatrixIdentity();
	matrixBufferPtr->view = XMMatrixTranspose(viewMatrix);
	matrixBufferPtr->proj = XMMatrixTranspose(D3D::GetProjectionMatrix());

	context->Unmap(m_matrixBuffer, 0);
}

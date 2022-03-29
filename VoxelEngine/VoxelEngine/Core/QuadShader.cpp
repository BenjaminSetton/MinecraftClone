#include "../Misc/pch.h"
#include "QuadShader.h"
#include "D3D.h"

#include "../Utility/Utility.h"

// ImGui Debug
#include "../Utility/ImGuiLayer.h"

#include "../Core/ShaderBufferManagers/QuadBufferManager.h"
#include "../Core/ShaderBufferManagers/QuadNDCBufferManager.h"

using namespace DirectX;

void QuadShader::CreateObjects(const WCHAR* vsFilename, const WCHAR* vsNDCFilename, const WCHAR* psFilename)
{
	// Create the shaders
	CreateShaders(vsFilename, vsNDCFilename, psFilename);

	// Create the rest of the objects
	CreateD3DObjects();

	m_renderInNDC = false;
}

void QuadShader::Initialize() {}

void QuadShader::Render()
{

	ID3D11DeviceContext* context = D3D::GetDeviceContext();


	BindObjects();

	BindVertexBuffers();

	uint32_t size = 0;
	if (m_renderInNDC)
	{
		QuadNDCBufferManager::UpdateBuffers();
		size = static_cast<uint32_t>(QuadNDCBufferManager::GetVertexData().size());
		context->Draw(size, 0);
		QuadNDCBufferManager::Clear();
	}
	else
	{
		QuadBufferManager::UpdateBuffers();
		size = static_cast<uint32_t>(QuadBufferManager::GetInstanceData().size());
		context->DrawInstanced(6, size, 0, 0);
		QuadBufferManager::Clear();
	}


}

void QuadShader::Shutdown()
{

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

	if (m_inputWSLayout)
	{
		m_inputWSLayout->Release();
		m_inputWSLayout = nullptr;
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

	// Create a clamp texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc = {};
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

	// Create the blend state
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;

	rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.AlphaToCoverageEnable = true;

	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDesc.RenderTarget[0] = rtbd;

	hr = device->CreateBlendState(&blendDesc, &m_blendState);
	VX_ASSERT(!FAILED(hr));

}

void QuadShader::CreateShaders(const WCHAR* vsFilename, const WCHAR* vsNDCFilename, const WCHAR* psFilename)
{
	ID3D11Device* device = D3D::GetDevice();

	HRESULT hr;
	ID3D10Blob* VSBlob;
	ID3D10Blob* VSNDCBlob;
	ID3D10Blob* PSBlob;

	// Compile the vertex shader
	hr = D3DCompileFromFile(vsFilename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS
#ifdef _DEBUG 
		| D3DCOMPILE_DEBUG
#endif
		, 0, &VSBlob, nullptr);
	VX_ASSERT(!FAILED(hr));

	// Compile the NDC vertex shader
	hr = D3DCompileFromFile(vsNDCFilename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS
#ifdef _DEBUG 
		| D3DCOMPILE_DEBUG
#endif
		, 0, &VSNDCBlob, nullptr);
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
	hr = device->CreateVertexShader(VSNDCBlob->GetBufferPointer(), VSNDCBlob->GetBufferSize(), nullptr, &m_vertexNDCShader);
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
		VSBlob->GetBufferSize(), &m_inputWSLayout);
	VX_ASSERT(!FAILED(hr));

	// Create the input element description
	D3D11_INPUT_ELEMENT_DESC inputNDCElementDesc[] =
	{
		// Per-vertex data
		{ "POSITION",	0, DXGI_FORMAT_R32G32_FLOAT,		0, 0,								D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NDCPOS",		0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SCALE",		0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the vertex input layout.
	hr = device->CreateInputLayout(inputNDCElementDesc, ARRAYSIZE(inputNDCElementDesc), VSNDCBlob->GetBufferPointer(),
		VSNDCBlob->GetBufferSize(), &m_inputNDCLayout);
	VX_ASSERT(!FAILED(hr));


	// Release the blobs as they are not needed anymore
	VSBlob->Release();
	VSNDCBlob->Release();
	PSBlob->Release();
	VSBlob = nullptr;
	PSBlob = nullptr;
}

void QuadShader::BindVertexBuffers()
{
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	if (m_renderInNDC)
	{
		ID3D11Buffer* buffers[] = { QuadNDCBufferManager::GetVertexBuffer() };
		unsigned int stride[] = { sizeof(QuadNDCVertexData) };
		unsigned int offset[] = { 0 };
		context->IASetVertexBuffers(0, ARRAYSIZE(buffers), buffers, stride, offset);
	}
	else
	{
		ID3D11Buffer* buffers[] = { QuadBufferManager::GetVertexBuffer(), QuadBufferManager::GetInstanceBuffer() };
		unsigned int stride[] = { sizeof(QuadVertexData), sizeof(QuadInstanceData) };
		unsigned int offset[] = { 0, 0 };
		context->IASetVertexBuffers(0, ARRAYSIZE(buffers), buffers, stride, offset);
	}
}

void QuadShader::BindObjects()
{
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	// Set the back buffer and the depth buffer
	ID3D11RenderTargetView* backBuffer = D3D::GetBackBuffer();
	context->OMSetDepthStencilState(D3D::GetDepthStencilState(), 1);
	context->OMSetRenderTargets(1, &backBuffer, D3D::GetDepthStencilView());


	if (m_renderInNDC)
	{
		context->IASetInputLayout(m_inputNDCLayout);

		context->VSSetShader(m_vertexNDCShader, nullptr, 0);
	}
	else
	{
		ID3D11Buffer* buffer[] = { m_matrixBuffer };
		context->VSSetConstantBuffers(0, 1, buffer);

		context->IASetInputLayout(m_inputWSLayout);

		context->VSSetShader(m_vertexShader, nullptr, 0);
	}

	context->PSSetShader(m_pixelShader, nullptr, 0);

	// Bind the quad texture
	context->PSSetShaderResources(0, 1, &m_quadTexture);

	// Set the sampler state in the pixel shader.
	ID3D11SamplerState* samplers[] = { m_samplerClamp };
	context->PSSetSamplers(0, ARRAYSIZE(samplers), samplers);

	// Set the blend state
	context->OMSetBlendState(m_blendState, nullptr, 0xFFFFFFFF);

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

void QuadShader::SetRenderInNDC(const bool renderInNDC) { m_renderInNDC = renderInNDC; }
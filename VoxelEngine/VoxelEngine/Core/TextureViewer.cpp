#include "../Misc/pch.h"

#include "TextureViewer.h"
#include "../Utility/Utility.h"
#include "D3D.h"
#include <d3dcompiler.h>

#include "Application.h"

using namespace DirectX;

TextureViewer::TextureViewer(ID3D11ShaderResourceView* tex, float x /*= 0.0f*/, float y /*= 0.0f*/,
	float scale /*= 1.0f*/)
	: m_texture(tex), m_x(x), m_y(y), m_scale(scale)
{
	CreateShaders();
	CreateObjects();
}

TextureViewer::~TextureViewer()
{
	// We do not have to release m_texture since we did not
	// create it!!

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	if (m_inputLayout)
	{
		m_inputLayout->Release();
		m_inputLayout = nullptr;
	}

	if (m_samplerClamp)
	{
		m_samplerClamp->Release();
		m_samplerClamp = nullptr;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	if(m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}
}

void TextureViewer::Render()
{

	// Only render if texture is bound
	if(m_texture)
	{
		BindObjects();

		D3D::TurnZBufferOff();

		D3D::GetDeviceContext()->Draw(6, 0);

		D3D::TurnZBufferOn();
	}
	else
	{
		VX_LOG_WARN("Texture not bound to TextureViewer instance!");
	}
}

void TextureViewer::SetTexture(ID3D11ShaderResourceView* const srv)
{
	if (!m_texture)
	{
		m_texture = srv;
		CreateVertexBuffer();
	}
	else if(m_texture != srv)
	{
		m_texture = srv;
		CreateVertexBuffer();
	}
}

void TextureViewer::CreateShaders()
{
	ID3D11Device* device = D3D::GetDevice();

	HRESULT hr;
	ID3D10Blob* VSBlob;
	ID3D10Blob* PSBlob;

	// Compile the vertex shader
	hr = D3DCompileFromFile(L"./Shaders/TextureViewer_VS.hlsl", nullptr, nullptr, "main", "vs_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG, 0, &VSBlob, nullptr);
	VX_ASSERT(!FAILED(hr));

	// Compile the pixel shader
	hr = D3DCompileFromFile(L"./Shaders/TextureViewer_PS.hlsl", nullptr, nullptr, "main", "ps_5_0",
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

void TextureViewer::CreateObjects()
{
	ID3D11Device* device = D3D::GetDevice();
	HRESULT hr;

	// Create a clamp texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc = {};
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

	CreateVertexBuffer();
}

void TextureViewer::BindObjects()
{
	ID3D11DeviceContext* context = D3D::GetDeviceContext();

	// Set the back buffer and the depth buffer
	ID3D11RenderTargetView* backBuffer = D3D::GetBackBuffer();
	context->OMSetDepthStencilState(D3D::GetDepthStencilState(), 1);
	context->OMSetRenderTargets(1, &backBuffer, D3D::GetDepthStencilView());

	// Bind the block texture and shadow map
	context->PSSetShaderResources(0, 1, &m_texture);

	// Set the vertex input layout.
	context->IASetInputLayout(m_inputLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	context->VSSetShader(m_vertexShader, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(m_pixelShader, nullptr, 0);

	// Bind the quad vertex buffer
	ID3D11Buffer* buffer[] = { m_vertexBuffer };
	unsigned int stride[] = { sizeof(QuadVertex) };
	unsigned int offset[] = { 0 };
	context->IASetVertexBuffers(0, 1, buffer, stride, offset);

	// Set the sampler state in the pixel shader.
	ID3D11SamplerState* samplers[] = { m_samplerClamp };
	context->PSSetSamplers(0, ARRAYSIZE(samplers), samplers);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void TextureViewer::CreateVertexBuffer()
{
	ID3D11Device* device = D3D::GetDevice();
	HRESULT hr;

	// If vertex buffer already exists, delete
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	// If texture is bound, create new vertex buffer
	if (m_texture)
	{
		// Create the quads
		ID3D11Resource* resource;
		ID3D11Texture2D* tex;
		D3D11_TEXTURE2D_DESC texDesc;

		m_texture->GetResource(&resource);
		resource->QueryInterface<ID3D11Texture2D>(&tex);
		tex->GetDesc(&texDesc);

		uint32_t windowWidth = Application::GetWindowWidth();
		uint32_t windowHeight = Application::GetWindowHeight();

		// Positions and width/height mapped from [-1, 1], instead of [0, 1]
		float texWidthNorm = texDesc.Width / static_cast<float>(windowWidth) * m_scale;
		float texHeightNorm = texDesc.Height / static_cast<float>(windowHeight) * m_scale;
		float xPosNorm = m_x / static_cast<float>(windowWidth);
		float yPosNorm = m_y / static_cast<float>(windowHeight);

		QuadVertex quadVertices[] = // CLOCKWISE winding, position must be in NDC
		{
			{{xPosNorm * 2.0f - 1.0f, -(yPosNorm * 2.0f - 1.0f), 0}, {0, 0}}, // TL
			{{(xPosNorm + texWidthNorm) * 2.0f - 1.0f, -(yPosNorm * 2.0f - 1.0f), 0}, {1, 0}}, // TR
			{{xPosNorm * 2.0f - 1.0f, -((yPosNorm + texHeightNorm) * 2.0f - 1.0f), 0}, {0, 1}}, // BL
			{{(xPosNorm + texWidthNorm) * 2.0f - 1.0f, -(yPosNorm * 2.0f - 1.0f), 0}, {1, 0}}, // TR
			{{(xPosNorm + texWidthNorm) * 2.0f - 1.0f, -((yPosNorm + texHeightNorm) * 2.0f - 1.0f), 0}, {1, 1}}, // BR
			{{xPosNorm * 2.0f - 1.0f, -((yPosNorm + texHeightNorm) * 2.0f - 1.0f), 0}, {0, 1}} // BL
		};

		// Create the vertex buffer
		D3D11_BUFFER_DESC vBufferDesc;
		vBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vBufferDesc.ByteWidth = sizeof(QuadVertex) * 6;
		vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vBufferDesc.CPUAccessFlags = 0;
		vBufferDesc.MiscFlags = 0;
		vBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vBufferData;
		vBufferData.pSysMem = &quadVertices;
		vBufferData.SysMemPitch = 0;
		vBufferData.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&vBufferDesc, &vBufferData, &m_vertexBuffer);
		VX_ASSERT(!FAILED(hr));
	}
}

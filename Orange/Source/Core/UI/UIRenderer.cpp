
#include "../Application.h"
#include "../D3D.h"
#include "../../Utility/FileSystem/FileSystem.h"
#include "UIBuffer.h"
#include "UIHelper.h"
#include "UIRenderer.h"
#include "../../Utility/Utility.h"

namespace Orange
{

	UIVertex UIRenderer::m_UIVertices[4] = { };
	std::unordered_map<char, ID3D11ShaderResourceView*> UIRenderer::m_charToSRVMap = std::unordered_map<char, ID3D11ShaderResourceView*>();

	DirectX::XMMATRIX UIRenderer::m_orthographicProjection = DirectX::XMMatrixIdentity();
	ID3D11VertexShader* UIRenderer::m_vertexShader = nullptr;
	ID3D11PixelShader* UIRenderer::m_pixelShader = nullptr;
	ID3D11Buffer* UIRenderer::m_constantBuffer = nullptr;
	ID3D11Buffer* UIRenderer::m_vertexBuffer = nullptr;
	ID3D11InputLayout* UIRenderer::m_inputLayout = nullptr;
	ID3D11SamplerState* UIRenderer::m_samplerClamp = nullptr;
	ID3D11BlendState* UIRenderer::m_blendState = nullptr;

	void UIRenderer::Initialize()
	{
		CreateShaders();
		CreateObjects();
	}

	void UIRenderer::Deinitialize()
	{
		DeleteShaders();
		DeleteObjects();
	}

	void UIRenderer::Draw()
	{
		BindObjects();

		// Loop over every draw command and execute it
		while (!UIBuffer::IsDrawCommandQueueEmpty())
		{
			// Get the current draw command
			UIDrawCommand currCommand = UIBuffer::PeekDrawCommand();

			// Get the vertices corresponding to that draw command
			UIVertex vertices[6];
			for (uint32_t i = 0; i < 6; i++)
			{
				vertices[i] = UIBuffer::PeekUIVertex();
				UIBuffer::PopUIVertex();
			}

			switch (currCommand.type)
			{
			case UIElementType::INVALID:
			{
				OG_ASSERT_MSG(false, "Attempting to draw UI of invalid type. Something went wrong");
				break;
			}
			case UIElementType::TEXT:
			{
				DrawChar(currCommand, vertices);
				break;
			}
			case UIElementType::IMAGE:
			{
				break;
			}
			}

			// Create the vertices using the current draw command

			UIBuffer::PopDrawCommand();
		}
	}

	void UIRenderer::DrawChar(const UIDrawCommand& drawCommand, const UIVertex* vertices)
	{
		ID3D11DeviceContext* context = D3D::GetDeviceContext();

		char c = drawCommand.drawContext.text.c;

		// We need to create a new SRV
		if (!m_charToSRVMap.contains(c))
		{
			CreateCharSRVAndAddToMap(drawCommand.textureHandle, c);
		}

		// The space character '' does not have a width or a height, so we cannot create a texture out of it.
		// In this case, we will not draw anything and we will just advance to simulate a "space".
		if (m_charToSRVMap[c] != nullptr)
		{
			BindCharTexture(c);

			// Update the vertex buffer with the new vertices
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			HRESULT hr = context->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			OG_ASSERT(!FAILED(hr));

			int64_t numBytes = (int64_t)sizeof(UIVertex) * 6;
			memcpy(mappedResource.pData, vertices, numBytes);

			context->Unmap(m_vertexBuffer, 0);

			BindVertexBuffer();

			context->Draw(6, 0);
		}
	}

	void UIRenderer::CreateObjects()
	{
		ID3D11Device* device = D3D::GetDevice();

		HRESULT hr;

		// Calculate and set the projection matrix
		Vec2 windowSize = Application::Handle->GetMainWindow()->GetSize();
		m_orthographicProjection = DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicOffCenterLH(0.0f, windowSize.x, 0.0f, windowSize.y, D3D::GetNearPlane(), D3D::GetFarPlane()));

		// Setup the description of the constant buffer
		D3D11_BUFFER_DESC cBufferDesc;
		cBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		cBufferDesc.ByteWidth = sizeof(DirectX::XMMATRIX); // The size of the projection matrix
		cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cBufferDesc.CPUAccessFlags = 0;
		cBufferDesc.MiscFlags = 0;
		cBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA cBufferData;
		cBufferData.pSysMem = static_cast<void*>(&m_orthographicProjection);
		cBufferData.SysMemPitch = 0;
		cBufferData.SysMemSlicePitch = 0;

		// Create the constant buffer
		hr = device->CreateBuffer(&cBufferDesc, &cBufferData, &m_constantBuffer);
		OG_ASSERT(!FAILED(hr));

		// Setup the description of the sampler
		D3D11_SAMPLER_DESC samplerDesc = {};
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = 0;

		// Create the sampler state.
		hr = device->CreateSamplerState(&samplerDesc, &m_samplerClamp);
		OG_ASSERT(!FAILED(hr));

		// Setup the blend state description
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));

		D3D11_RENDER_TARGET_BLEND_DESC rtbd;
		ZeroMemory(&rtbd, sizeof(rtbd));

		rtbd.BlendEnable = true;

		rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.AlphaToCoverageEnable = false;

		rtbd.BlendOp = D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		blendDesc.RenderTarget[0] = rtbd;

		hr = device->CreateBlendState(&blendDesc, &m_blendState);
		OG_ASSERT(!FAILED(hr));

		// Setup the vertex buffer description
		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(UIVertex) * 6;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// Create the vertex buffer
		hr = D3D::GetDevice()->CreateBuffer(&vertexBufferDesc, nullptr, &m_vertexBuffer);
		OG_ASSERT(!FAILED(hr));
	}

	void UIRenderer::CreateShaders()
	{
		ID3D11Device* device = D3D::GetDevice();

		HRESULT hr;
		ID3D10Blob* VSBlob;
		ID3D10Blob* PSBlob;

		// Compile the vertex shader
		hr = D3DCompileFromFile(FileSystem::GetFileNameRelativeToGeneratedDirectory(L"DefaultUI_VS.hlsl").c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS
#ifdef OG_DEBUG 
			| D3DCOMPILE_DEBUG
#endif
			, 0, &VSBlob, nullptr);
		OG_ASSERT(!FAILED(hr));

		// Compile the pixel shader
		hr = D3DCompileFromFile(FileSystem::GetFileNameRelativeToGeneratedDirectory(L"DefaultUI_PS.hlsl").c_str(), nullptr, nullptr, "main", "ps_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS
#ifdef OG_DEBUG 
			| D3DCOMPILE_DEBUG
#endif
			, 0, &PSBlob, nullptr);
		OG_ASSERT(!FAILED(hr));

		// Create the shaders
		hr = device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, &m_vertexShader);
		OG_ASSERT(!FAILED(hr));
		hr = device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, &m_pixelShader);
		OG_ASSERT(!FAILED(hr));

		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "COORDINATE",	0, DXGI_FORMAT_R32G32_FLOAT,    0, 0,							  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "SIZE",       0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "POSITION",   0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// Create the input layout.
		hr = device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), VSBlob->GetBufferPointer(),
			VSBlob->GetBufferSize(), &m_inputLayout);
		OG_ASSERT(!FAILED(hr));

		// Release the blobs as they are not needed anymore
		VSBlob->Release();
		PSBlob->Release();
		VSBlob = nullptr;
		PSBlob = nullptr;
	}

	void UIRenderer::BindVertexBuffer()
	{
		ID3D11DeviceContext* context = D3D::GetDeviceContext();

		ID3D11Buffer* buffers[] = { m_vertexBuffer };
		unsigned int stride[] = { sizeof(UIVertex) };
		unsigned int offset[] = { 0 };
		context->IASetVertexBuffers(0, ARRAYSIZE(buffers), buffers, stride, offset);
	}

	void UIRenderer::BindObjects()
	{
		ID3D11DeviceContext* context = D3D::GetDeviceContext();

		// Set the back buffer and the depth buffer
		ID3D11RenderTargetView* rttRTV = D3D::GetBackBuffer();
		ID3D11DepthStencilState* rttDSS = D3D::GetDepthStencilState();

		context->OMSetDepthStencilState(rttDSS, 1);
		context->OMSetRenderTargets(1, &rttRTV, nullptr);

		ID3D11Buffer* buffer[] = { m_constantBuffer };
		context->VSSetConstantBuffers(0, 1, buffer);

		context->IASetInputLayout(m_inputLayout);

		context->VSSetShader(m_vertexShader, nullptr, 0);
		context->PSSetShader(m_pixelShader, nullptr, 0);

		// Set the sampler state in the pixel shader.
		ID3D11SamplerState* samplers[] = { m_samplerClamp };
		context->PSSetSamplers(0, ARRAYSIZE(samplers), samplers);

		// Set the blend state
		context->OMSetBlendState(m_blendState, nullptr, 0xFFFFFFFF);

		// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}


	void UIRenderer::BindCharTexture(const char c)
	{
		ID3D11DeviceContext* context = D3D::GetDeviceContext();

		context->PSSetShaderResources(0, 1, &m_charToSRVMap[c]);
	}

	void UIRenderer::DeleteObjects()
	{
		if (m_vertexBuffer)
		{
			m_vertexBuffer->Release();
			m_vertexBuffer = nullptr;
		}

		if (m_blendState)
		{
			m_blendState->Release();
			m_blendState = nullptr;
		}

		if (m_samplerClamp)
		{
			m_samplerClamp->Release();
			m_samplerClamp = nullptr;
		}

		if (m_constantBuffer)
		{
			m_constantBuffer->Release();
			m_constantBuffer = nullptr;
		}
	}

	void UIRenderer::DeleteShaders()
	{
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

	void UIRenderer::CreateCharSRVAndAddToMap(const Texture* tex, const char c)
	{

		if (tex->GetTextureData().size.x == 0 || tex->GetTextureData().size.y == 0)
		{
			m_charToSRVMap[c] = nullptr;
			return;
		}

		HRESULT hr;
		ID3D11Device* device = D3D::GetDevice();
		ID3D11Texture2D* texObject;
		ID3D11ShaderResourceView* charSRV;

		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));
		texDesc.Width = static_cast<UINT>(tex->GetTextureData().size.x);
		texDesc.Height = static_cast<UINT>(tex->GetTextureData().size.y);
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R8_UNORM;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_IMMUTABLE;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA texData;
		texData.pSysMem = tex->GetTextureData().data;
		texData.SysMemPitch = texDesc.Width;
		texData.SysMemSlicePitch = 0;

		// Create the texture
		hr = device->CreateTexture2D(&texDesc, &texData, &texObject);
		OG_ASSERT(!FAILED(hr));

		// Create the SRV
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		hr = device->CreateShaderResourceView(texObject, &srvDesc, &charSRV);
		OG_ASSERT(!FAILED(hr));

		// Finally insert the SRV into the map
		m_charToSRVMap[c] = charSRV;
	}

}


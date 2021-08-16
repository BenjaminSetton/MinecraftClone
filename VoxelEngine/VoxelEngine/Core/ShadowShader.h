#ifndef _SHADOW_SHADER
#define _SHADOW_SHADER

// Includes
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#include "ChunkManager.h"


class ShadowShader
{
public:

	void CreateObjects(const WCHAR* vsFilename, const WCHAR* psFilename);

	void Initialize(DirectX::XMFLOAT3 lightDirection, const uint32_t width,
		const uint32_t height);

	void Render();

	void Shutdown();

	void UpdateLightMatrix();

	ID3D11ShaderResourceView* GetShadowMap();

	const DirectX::XMMATRIX GetLightViewMatrix();


private:

	// pipeline vertex struct
	struct MatrixBuffer
	{
		DirectX::XMMATRIX worldMatrix;
		DirectX::XMMATRIX viewMatrix;
		DirectX::XMMATRIX orthoMatrix;
	};

	void CreateD3DObjects();

	void CreateShaders(const WCHAR* vsFilename, const WCHAR* psFilename);

	// Depth buffer has to be resized if window is resized
	void CreateDepthBuffer(const uint32_t width, const uint32_t height);

	void BindVertexBuffer(Chunk* chunk);

	void BindObjects();

private:

	ID3D11Texture2D* m_shadowTex = nullptr;
	ID3D11DepthStencilView* m_depthView = nullptr;
	ID3D11DepthStencilState* m_depthState = nullptr;
	ID3D11ShaderResourceView* m_shadowMapSRV = nullptr;

	// D3D object definitions
	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;

	ID3D11Buffer* m_matrixBuffer = nullptr;

	ID3D11InputLayout* m_inputLayout = nullptr;

	DirectX::XMMATRIX m_lightViewMatrix;

};

#endif


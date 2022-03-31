#ifndef _DEBUG_RENDERER_SHADER_H
#define _DEBUG_RENDERER_SHADER_H

// Includes
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

class DebugRendererShader
{
public:

	void CreateObjects(const WCHAR* vsFilename, const WCHAR* psFilename);

	void Initialize(DirectX::XMMATRIX camViewMatrix);

	void Render();

	void Shutdown();

	void UpdateViewMatrix(DirectX::XMMATRIX viewMatrix);

private:

	void CreateD3DObjects();

	void CreateShaders(const WCHAR* vsFilename, const WCHAR* psFilename);

	void UpdateVertexBuffer();

	void BindObjects();

private:


	// D3D object definitions
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;

	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_vertexBuffer;

	ID3D11InputLayout* m_inputLayout;

private:

	struct MatrixBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX proj;
	};

};

#endif

#ifndef _QUAD_SHADER_H
#define _QUAD_SHADER_H

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

class QuadShader
{
public:

	void CreateObjects(const WCHAR* vsFilename, const WCHAR* psFilename);

	void Initialize();

	void Render();

	void Shutdown();

	void UpdateViewMatrix(DirectX::XMMATRIX viewMatrix);

	void SetQuadTexture(ID3D11ShaderResourceView* quadTexture);

private:

	// pipeline vertex struct
	struct MatrixBuffer
	{
		DirectX::XMMATRIX viewMatrix;
		DirectX::XMMATRIX projectionMatrix;
	};

	void CreateD3DObjects();

	void CreateShaders(const WCHAR* vsFilename, const WCHAR* psFilename);

	void BindVertexBuffers();

	void BindObjects();

private:


	// D3D object definitions
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;

	ID3D11Buffer* m_matrixBuffer;

	ID3D11InputLayout* m_inputLayout;

	ID3D11SamplerState* m_samplerWrap;
	ID3D11SamplerState* m_samplerClamp;

	ID3D11ShaderResourceView* m_quadTexture;

};

#endif
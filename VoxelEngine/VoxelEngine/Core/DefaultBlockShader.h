#ifndef _DEFAULTBLOCKSHADER_H
#define _DEFAULTBLOCKSHADER_H

// Includes
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#include "ChunkManager.h"


// TODO: Track the following rendering stats
//
//	- Number of draw calls per frame
//	- Number of vertices being rendered per frame (so the sum of all chunk vertices being rendered)
//	- Time taken to render? (maybe)
//
//
class DefaultBlockShader
{
public:

	void CreateObjects(const WCHAR* vsFilename, const WCHAR* gsFilename, const WCHAR* psFilename);

	void Initialize(DirectX::XMMATRIX camViewMatrix, DirectX::XMMATRIX lightViewMatrix);
	
	void Render(ID3D11ShaderResourceView* const* srvs);

	void Shutdown();

	void UpdateViewMatrices(DirectX::XMMATRIX camViewMatrix, DirectX::XMMATRIX lightViewMatrix);

	void UpdateLightMatrix();


private:

	// pipeline vertex struct
	struct MatrixBuffer
	{
		DirectX::XMMATRIX worldMatrix;
		DirectX::XMMATRIX viewMatrix;
		DirectX::XMMATRIX projectionMatrix;
		DirectX::XMMATRIX lightViewMatrix;
		DirectX::XMMATRIX lightProjectionMatrix;
	};

	struct LightBuffer
	{
		DirectX::XMFLOAT4 lightDir[2];
		DirectX::XMFLOAT4 lightCol[2];
		DirectX::XMFLOAT4 lightAmbient[2];
	};


	void CreateD3DObjects();

	void CreateShaders(const WCHAR* vsFilename, const WCHAR* gsFilename, const WCHAR* psFilename);

	void BindVertexBuffers();

	void BindObjects(ID3D11ShaderResourceView* const* srvs);

private:


	// D3D object definitions
	ID3D11VertexShader* m_vertexShader;
	ID3D11GeometryShader* m_geometryShader;
	ID3D11PixelShader* m_pixelShader;

	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;

	ID3D11Buffer* m_UVBuffer;

	ID3D11InputLayout* m_inputLayout;

	ID3D11SamplerState* m_samplerWrap;
	ID3D11SamplerState* m_samplerClamp;

	// Stores the transposed projection matrix
	DirectX::XMMATRIX m_camPM;
	DirectX::XMMATRIX m_lightPM;

};


#endif
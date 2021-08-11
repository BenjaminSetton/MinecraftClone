#ifndef _DEFAULTBLOCKSHADER_H
#define _DEFAULTBLOCKSHADER_H

// Includes
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#include "Chunk.h"


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

	void CreateObjects(ID3D11Device* device, const WCHAR* vsFilename, const WCHAR* psFilename);

	void Initialize(ID3D11DeviceContext* context, DirectX::XMMATRIX WM, DirectX::XMMATRIX VM, 
		DirectX::XMMATRIX PM, DirectX::XMFLOAT3 lightDir, DirectX::XMFLOAT4 lightCol, ID3D11ShaderResourceView* srv);
	
	void Render(ID3D11DeviceContext* context);

	void Shutdown();

	
	void SetChunk(Chunk* const chunk);

	const Chunk* GetChunk() const;

	void UpdateViewMatrix(ID3D11DeviceContext* context, DirectX::XMMATRIX viewMatrix);


private:

	// pipeline vertex struct
	struct MatrixBuffer
	{
		DirectX::XMMATRIX worldMatrix;
		DirectX::XMMATRIX viewMatrix;
		DirectX::XMMATRIX projectionMatrix;
	};

	struct LightBuffer
	{
		DirectX::XMFLOAT3 lightDir;
		DirectX::XMFLOAT4 lightCol;
		float padding;
	};


	void CreateD3DObjects(ID3D11Device* device);

	void CreateShaders(ID3D11Device* device, const WCHAR* vsFilename, const WCHAR* psFilename);

	void UpdateVertexBuffer(ID3D11DeviceContext* context);

private:


	// D3D object definitions
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;

	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;

	ID3D11Buffer* m_vertexBuffer;

	ID3D11InputLayout* m_inputLayout;
	ID3D11SamplerState* m_sampler;

	// The chunk currently being rendered
	Chunk* m_chunk = nullptr;

	// Stores the transposed projection matrix
	DirectX::XMMATRIX m_projection;

};


#endif
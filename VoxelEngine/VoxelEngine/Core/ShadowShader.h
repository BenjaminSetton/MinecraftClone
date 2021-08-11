#ifndef _SHADOW_SHADER
#define _SHADOW_SHADER

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#include "Chunk.h"

class ShadowShader
{
public:

	void CreateObjects(ID3D11Device* device, const WCHAR* vsFilename, const WCHAR* psFilename);

	void Render(ID3D11DeviceContext* context, unsigned int indexCount, DirectX::XMMATRIX WM,
		DirectX::XMMATRIX VM, DirectX::XMMATRIX PM, DirectX::XMFLOAT3 lightDir,
		DirectX::XMFLOAT4 lightCol, ID3D11ShaderResourceView* srv);

	void Shutdown();

	void SetChunk(Chunk* const chunk);

private:

	// pipeline vertex struct
	struct CameraMatrixBuffer
	{
		DirectX::XMMATRIX worldMatrix;
		DirectX::XMMATRIX viewMatrix;
		DirectX::XMMATRIX projectionMatrix;
	};

	void SetShaderParameters(ID3D11DeviceContext* context, DirectX::XMMATRIX WM, DirectX::XMMATRIX VM,
		DirectX::XMMATRIX PM, DirectX::XMFLOAT3 lightDir, DirectX::XMFLOAT4 lightCol, ID3D11ShaderResourceView* srv);

	void CreateD3DObjects(ID3D11Device* device);

	void CreateShaders(ID3D11Device* device, const WCHAR* vsFilename, const WCHAR* psFilename);

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

};

#endif


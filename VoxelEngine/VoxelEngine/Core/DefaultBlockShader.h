#ifndef _DEFAULTBLOCKSHADER_H
#define _DEFAULTBLOCKSHADER_H

// Includes
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

class DefaultBlockShader
{
public:

	void CreateObjects(ID3D11Device* device, const WCHAR* vsFilename, const WCHAR* psFilename);

	void Render(ID3D11DeviceContext* context, unsigned int indexCount, DirectX::XMMATRIX WM, 
		DirectX::XMMATRIX VM, DirectX::XMMATRIX PM, DirectX::XMFLOAT3 lightDir, 
		DirectX::XMFLOAT4 lightCol, ID3D11ShaderResourceView* srv);

	void Shutdown();

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

	void SetShaderParameters(ID3D11DeviceContext* context, DirectX::XMMATRIX WM, DirectX::XMMATRIX VM, 
		DirectX::XMMATRIX PM, DirectX::XMFLOAT3 lightDir, DirectX::XMFLOAT4 lightCol, ID3D11ShaderResourceView* srv);

	void CreateD3DObjects(ID3D11Device* device);

	void CreateShaders(ID3D11Device* device, const WCHAR* vsFilename, const WCHAR* psFilename);


	// D3D object definitions
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;

	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;

	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	ID3D11InputLayout* m_inputLayout;
	ID3D11SamplerState* m_sampler;
};


#endif
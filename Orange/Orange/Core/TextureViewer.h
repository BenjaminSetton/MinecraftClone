#ifndef _TEXTURE_VIEWER_H
#define _TEXTURE_VIEWER_H

#include <d3d11.h>
#include <DirectXMath.h>

// Renders a texture to the screen's X/Y position in pixels
class TextureViewer
{
public:

	TextureViewer(ID3D11ShaderResourceView* tex, float x = 0.0f, float y = 0.0f, 
		float scale = 1.0f);
	TextureViewer(const TextureViewer& other) = default;
	~TextureViewer();

	void Render();

	void SetTexture(ID3D11ShaderResourceView* const srv);

private:

	struct QuadVertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 uv;
	};

	void CreateShaders();

	void CreateObjects();

	void BindObjects();

	void CreateVertexBuffer();

private:

	float m_x, m_y;
	float m_scale;
	ID3D11ShaderResourceView* m_texture = nullptr;


	// Other DirectX objects
	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;

	ID3D11SamplerState* m_samplerClamp = nullptr;

	ID3D11InputLayout* m_inputLayout = nullptr;

	ID3D11Buffer* m_vertexBuffer = nullptr;

};

#endif
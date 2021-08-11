#ifndef _SHADOW_SHADER
#define _SHADOW_SHADER

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#include "Chunk.h"

class ShadowShader
{
public:

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


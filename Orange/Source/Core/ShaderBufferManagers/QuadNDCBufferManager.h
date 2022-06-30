#ifndef _QUADNDCBUFFERMANAGER_H
#define _QUADNDCBUFFERMANAGER_H

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

struct QuadNDCVertexData
{
	DirectX::XMFLOAT2	pos;
	DirectX::XMFLOAT2	uv;
	DirectX::XMFLOAT2	NDCPos;
	DirectX::XMFLOAT2	scale;
};

class QuadNDCBufferManager
{
public:

	static void Initialize();
	static void Shutdown();

	static void UpdateBuffers();

	static void Clear();

	static ID3D11Buffer* GetVertexBuffer();

	static std::vector<QuadNDCVertexData>& GetVertexData();

	static void PushVertex(const QuadNDCVertexData& quadData);

private:

	// Stores the transposed transforms
	static std::vector<QuadNDCVertexData> m_vertexData;

	static ID3D11Buffer* m_vertexBuffer;

};

#endif
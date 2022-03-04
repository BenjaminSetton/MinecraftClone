#ifndef _QUADBUFFERMANAGER_H
#define _QUADBUFFERMANAGER_H

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

struct QuadVertexData
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
};

struct QuadInstanceData
{
	const bool operator==(const QuadInstanceData& rhs)
	{
		return  (transform.r[0].m128_f32[0] == rhs.transform.r[0].m128_f32[0]) && (transform.r[0].m128_f32[1] == rhs.transform.r[0].m128_f32[1]) && (transform.r[0].m128_f32[2] == rhs.transform.r[0].m128_f32[2]) && (transform.r[0].m128_f32[3] == rhs.transform.r[0].m128_f32[3]) &&
				(transform.r[1].m128_f32[0] == rhs.transform.r[1].m128_f32[0]) && (transform.r[1].m128_f32[1] == rhs.transform.r[1].m128_f32[1]) && (transform.r[1].m128_f32[2] == rhs.transform.r[1].m128_f32[2]) && (transform.r[1].m128_f32[3] == rhs.transform.r[1].m128_f32[3]) &&
				(transform.r[2].m128_f32[0] == rhs.transform.r[2].m128_f32[0]) && (transform.r[2].m128_f32[1] == rhs.transform.r[2].m128_f32[1]) && (transform.r[2].m128_f32[2] == rhs.transform.r[2].m128_f32[2]) && (transform.r[2].m128_f32[3] == rhs.transform.r[2].m128_f32[3]) &&
				(transform.r[3].m128_f32[0] == rhs.transform.r[3].m128_f32[0]) && (transform.r[3].m128_f32[1] == rhs.transform.r[3].m128_f32[1]) && (transform.r[3].m128_f32[2] == rhs.transform.r[3].m128_f32[2]) && (transform.r[3].m128_f32[3] == rhs.transform.r[3].m128_f32[3]);
	}

	DirectX::XMMATRIX transform;
};

// This class is responsible for managing buffers
// used for rendering by shader classes
class QuadBufferManager
{
public:

	static void Initialize();
	static void Shutdown();

	static void UpdateBuffers();

	static void Clear();

	static ID3D11Buffer* GetVertexBuffer();
	static ID3D11Buffer* GetInstanceBuffer();

	static std::vector<QuadInstanceData>& GetInstanceData();

	static void PushQuad(const QuadInstanceData& quadData);

	static void PopQuad(const QuadInstanceData& quadData);

private:

	// Stores the transposed transforms
	static std::vector<QuadInstanceData> m_instanceData;

	static ID3D11Buffer* m_vertexBuffer;
	static ID3D11Buffer* m_instanceBuffer;

};

#endif
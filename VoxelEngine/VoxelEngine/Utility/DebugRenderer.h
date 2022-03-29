#ifndef _DEBUG_RENDERER_H
#define _DEBUG_RENDERER_H

#include <map>
#include <DirectXMath.h>

class DebugRenderer
{
public:

	struct ColoredVertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
	};

	static void DrawLine(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, const DirectX::XMFLOAT4& startColor, const DirectX::XMFLOAT4& endColor);

	static void DrawLine(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, const DirectX::XMFLOAT4& lineColor);

	static void DrawSphere(const int32_t levelOfDetail, const DirectX::XMFLOAT3& position, const float radius, const DirectX::XMFLOAT4& color);

	static void DrawCircle(const int32_t levelOfDetail, const DirectX::XMFLOAT3& position, const float radius, const DirectX::XMFLOAT4& color);

	static void DrawAABB(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& extents, const DirectX::XMFLOAT4& color);

	static void Clear();

	static const ColoredVertex* GetLineVertices();

	static size_t GetVertexCount();

	static size_t GetVertexCapacity();

	static void SetMaxClearTimer(const float& clearTimer);

	static void DecreaseCurrentClearTimer(const float& dt);

private:

	/////////////////////////////////////////////////
	//
	// RENDERER CLASS DECLARATIONS
	//

	class DebugLine
	{
	public:

		static void AddLine(DirectX::XMFLOAT3 point_a, DirectX::XMFLOAT3 point_b, DirectX::XMFLOAT4 color_a, DirectX::XMFLOAT4 color_b);

		static inline void AddLine(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 q, DirectX::XMFLOAT4 color) { AddLine(p, q, color, color); }

	private:

	};


	// Used to programmatically create a sphere used for debugging
	// Credit goes to author Andreas Kahler for the original C# code
	// Link: http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
	class DebugSphere
	{
	public:

		static void DrawSphere(const int32_t levelOfDetail, const DirectX::XMFLOAT3 pos, const float radius, const DirectX::XMFLOAT4 color);

		static void DrawCircle(const int32_t levelOfDetail, const DirectX::XMFLOAT3 pos, const float radius, const DirectX::XMFLOAT4 color);

	private:

		static void Reset();

		struct TriangleIndices
		{
			int32_t v1, v2, v3;
		};

		struct VertexIndices
		{
			int32_t v1, v2;
		};

		static int32_t AddVertex(DirectX::XMFLOAT3 vert, const float radius);

		static int32_t GetMiddlePoint(const int64_t a, const int64_t b, const float radius, const DirectX::XMFLOAT3 pos);

		static void AddFaceToRenderer(const int32_t ind1, const int32_t ind2, const int32_t ind3, const DirectX::XMFLOAT3 pos, const DirectX::XMFLOAT4 color);

		static std::vector<DirectX::XMFLOAT3> mGeometry;
		static int32_t mIndex;
		static std::map<int64_t, int32_t> mMiddlePointIndexCache;

	};

private:

	static float mClearMaxTimer;

	static float mClearCurrentTimer;

};

#endif


#ifndef _DEBUG_RENDERER_H
#define _DEBUG_RENDERER_H

#include <map>
#include <DirectXMath.h>


class DebugLine
{
public:

	struct ColoredVertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
	};

public:

	static void AddLine(DirectX::XMFLOAT3 point_a, DirectX::XMFLOAT3 point_b, DirectX::XMFLOAT4 color_a, DirectX::XMFLOAT4 color_b);

	static inline void AddLine(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 q, DirectX::XMFLOAT4 color) { AddLine(p, q, color, color); }

	static void ClearLines();

	static const ColoredVertex* GetLineVerts();

	static size_t GetLineVertCount();

	static size_t GetLineVertCapacity();

private:

};


// Used to programmatically create a sphere used for debugging
		// Credit goes to author Andreas Kahler for the original C# code
		// Link: http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
class DebugSphere
{
public:

	/// 
	/// RETURN: N/A
	/// PARAMETERS: NONE
	/// DESCRIPTION: Initializes all member variables
	/// 
	void Initialize();

	///
	/// RETURN: N/A
	/// PARAMETERS: NONE
	/// DESCRIPTION: Destructs all member variables and cleans up memory
	/// 
	void Shutdown();


	/// 
	/// RETURN: void
	/// PARAMETERS: int32_t _levelOfDetail, DirectX::XMFLOAT3 _pos, float _radius, DirectX::XMFLOAT4 _color
	/// DESCRIPTION: Draws a sphere with a given LOD, radius, position and a color
	/// 
	static void DrawSphere(const int32_t _levelOfDetail, const DirectX::XMFLOAT3 _pos, const float _radius, const DirectX::XMFLOAT4 _color);

	/// 
	/// RETURN: void
	/// PARAMETERS: int32_t _levelOfDetail, DirectX::XMFLOAT3 _pos, float _radius, DirectX::XMFLOAT4 _color
	/// DESCRIPTION: Draws a circle with a given LOD, radius, position and a color
	/// 
	static void DrawCircle(const int32_t _levelOfDetail, const DirectX::XMFLOAT3 _pos, const float _radius, const DirectX::XMFLOAT4 _color);

	/// 
	/// RETURN: void
	/// PARAMETERS: NONE
	/// DESCRIPTION: Resets mGeometry, mIndex, and mMiddlePointIndexCache
	/// 
	static void Reset();

private:

	struct TriangleIndices
	{
		int32_t v1, v2, v3;
	};

	struct VertexIndices
	{
		int32_t v1, v2;
	};

	/// 
	/// RETURN: int32_t
	/// PARAMETERS: DirectX::XMFLOAT3 _vert, const float _radius, const DirectX::XMFLOAT3 _pos
	/// DESCRIPTION: Adds a vertex to the vector of vertices with _radius and _pos
	/// 
	static int32_t AddVertex(DirectX::XMFLOAT3 _vert, const float _radius, const DirectX::XMFLOAT3 _pos);

	/// 
	/// RETURN: int32_t
	/// PARAMETERS: NONE
	/// DESCRIPTION: Returns the middle point between two points
	/// 
	static int32_t GetMiddlePoint(const int64_t _a, const int64_t _b, const float _radius, const DirectX::XMFLOAT3 _pos);

	/// 
	/// RETURN: void
	/// PARAMETERS: const int32_t ind1, const int32_t ind2, const int32_t ind3, DirectX::XMFLOAT4 _color
	/// DESCRIPTION: Adds the face of the sphere to the debug renderer for drawing
	/// 
	static void AddFaceToRenderer(const int32_t ind1, const int32_t ind2, const int32_t ind3, const DirectX::XMFLOAT3 _pos, const DirectX::XMFLOAT4 _color);

	static std::vector<DirectX::XMFLOAT3> mGeometry;
	static int32_t mIndex;
	static std::map<int64_t, int32_t> mMiddlePointIndexCache;

};

#endif


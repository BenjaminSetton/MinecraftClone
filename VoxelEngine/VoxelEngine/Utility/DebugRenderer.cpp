#include "../Misc/pch.h"
#include "DebugRenderer.h"

// Anonymous namespace
namespace
{
	// Declarations in an anonymous namespace are global BUT only have internal linkage.
	// In other words, these variables are global but are only visible in this source file.

	// Maximum number of debug lines at one time (i.e: Capacity)
	constexpr size_t MAX_LINE_VERTS = 194000;

	// CPU-side buffer of debug-line verts
	// Copied to the GPU and reset every frame.
	size_t line_vert_count = 0;	
	std::array<DebugLine::ColoredVertex, MAX_LINE_VERTS> line_verts;
}

void DebugLine::AddLine(DirectX::XMFLOAT3 ptA, DirectX::XMFLOAT3 ptB, DirectX::XMFLOAT4 clA, DirectX::XMFLOAT4 clB)
{
	// Add points to debug_verts, increments debug_vert_count
	line_verts[line_vert_count].pos = ptA;
	line_verts[line_vert_count].color = clA;
	line_vert_count++;
	line_verts[line_vert_count].pos = ptB;
	line_verts[line_vert_count].color = clB;
	line_vert_count++;
}

void DebugLine::ClearLines()
{
	// Resets debug_vert_count
	line_verts.fill(ColoredVertex());
	line_vert_count = 0;
}

DebugLine::ColoredVertex* DebugLine::GetLineVerts()
{
	return line_verts.data();
}

size_t DebugLine::GetLineVertCount()
{
	return line_vert_count;
}

size_t DebugLine::GetLineVertCapacity()
{
	return MAX_LINE_VERTS;
}



////////////////////////////////////////////
///
///		DebugSphere methods
/// 
////////////////////////////////////////////

std::vector<DirectX::XMFLOAT3> DebugSphere::mGeometry = std::vector<DirectX::XMFLOAT3>();
int32_t DebugSphere::mIndex = 0;
std::map<int64_t, int32_t> DebugSphere::mMiddlePointIndexCache = std::map<int64_t, int32_t>();

void DebugSphere::Initialize() {}

void DebugSphere::Shutdown()
{
	Reset();
	// No dynamic memory to deal with
}

int32_t DebugSphere::AddVertex(DirectX::XMFLOAT3 _vert, const float _radius, const DirectX::XMFLOAT3 _pos)
{
	float length = sqrt(_vert.x * _vert.x + _vert.y * _vert.y + _vert.z * _vert.z);
	DirectX::XMFLOAT3 normPos = { _vert.x / length, _vert.y / length, _vert.z / length };
	DirectX::XMFLOAT3 newPos = { normPos.x * _radius, normPos.y * _radius, normPos.z * _radius };
	mGeometry.push_back(newPos);
	return mIndex++;
}


int32_t DebugSphere::GetMiddlePoint(const int64_t _a, const int64_t _b, const float _radius, const DirectX::XMFLOAT3 _pos)
{
	// Check map to see if value is already stored there
	bool firstIsSmaller = _a < _b;
	int64_t smallIndex = firstIsSmaller ? _a : _b;
	int64_t bigIndex = firstIsSmaller ? _b : _a;
	int64_t mapKey = (smallIndex << 32) + bigIndex;

	auto iter = mMiddlePointIndexCache.find(mapKey);
	if (iter != mMiddlePointIndexCache.end())
	{
		// We found the middle point already stored inside the map, so just return it
		return mMiddlePointIndexCache[iter->first];
	}
	// If it's not in the cache, calculate it

	DirectX::XMFLOAT3 p1 = mGeometry[_a];
	DirectX::XMFLOAT3 p2 = mGeometry[_b];
	DirectX::XMFLOAT3 midPoint =
	{
		(p1.x + p2.x) / 2.0f,
		(p1.y + p2.y) / 2.0f,
		(p1.z + p2.z) / 2.0f
	};

	// Add to geometry vector
	int32_t index = AddVertex(midPoint, _radius, _pos);

	// Store it in map for future reference, saving a calculation
	mMiddlePointIndexCache[mapKey] = index;

	// Return it's index in the mGeometry vector
	return index;

}

void DebugSphere::AddFaceToRenderer(const int32_t ind1, const int32_t ind2, const int32_t ind3, const DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT4 _color)
{
	DebugLine::AddLine
	(
		{ mGeometry[ind1].x + _pos.x, mGeometry[ind1].y + _pos.y, mGeometry[ind1].z + _pos.z },
		{ mGeometry[ind2].x + _pos.x, mGeometry[ind2].y + _pos.y, mGeometry[ind2].z + _pos.z },
		_color
	);
	DebugLine::AddLine
	(
		{ mGeometry[ind2].x + _pos.x, mGeometry[ind2].y + _pos.y, mGeometry[ind2].z + _pos.z },
		{ mGeometry[ind3].x + _pos.x, mGeometry[ind3].y + _pos.y, mGeometry[ind3].z + _pos.z },
		_color
	);
	DebugLine::AddLine
	(
		{ mGeometry[ind1].x + _pos.x, mGeometry[ind1].y + _pos.y, mGeometry[ind1].z + _pos.z },
		{ mGeometry[ind3].x + _pos.x, mGeometry[ind3].y + _pos.y, mGeometry[ind3].z + _pos.z },
		_color
	);
}

void DebugSphere::DrawSphere(const int32_t _levelOfDetail, const DirectX::XMFLOAT3 _pos, const float _radius, const DirectX::XMFLOAT4 _color)
{
	// Reset all member variables
	Reset();

	// Golden ratio variable used to generate spherical shape
	float t = (1.0f + sqrt(5.0f)) / 2.0f;

	std::vector<TriangleIndices> faces;

	// Create the initial vertices of the sphere
	DirectX::XMFLOAT3 position = { -1.0f, t, 0 };
	AddVertex(position, _radius, _pos);
	position = { 1.0f, t, 0 };
	AddVertex(position, _radius, _pos);
	position = { -1.0f, -t, 0 };
	AddVertex(position, _radius, _pos);
	position = { 1.0f, -t, 0 };
	AddVertex(position, _radius, _pos);

	position = { 0, -1.0f, t };
	AddVertex(position, _radius, _pos);
	position = { 0, 1.0f, t };
	AddVertex(position, _radius, _pos);
	position = { 0, -1.0f, -t };
	AddVertex(position, _radius, _pos);
	position = { 0, 1.0f, -t };
	AddVertex(position, _radius, _pos);

	position = { t, 0, -1.0f };
	AddVertex(position, _radius, _pos);
	position = { t, 0, 1.0f };
	AddVertex(position, _radius, _pos);
	position = { -t, 0, -1.0f };
	AddVertex(position, _radius, _pos);
	position = { -t, 0, 1.0f };
	AddVertex(position, _radius, _pos);

	// Add the faces of the sphere to the debug renderer

		// Five faces around first chosen point (point 0)
	faces.push_back({ 0, 11, 5 });
	faces.push_back({ 0, 5, 1 });
	faces.push_back({ 0, 1, 7 });
	faces.push_back({ 0, 7, 10 });
	faces.push_back({ 0, 10, 11 });
	// Five adjacent faces
	faces.push_back({ 1, 5, 9 });
	faces.push_back({ 5, 11, 4 });
	faces.push_back({ 11, 10, 2 });
	faces.push_back({ 10, 7, 6 });
	faces.push_back({ 7, 1, 8 });
	// Five faces around point 3
	faces.push_back({ 3, 9, 4 });
	faces.push_back({ 3, 4, 2 });
	faces.push_back({ 3, 2, 6 });
	faces.push_back({ 3, 6, 8 });
	faces.push_back({ 3, 8, 9 });
	// Five adjacent faces
	faces.push_back({ 4, 9, 5 });
	faces.push_back({ 2, 4, 11 });
	faces.push_back({ 6, 2, 10 });
	faces.push_back({ 8, 6, 7 });
	faces.push_back({ 9, 8, 1 });

	// Refine the triangles with _levelOfDetail

	for (int i = 0; i < _levelOfDetail; i++)
	{
		std::vector<TriangleIndices> subfaces;
		for (auto tri : faces)
		{
			int32_t a = GetMiddlePoint(tri.v1, tri.v2, _radius, _pos);
			int32_t b = GetMiddlePoint(tri.v2, tri.v3, _radius, _pos);
			int32_t c = GetMiddlePoint(tri.v1, tri.v3, _radius, _pos);

			subfaces.push_back({ tri.v1, a, c });
			subfaces.push_back({ tri.v2, b, a });
			subfaces.push_back({ tri.v3, c, b });
			subfaces.push_back({ a, b, c });
		}
		faces = subfaces;
	}

	// Loop through the faces and draw them using the debug renderer
	for (auto tri : faces)
	{
		AddFaceToRenderer(tri.v1, tri.v2, tri.v3, _pos, _color);
	}

}

void DebugSphere::DrawCircle(const int32_t _levelOfDetail, const DirectX::XMFLOAT3 _pos, const float _radius, const DirectX::XMFLOAT4 _color)
{
	// Reset all member variables
	Reset();

	// Golden ratio variable used to generate spherical shape
	float t = (1.0f + sqrt(5.0f)) / 2.0f;

	std::vector<VertexIndices> verts;

	// Create the initial vertices of the sphere
	DirectX::XMFLOAT3 position;

	position = { 0, 0, -1.0f };
	AddVertex(position, _radius, _pos);
	position = { 1.0f, 0, -1.0f };
	AddVertex(position, _radius, _pos);
	position = { 1.0f, 0, 0 };
	AddVertex(position, _radius, _pos);
	position = { 1.0f, 0, 1.0f };
	AddVertex(position, _radius, _pos);
	position = { 0, 0, 1.0f };
	AddVertex(position, _radius, _pos);
	position = { -1.0f, 0, 1.0f };
	AddVertex(position, _radius, _pos);
	position = { -1.0f, 0, 0 };
	AddVertex(position, _radius, _pos);
	position = { -1.0f, 0, -1.0f };
	AddVertex(position, _radius, _pos);


	// Push back first 8 verts
	verts.push_back({ 0, 1 });
	verts.push_back({ 1, 2 });
	verts.push_back({ 2, 3 });
	verts.push_back({ 3, 4 });
	verts.push_back({ 4, 5 });
	verts.push_back({ 5, 6 });
	verts.push_back({ 6, 7 });
	verts.push_back({ 7, 0 });

	for (int i = 0; i < _levelOfDetail; i++)
	{
		std::vector<VertexIndices> subverts;
		for (auto vert : verts)
		{
			int32_t midPoint = GetMiddlePoint(vert.v1, vert.v2, _radius, _pos);

			subverts.push_back({ vert.v1, midPoint });
			subverts.push_back({ midPoint, vert.v2 });
		}
		verts = subverts;
	}

	// Loop through the faces and draw them using the debug renderer
	for (auto vert : verts)
	{
		DebugLine::AddLine
		(
			{ mGeometry[vert.v1].x + _pos.x, mGeometry[vert.v1].y + _pos.y, mGeometry[vert.v1].z + _pos.z },
			{ mGeometry[vert.v2].x + _pos.x, mGeometry[vert.v2].y + _pos.y, mGeometry[vert.v2].z + _pos.z },
			_color
		);
	}
}

void DebugSphere::Reset()
{
	mIndex = 0;
	mMiddlePointIndexCache.clear();
	mGeometry.clear();
}
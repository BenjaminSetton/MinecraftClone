#include "../Misc/pch.h"
#include "DebugRenderer.h"

// DEBUG PURPOSES ONLY
#include "Input.h"

// Anonymous namespace
namespace
{
	// Declarations in an anonymous namespace are global BUT only have internal linkage.
	// In other words, these variables are global but are only visible in this source file.

	// Maximum number of debug lines at one time (i.e: Capacity)
	constexpr size_t MAX_LINE_VERTS = 250000;

	// CPU-side buffer of debug-line verts
	// Copied to the GPU and reset every frame.
	size_t line_vert_count = 0;	
	std::array<DebugRenderer::ColoredVertex, MAX_LINE_VERTS> line_verts;
}


float DebugRenderer::mClearMaxTimer = 10.0f;

float DebugRenderer::mClearCurrentTimer = mClearMaxTimer;


void DebugRenderer::DrawLine(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, const DirectX::XMFLOAT4& startColor, const DirectX::XMFLOAT4& endColor)
{
	DebugLine::AddLine(startPosition, endPosition, startColor, endColor);
}

void DebugRenderer::DrawLine(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, const DirectX::XMFLOAT4& lineColor)
{
	DebugRenderer::DrawLine(startPosition, endPosition, lineColor, lineColor);
}

void DebugRenderer::DrawSphere(const int32_t levelOfDetail, const DirectX::XMFLOAT3& position, const float radius, const DirectX::XMFLOAT4& color)
{
	DebugSphere::DrawSphere(levelOfDetail, position, radius, color);
}

void DebugRenderer::DrawCircle(const int32_t levelOfDetail, const DirectX::XMFLOAT3& position, const float radius, const DirectX::XMFLOAT4& color)
{
	DebugSphere::DrawCircle(levelOfDetail, position, radius, color);
}

void DebugRenderer::DrawAABB(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& extents, const DirectX::XMFLOAT4& color)
{
	DirectX::XMFLOAT3 trb = { center.x + extents.x, center.y + extents.y, center.z + extents.z };
	DirectX::XMFLOAT3 tlb = { center.x - extents.x, center.y + extents.y, center.z + extents.z };
	DirectX::XMFLOAT3 trf = { center.x + extents.x, center.y + extents.y, center.z - extents.z };
	DirectX::XMFLOAT3 tlf = { center.x - extents.x, center.y + extents.y, center.z - extents.z };
	DirectX::XMFLOAT3 brb = { center.x + extents.x, center.y - extents.y, center.z + extents.z };
	DirectX::XMFLOAT3 blb = { center.x - extents.x, center.y - extents.y, center.z + extents.z };
	DirectX::XMFLOAT3 brf = { center.x + extents.x, center.y - extents.y, center.z - extents.z };
	DirectX::XMFLOAT3 blf = { center.x - extents.x, center.y - extents.y, center.z - extents.z };

	// top square
	DrawLine(trb, tlb, color);
	DrawLine(tlb, tlf, color);
	DrawLine(tlf, trf, color);
	DrawLine(trf, trb, color);

	// left
	DrawLine(tlf, blf, color);
	DrawLine(blf, blb, color);
	DrawLine(blb, tlb, color);

	// right
	DrawLine(trf, brf, color);
	DrawLine(brf, brb, color);
	DrawLine(brb, trb, color);

	DrawLine(blf, brf, color);
	DrawLine(blb, brb, color);

}

void DebugRenderer::Clear()
{
	// Resets debug_vert_count
	line_verts.fill(ColoredVertex());
	line_vert_count = 0;
}

const DebugRenderer::ColoredVertex* DebugRenderer::GetLineVertices()
{
	return line_verts.data();
}

size_t DebugRenderer::GetVertexCount()
{
	return line_vert_count;
}

size_t DebugRenderer::GetVertexCapacity()
{
	return MAX_LINE_VERTS;
}

void DebugRenderer::SetMaxClearTimer(const float& clearTimer) 
{ 
	mClearMaxTimer = clearTimer;
	mClearCurrentTimer = clearTimer;
}

void DebugRenderer::DecreaseCurrentClearTimer(const float& dt) 
{
	UNREFERENCED_PARAMETER(dt);
	//mClearCurrentTimer -= dt;
	//if (mClearCurrentTimer <= 0)
	//{
	//	mClearCurrentTimer += mClearMaxTimer;
	//	//Clear();
	//}

	//if(Input::IsKeyDown(KeyCode::X)) Clear();
}

void DebugRenderer::DebugLine::AddLine(DirectX::XMFLOAT3 ptA, DirectX::XMFLOAT3 ptB, DirectX::XMFLOAT4 clA, DirectX::XMFLOAT4 clB)
{
	if (line_vert_count >= MAX_LINE_VERTS) return;

	line_verts[line_vert_count].pos = ptA;
	line_verts[line_vert_count].color = clA;
	line_vert_count++;
	line_verts[line_vert_count].pos = ptB;
	line_verts[line_vert_count].color = clB;
	line_vert_count++;
}



////////////////////////////////////////////
///
///		DebugSphere methods
/// 
////////////////////////////////////////////

std::vector<DirectX::XMFLOAT3> DebugRenderer::DebugSphere::mGeometry = std::vector<DirectX::XMFLOAT3>();
int32_t DebugRenderer::DebugSphere::mIndex = 0;
std::map<int64_t, int32_t> DebugRenderer::DebugSphere::mMiddlePointIndexCache = std::map<int64_t, int32_t>();

int32_t DebugRenderer::DebugSphere::AddVertex(DirectX::XMFLOAT3 vert, const float radius)
{
	float length = sqrt(vert.x * vert.x + vert.y * vert.y + vert.z * vert.z);
	DirectX::XMFLOAT3 normPos = { vert.x / length, vert.y / length, vert.z / length };
	DirectX::XMFLOAT3 newPos = { normPos.x * radius, normPos.y * radius, normPos.z * radius };
	mGeometry.push_back(newPos);
	return mIndex++;
}


int32_t DebugRenderer::DebugSphere::GetMiddlePoint(const int64_t _a, const int64_t _b, const float _radius, const DirectX::XMFLOAT3 _pos)
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
	int32_t index = AddVertex(midPoint, _radius);

	// Store it in map for future reference, saving a calculation
	mMiddlePointIndexCache[mapKey] = index;

	// Return it's index in the mGeometry vector
	return index;

}

void DebugRenderer::DebugSphere::AddFaceToRenderer(const int32_t ind1, const int32_t ind2, const int32_t ind3, const DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 color)
{
	DebugLine::AddLine
	(
		{ mGeometry[ind1].x + pos.x, mGeometry[ind1].y + pos.y, mGeometry[ind1].z + pos.z },
		{ mGeometry[ind2].x + pos.x, mGeometry[ind2].y + pos.y, mGeometry[ind2].z + pos.z },
		color
	);
	DebugLine::AddLine
	(
		{ mGeometry[ind2].x + pos.x, mGeometry[ind2].y + pos.y, mGeometry[ind2].z + pos.z },
		{ mGeometry[ind3].x + pos.x, mGeometry[ind3].y + pos.y, mGeometry[ind3].z + pos.z },
		color
	);
	DebugLine::AddLine
	(
		{ mGeometry[ind1].x + pos.x, mGeometry[ind1].y + pos.y, mGeometry[ind1].z + pos.z },
		{ mGeometry[ind3].x + pos.x, mGeometry[ind3].y + pos.y, mGeometry[ind3].z + pos.z },
		color
	);
}

void DebugRenderer::DebugSphere::DrawSphere(const int32_t levelOfDetail, const DirectX::XMFLOAT3 pos, const float radius, const DirectX::XMFLOAT4 color)
{
	// Reset all member variables
	Reset();

	// Golden ratio variable used to generate spherical shape
	float t = (1.0f + sqrt(5.0f)) / 2.0f;

	std::vector<TriangleIndices> faces;

	// Create the initial vertices of the sphere
	DirectX::XMFLOAT3 position = { -1.0f, t, 0 };
	AddVertex(position, radius);
	position = { 1.0f, t, 0 };
	AddVertex(position, radius);
	position = { -1.0f, -t, 0 };
	AddVertex(position, radius);
	position = { 1.0f, -t, 0 };
	AddVertex(position, radius);

	position = { 0, -1.0f, t };
	AddVertex(position, radius);
	position = { 0, 1.0f, t };
	AddVertex(position, radius);
	position = { 0, -1.0f, -t };
	AddVertex(position, radius);
	position = { 0, 1.0f, -t };
	AddVertex(position, radius);

	position = { t, 0, -1.0f };
	AddVertex(position, radius);
	position = { t, 0, 1.0f };
	AddVertex(position, radius);
	position = { -t, 0, -1.0f };
	AddVertex(position, radius);
	position = { -t, 0, 1.0f };
	AddVertex(position, radius);

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

	for (int i = 0; i < levelOfDetail; i++)
	{
		std::vector<TriangleIndices> subfaces;
		for (auto tri : faces)
		{
			int32_t a = GetMiddlePoint(tri.v1, tri.v2, radius, pos);
			int32_t b = GetMiddlePoint(tri.v2, tri.v3, radius, pos);
			int32_t c = GetMiddlePoint(tri.v1, tri.v3, radius, pos);

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
		AddFaceToRenderer(tri.v1, tri.v2, tri.v3, pos, color);
	}

}

void DebugRenderer::DebugSphere::DrawCircle(const int32_t levelOfDetail, const DirectX::XMFLOAT3 pos, const float radius, const DirectX::XMFLOAT4 color)
{
	// Reset all member variables
	Reset();

	std::vector<VertexIndices> circleVertices;

	// Create the initial vertices of the sphere
	DirectX::XMFLOAT3 position;

	position = { 0, 0, -1.0f };
	AddVertex(position, radius);
	position = { 1.0f, 0, -1.0f };
	AddVertex(position, radius);
	position = { 1.0f, 0, 0 };
	AddVertex(position, radius);
	position = { 1.0f, 0, 1.0f };
	AddVertex(position, radius);
	position = { 0, 0, 1.0f };
	AddVertex(position, radius);
	position = { -1.0f, 0, 1.0f };
	AddVertex(position, radius);
	position = { -1.0f, 0, 0 };
	AddVertex(position, radius);
	position = { -1.0f, 0, -1.0f };
	AddVertex(position, radius);


	// Push back first 8 verts
	circleVertices.push_back({ 0, 1 });
	circleVertices.push_back({ 1, 2 });
	circleVertices.push_back({ 2, 3 });
	circleVertices.push_back({ 3, 4 });
	circleVertices.push_back({ 4, 5 });
	circleVertices.push_back({ 5, 6 });
	circleVertices.push_back({ 6, 7 });
	circleVertices.push_back({ 7, 0 });

	for (int i = 0; i < levelOfDetail; i++)
	{
		std::vector<VertexIndices> subverts;
		for (auto vertex : circleVertices)
		{
			int32_t midPoint = GetMiddlePoint(vertex.v1, vertex.v2, radius, pos);

			subverts.push_back({ vertex.v1, midPoint });
			subverts.push_back({ midPoint, vertex.v2 });
		}
		circleVertices = subverts;
	}

	// Loop through the faces and draw them using the debug renderer
	for (auto vertex : circleVertices)
	{
		DebugLine::AddLine
		(
			{ mGeometry[vertex.v1].x + pos.x, mGeometry[vertex.v1].y + pos.y, mGeometry[vertex.v1].z + pos.z },
			{ mGeometry[vertex.v2].x + pos.x, mGeometry[vertex.v2].y + pos.y, mGeometry[vertex.v2].z + pos.z },
			color
		);
	}
}

void DebugRenderer::DebugSphere::Reset()
{
	mIndex = 0;
	mMiddlePointIndexCache.clear();
	mGeometry.clear();
}
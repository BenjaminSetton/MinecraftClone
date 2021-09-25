#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "Chunk.h"

#define PASS_STRADDLING_CHUNKS 1

struct Sphere
{
	DirectX::XMFLOAT3 center;
	float radius;
};

struct AABB
{
	DirectX::XMFLOAT3 center, extent;
};

struct Plane
{
	DirectX::XMFLOAT3 normal;
	float point;
};

enum Frustum_Planes
{
	LEFT,
	RIGHT,
	TOP,
	BOTTOM,
	FRONT,
	BACK,
	COUNT
};

struct Frustum
{
	// Stores 6 planes that define a frustum
	std::array<Plane, Frustum_Planes::COUNT> planes;
};

class FrustumCulling
{
public:

	FrustumCulling() = default;
	FrustumCulling(const FrustumCulling& other) = delete;
	~FrustumCulling() = default;

	void SetFrustum(const Frustum& frustum);
	const Frustum GetFrustum();

	// Returns TRUE is visible
	// Returns FALSE is not visible
	bool CalculateChunkAgainstFrustum(Chunk* chunk);

	void CalculateFrustum(float FOV, float aspectRatio, float nearPlane, float farPlane, const DirectX::XMMATRIX& viewMatrix);

private:

	const AABB ConvertChunkToAABB(Chunk* _chunk);

	int TestAABBAgainstPlane(const AABB& aabb, const Plane& plane);

	int TestSphereAgainstPlane(const Sphere& sphere, const Plane& plane);

	Plane CalculatePlaneFromPoints(const XMVECTOR& a, const XMVECTOR& b, const XMVECTOR& c);

private:

	Frustum m_frustum;
};


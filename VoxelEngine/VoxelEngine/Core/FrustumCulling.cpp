#include "../Misc/pch.h"
#include "FrustumCulling.h"

using namespace DirectX;

void FrustumCulling::SetFrustum(const Frustum& frustum) { m_frustum = frustum; }
const Frustum FrustumCulling::GetFrustum() { return m_frustum; }

bool FrustumCulling::CalculateChunkAgainstFrustum(Chunk* chunk)
{
	// Convert Chunk* to AABB to test against frustum
	AABB aabb = ConvertChunkToAABB(chunk);

	// Test the AABB against the frustum and return the result
#ifdef PASS_STRADDLING_CHUNKS 1
	if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::BACK]) < 0) return false;
	if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::BOTTOM]) < 0) return false;
	if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::FRONT]) < 0) return false;
	if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::LEFT]) < 0) return false;
	if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::RIGHT]) < 0) return false;
	if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::TOP]) < 0) return false;
	return true;
#elif
	if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::BACK]) > 0) return false;
	if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::BOTTOM]) > 0) return false;
	if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::FRONT]) > 0) return false;
	if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::LEFT]) > 0) return false;
	if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::RIGHT]) > 0) return false;
	if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::TOP]) > 0) return false;
	return true;
#endif
}

void FrustumCulling::CalculateFrustum(float FOV, float aspectRatio, float nearPlane, float farPlane, const DirectX::XMMATRIX& viewMatrix)
{
	XMVECTOR cameraPos = viewMatrix.r[3];

	//Project the centers outward
	XMVECTOR nearCenter = cameraPos + viewMatrix.r[2] * nearPlane;
	XMVECTOR farCenter = cameraPos + viewMatrix.r[2] * farPlane;

	//Variables needed to calculate the values
	float halfHeightNear = tan(FOV / 2.0f) * nearPlane;
	float halfHeightFar = tan(FOV / 2.0f) * farPlane;
	float halfWidthNear = halfHeightNear * aspectRatio;
	float halfWidthFar = halfHeightFar * aspectRatio;

	//All possible points of frustum
	XMVECTOR zeroVector = { 0.0f, 0.0f, 0.0f, 0.0f};
	XMVECTOR topLeftNear = nearCenter + (viewMatrix.r[1] * halfHeightNear - viewMatrix.r[0] * halfWidthNear);
	XMVECTOR topRightNear = nearCenter + (viewMatrix.r[1] * halfHeightNear + viewMatrix.r[0] * halfWidthNear);
	XMVECTOR botRightNear = nearCenter + (zeroVector - (viewMatrix.r[1] * halfHeightNear) + viewMatrix.r[0] * halfWidthNear);
	XMVECTOR botLeftNear = nearCenter + (zeroVector - (viewMatrix.r[1] * halfHeightNear) - viewMatrix.r[0] * halfWidthNear);
	XMVECTOR topLeftFar = farCenter + (viewMatrix.r[1] * halfHeightFar - viewMatrix.r[0] * halfWidthFar);
	XMVECTOR topRightFar = farCenter + (viewMatrix.r[1] * halfHeightFar + viewMatrix.r[0] * halfWidthFar);
	XMVECTOR botRightFar = farCenter + (zeroVector - (viewMatrix.r[1] * halfHeightFar) + viewMatrix.r[0] * halfWidthFar);
	XMVECTOR botLeftFar = farCenter + (zeroVector - (viewMatrix.r[1] * halfHeightFar) - viewMatrix.r[0] * halfWidthFar);

	//Near plane
	m_frustum.planes[Frustum_Planes::FRONT] = CalculatePlaneFromPoints(topLeftNear, topRightNear, botRightNear);
	//Far plane
	m_frustum.planes[Frustum_Planes::BACK] = CalculatePlaneFromPoints(botRightFar, topRightFar, topLeftFar);
	//Left plane
	m_frustum.planes[Frustum_Planes::LEFT] = CalculatePlaneFromPoints(botLeftFar, topLeftFar, topLeftNear);
	//Right plane
	m_frustum.planes[Frustum_Planes::RIGHT] = CalculatePlaneFromPoints(botRightFar, botRightNear, topRightNear);
	//Up plane
	m_frustum.planes[Frustum_Planes::TOP] = CalculatePlaneFromPoints(topLeftFar, topRightFar, topRightNear);
	//Down plane
	m_frustum.planes[Frustum_Planes::BOTTOM] = CalculatePlaneFromPoints(botRightFar, botLeftFar, botLeftNear);
}

const AABB FrustumCulling::ConvertChunkToAABB(Chunk* _chunk)
{
	XMFLOAT3 chunkPos = _chunk->GetPosition();
	AABB aabb;
	aabb.center = { chunkPos.x + CHUNK_SIZE / 2.0f, chunkPos.y - CHUNK_SIZE / 2.0f, chunkPos.z + CHUNK_SIZE / 2.0f };
	aabb.extent = { chunkPos.x - aabb.center.x, chunkPos.y - aabb.center.y, chunkPos.z - aabb.center.z };
	return aabb;
}

int FrustumCulling::TestAABBAgainstPlane(const AABB& aabb, const Plane& plane)
{
	float projectedRadius = aabb.extent.x * abs(plane.normal.x) + aabb.extent.y * abs(plane.normal.y) + aabb.extent.z * abs(plane.normal.z);
	XMFLOAT3 projSpherePos = aabb.center;
	Sphere testSphere = { projSpherePos, projectedRadius };
	return TestSphereAgainstPlane(testSphere, plane);
}

int FrustumCulling::TestSphereAgainstPlane(const Sphere& sphere, const Plane& plane)
{
	float sphereDist = XMVector3Dot(XMLoadFloat3(&sphere.center), XMLoadFloat3(&plane.normal)).m128_f32[0] - plane.point;

	if (sphereDist > sphere.radius) return 1;
	else if (sphereDist < -sphere.radius) return -1;
	else return 0;
}

Plane FrustumCulling::CalculatePlaneFromPoints(const XMVECTOR& a, const XMVECTOR& b, const XMVECTOR& c)
{
	Plane plane;

	//Considering B is the "center" of the triangle
	XMVECTOR vecOne = XMVector3Normalize(a - b);
	XMVECTOR vecTwo = XMVector3Normalize(c - b);
	XMVECTOR normal = XMVector3Cross(vecOne, vecTwo);

	XMStoreFloat3(&plane.normal, normal);

	//The offset is the dot product between a triangle vertex and the normal
	plane.point = XMVector3Dot(normal, b).m128_f32[0];

	return plane;
}
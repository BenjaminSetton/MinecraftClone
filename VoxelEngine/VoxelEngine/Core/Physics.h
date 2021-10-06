#ifndef _PHYSICS_H
#define _PHYSICS_H

#include <DirectXMath.h>

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

// Helper physics class
class Physics
{
public:

	static void ApplyVelocity(DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& vel, const float& dt);
	static void ApplyAcceleration(DirectX::XMVECTOR& vel, const DirectX::XMVECTOR& accel, const float& dt);
	static void ApplyGravity(DirectX::XMVECTOR& vel, const float& dt);

	static const bool DetectCollision(const DirectX::XMVECTOR& pos);

	static const bool DetectCollision(const AABB& aabb);

private:

};

#endif // _PHYSICS_H

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

	static void ApplyVelocity(DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& vel, const float& dt);
	static void ApplyAcceleration(DirectX::XMFLOAT3& vel, const DirectX::XMFLOAT3& accel, const float& dt);
	static void ApplyGravity(DirectX::XMFLOAT3& vel, const float& dt);

	static void ConvertPositionToVelocity(DirectX::XMFLOAT3& vel, const DirectX::XMFLOAT3& pos, const float& dt);
	static void ConvertVelocityToAcceleration(DirectX::XMFLOAT3& accel, const DirectX::XMFLOAT3& vel, const float& dt);
	static void ConvertPositionToAcceleration(DirectX::XMFLOAT3& accel, const DirectX::XMFLOAT3& pos, const float& dt);

	static const bool DetectCollision(const DirectX::XMFLOAT3& pos);

	static const bool DetectCollision(const AABB& aabb, std::vector<DirectX::XMFLOAT3>* out_collisionPositions = nullptr);

private:

};

#endif // _PHYSICS_H

#include "../Misc/pch.h"
#include "Physics.h"

#include "ChunkManager.h"
#include "../Utility/Utility.h"

using namespace DirectX;

void Physics::ApplyVelocity(DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& vel, const float& dt)
{
	//pos =
	//{
	//	pos.m128_f32[0] + (vel.m128_f32[0] * dt),
	//	pos.m128_f32[1] + (vel.m128_f32[1] * dt),
	//	pos.m128_f32[3] + (vel.m128_f32[2] * dt),
	//	1.0f
	//};
	pos += XMVectorScale(vel, dt);
}

void Physics::ApplyAcceleration(DirectX::XMVECTOR& vel, const DirectX::XMVECTOR& accel, const float& dt)
{
	//vel =
	//{
	//	vel.m128_f32[0] + (accel.m128_f32[0] * dt),
	//	vel.m128_f32[1] + (accel.m128_f32[1] * dt),
	//	vel.m128_f32[2] + (accel.m128_f32[2] * dt),
	//	1.0f
	//};
	vel += XMVectorScale(accel, dt);
}
void Physics::ApplyGravity(DirectX::XMVECTOR& vel, const float& dt)
{
	const XMVECTOR gravityVector = { 0.0f, -9.8f, 0.0f };
	vel += gravityVector * dt;
}

void Physics::ConvertPositionToVelocity(DirectX::XMFLOAT3& vel, const DirectX::XMFLOAT3& pos, const float& dt)
{
	vel.x = pos.x / dt;
	vel.y = pos.y / dt;
	vel.z = pos.z / dt;
}

void Physics::ConvertVelocityToAcceleration(DirectX::XMFLOAT3& accel, const DirectX::XMFLOAT3& vel, const float& dt)
{
	ConvertPositionToVelocity(accel, vel, dt);
}

void Physics::ConvertPositionToAcceleration(DirectX::XMFLOAT3& accel, const DirectX::XMFLOAT3& pos, const float& dt)
{
	ConvertPositionToVelocity(accel, pos, dt);
	ConvertVelocityToAcceleration(accel, pos, dt);
}

const bool Physics::DetectCollision(const DirectX::XMVECTOR& pos)
{
	XMFLOAT3 pos_f3;
	XMStoreFloat3(&pos_f3, pos);

	XMFLOAT3 posCS = ChunkManager::WorldToChunkSpace(pos_f3);
	XMFLOAT3 chunkPosWS = ChunkManager::ChunkToWorldSpace(posCS);

	std::shared_ptr<Chunk> chunk = ChunkManager::GetChunkAtPos(posCS);
	// No chunk exists, so collision can't happen
	// This should never be the case when testing collision with player's position
	// but since vertical chunks aren't a thing yet we need this workaround
	if (!chunk) return false;
	
	uint32_t x, y, z;
	x = static_cast<float>(pos.m128_f32[0]) - chunkPosWS.x;
	y = static_cast<float>(pos.m128_f32[1]) - chunkPosWS.y;
	z = static_cast<float>(pos.m128_f32[2]) - chunkPosWS.z;
	BlockType blockType = chunk->GetBlock(x, y, z)->GetType();
	
	if (blockType != BlockType::Air) return true;
	else return false;
}

const bool DetectCollision(const AABB& aabb)
{
	//
	//	!NOTE! 
	//	Unimplemented
	//
	//
	return false;

	/*XMFLOAT3 posCS = ChunkManager::WorldToChunkSpace(aabb.center);
	XMFLOAT3 chunkPosWS = ChunkManager::ChunkToWorldSpace(posCS);

	std::shared_ptr<Chunk> chunk = ChunkManager::GetChunkAtPos(posCS);
	if (!chunk) return false;


	BlockType blockType = chunk->GetBlock(pos.m128_f32[0] - chunkPosWS.x, pos.m128_f32[1] - chunkPosWS.y, pos.m128_f32[2] - chunkPosWS.z)->GetType();*/
}
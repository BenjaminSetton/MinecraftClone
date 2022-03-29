#include "../Misc/pch.h"
#include "Physics.h"

#include "ChunkManager.h"
#include "../Utility/Utility.h"
#include "../Utility/Math.h"

// TEMP DEBUG
#include "../Utility/ImGuiDrawData.h"

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

	XMFLOAT3 posCS = VX_MATH::WorldToChunkSpace(pos_f3);
	XMFLOAT3 chunkPosWS = VX_MATH::ChunkToWorldSpace(posCS);

	Chunk* chunk = ChunkManager::GetChunkAtPos(posCS);
	VX_ASSERT(chunk != nullptr);
	
	uint32_t x, y, z;
	x = static_cast<uint32_t>(pos.m128_f32[0] - chunkPosWS.x);
	y = static_cast<uint32_t>(pos.m128_f32[1] - chunkPosWS.y);
	z = static_cast<uint32_t>(pos.m128_f32[2] - chunkPosWS.z);
	BlockType blockType = chunk->GetBlock(x, y, z)->GetType();
	
	if (blockType != BlockType::Air) return true;
	else return false;
}

// out_collisionPositions contains a list of all intersected blocks. This means that it will keep checking for collisions even after
// it finds a collision, so in that sense it's inefficient. However, it's sometimes useful to know the positions of the blocks we hit
const bool Physics::DetectCollision(const AABB& aabb, std::vector<XMFLOAT3>* out_collisionPositions)
{
	XMFLOAT3 aabbMin = { aabb.center.x - aabb.extent.x, aabb.center.y - aabb.extent.y, aabb.center.z - aabb.extent.z };
	XMFLOAT3 aabbMax = { aabb.center.x + aabb.extent.x, aabb.center.y + aabb.extent.y, aabb.center.z + aabb.extent.z };

	// for each axis, find the range
	// after finding the range for each axis, find the position of included blocks
	XMFLOAT3 range = 
	{ 
		static_cast<float>(abs(static_cast<int32_t>(aabbMax.x) - static_cast<int32_t>(aabbMin.x)) + 1),
		static_cast<float>(abs(static_cast<int32_t>(aabbMax.y) - static_cast<int32_t>(aabbMin.y)) + 1),
		static_cast<float>(abs(static_cast<int32_t>(aabbMax.z) - static_cast<int32_t>(aabbMin.z)) + 1)
	};

	// temp debug
	PlayerPhysics_Data::AABBMin = aabbMin;
	PlayerPhysics_Data::AABBCollisionRange = range;
	//

	bool collisionHappened = false;
	for (uint32_t x = 0; x < range.x; x++)
	{
		for (uint32_t y = 0; y < range.y; y++)
		{
			for (uint32_t z = 0; z < range.z; z++)
			{
				// intersected block pos in world space
				XMFLOAT3 intersectedBlockPos = { floor(aabbMin.x) + x, floor(aabbMin.y) + y, floor(aabbMin.z) + z };

				XMFLOAT3 posCS = VX_MATH::WorldToChunkSpace(intersectedBlockPos);
				XMFLOAT3 chunkPosWS = VX_MATH::ChunkToWorldSpace(posCS);

				Chunk* chunk = ChunkManager::GetChunkAtPos(posCS);
				VX_ASSERT(chunk != nullptr);

				uint32_t localX, localY, localZ;
				localX = static_cast<uint32_t>(intersectedBlockPos.x - chunkPosWS.x);
				localY = static_cast<uint32_t>(intersectedBlockPos.y - chunkPosWS.y);
				localZ = static_cast<uint32_t>(intersectedBlockPos.z - chunkPosWS.z);
				BlockType blockType = chunk->GetBlock(localX, localY, localZ)->GetType();

				if(blockType != BlockType::Air)
					DebugRenderer::DrawAABB({ intersectedBlockPos.x + 0.5f, intersectedBlockPos.y + 0.5f, intersectedBlockPos.z + 0.5f }, { 0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f, 1.0f, 1.0f });
				else
					DebugRenderer::DrawAABB({ intersectedBlockPos.x + 0.5f, intersectedBlockPos.y + 0.5f, intersectedBlockPos.z + 0.5f }, { 0.5f, 0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f, 1.0f });

				if (blockType != BlockType::Air)
				{
					if(out_collisionPositions) out_collisionPositions->push_back(intersectedBlockPos);
					collisionHappened = true;
				}
			}
		}
	}

	return collisionHappened;

}
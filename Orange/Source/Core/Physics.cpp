#include "../Misc/pch.h"
#include "Physics.h"

#include "ChunkManager.h"
#include "../Utility/Utility.h"
#include "../Utility/Math.h"

// TEMP DEBUG
#include "../Utility/ImGuiDrawData.h"

using namespace DirectX;

void Physics::ApplyVelocity(XMFLOAT3& pos, const XMFLOAT3& vel, const float& dt)
{
	XMVECTOR newPos = XMLoadFloat3(&pos);
	XMVECTOR newVel = XMLoadFloat3(&vel);
	newPos += XMVectorScale(newVel, dt);
	XMStoreFloat3(&pos, newPos);
}

void Physics::ApplyAcceleration(XMFLOAT3& vel, const XMFLOAT3& accel, const float& dt)
{
	XMVECTOR newVel = XMLoadFloat3(&vel);
	XMVECTOR newAccel = XMLoadFloat3(&accel);
	newVel += XMVectorScale(newAccel, dt);
	XMStoreFloat3(&vel, newVel);
}
void Physics::ApplyGravity(XMFLOAT3& vel, const float& dt)
{
	const XMVECTOR gravityVector = { 0.0f, -9.8f, 0.0f };
	XMVECTOR newVel = XMLoadFloat3(&vel);
	newVel += gravityVector * dt;
	XMStoreFloat3(&vel, newVel);
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

const bool Physics::DetectCollision(const DirectX::XMFLOAT3& pos)
{

	XMFLOAT3 posCS = Orange::Math::WorldToChunkSpace(pos);
	XMFLOAT3 chunkPosWS = Orange::Math::ChunkToWorldSpace(posCS);

	Chunk* chunk = ChunkManager::GetChunkAtPos(posCS);
	OG_ASSERT(chunk != nullptr);
	
	uint32_t x, y, z;
	x = static_cast<uint32_t>(pos.x - chunkPosWS.x);
	y = static_cast<uint32_t>(pos.y - chunkPosWS.y);
	z = static_cast<uint32_t>(pos.z - chunkPosWS.z);
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
		static_cast<float>(abs(floor(aabbMax.x) - floor(aabbMin.x)) + 1),
		static_cast<float>(abs(floor(aabbMax.y) - floor(aabbMin.y)) + 1),
		static_cast<float>(abs(floor(aabbMax.z) - floor(aabbMin.z)) + 1)
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

				XMFLOAT3 posCS = Orange::Math::WorldToChunkSpace(intersectedBlockPos);
				XMFLOAT3 chunkPosWS = Orange::Math::ChunkToWorldSpace(posCS);

				Chunk* chunk = ChunkManager::GetChunkAtPos(posCS);
				OG_ASSERT(chunk != nullptr);

				uint32_t localX, localY, localZ;
				localX = static_cast<uint32_t>(intersectedBlockPos.x - chunkPosWS.x);
				localY = static_cast<uint32_t>(intersectedBlockPos.y - chunkPosWS.y);
				localZ = static_cast<uint32_t>(intersectedBlockPos.z - chunkPosWS.z);
				BlockType blockType = chunk->GetBlock(localX, localY, localZ)->GetType();

				//if(blockType != BlockType::Air)
				//	DebugRenderer::DrawAABB({ intersectedBlockPos.x + 0.5f, intersectedBlockPos.y + 0.5f, intersectedBlockPos.z + 0.5f }, { 0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f, 1.0f, 1.0f });
				//else
				//	DebugRenderer::DrawAABB({ intersectedBlockPos.x + 0.5f, intersectedBlockPos.y + 0.5f, intersectedBlockPos.z + 0.5f }, { 0.5f, 0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f, 1.0f });

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
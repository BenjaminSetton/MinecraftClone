#ifndef _MATH_H
#define _MATH_H

#include <DirectXMath.h>
#include <functional>
#include <algorithm>

#include "../Core/ChunkManager.h"

// DEBUG INCLUDE
#include "Input.h"


namespace VX_MATH
{

#define pow2(x) (x) * (x)

	// Some utility macros for DirectX::XMFLOAT structs
	// THESE DO NOT HAVE ANY SAFETY CHECKS, USE AT YOUR OWN RISK
#define XMFLOAT3_BRACKET_OP_32(varPtr, index) *((float*)(&varPtr) + index)

#define XMFLOAT3_IS_EQUAL(vecOne, vecTwo) \
	((XMFLOAT3_BRACKET_OP_32(vecOne, 0) == XMFLOAT3_BRACKET_OP_32(vecTwo, 0)) && \
	(XMFLOAT3_BRACKET_OP_32(vecOne, 1) == XMFLOAT3_BRACKET_OP_32(vecTwo, 1)) && \
	(XMFLOAT3_BRACKET_OP_32(vecOne, 2) == XMFLOAT3_BRACKET_OP_32(vecTwo, 2)))


	// This function returns a unique identifier for all chunks within a range of
	// 65,536 in any dimension of chunkPos
	inline uint64_t GetHashKeyFromChunkPosition(const DirectX::XMFLOAT3& chunkPos)
	{
		uint64_t result = 0;
		// FOR DEBUG PURPOSE, CHECK IF WE EXCEEDED 16 BITS
#ifdef _DEBUG
		if (
			static_cast<int32_t>(chunkPos.x) != static_cast<int16_t>(chunkPos.x) ||
			static_cast<int32_t>(chunkPos.y) != static_cast<int16_t>(chunkPos.y) ||
			static_cast<int32_t>(chunkPos.z) != static_cast<int16_t>(chunkPos.z)
			)
			VX_ASSERT_MSG(false, "Underflow or overflow detected in %s", __FUNCTION__);
#endif
		int64_t x, y, z;
		x = y = z = 0;
		x = static_cast<int64_t>(chunkPos.x);
		y = static_cast<int64_t>(chunkPos.y);
		z = static_cast<int64_t>(chunkPos.z);
		result = ((x << 32) & 0x0000FFFF00000000) | ((y << 16) & 0x00000000FFFF0000) | (z & 0x000000000000FFFF);

		return result;
	}

	inline DirectX::XMFLOAT3 WorldToChunkSpace(const DirectX::XMFLOAT3& pos)
	{
		DirectX::XMFLOAT3 convertedPos = { (float)((int32_t)pos.x / CHUNK_SIZE), (float)((int32_t)pos.y / CHUNK_SIZE), (float)((int32_t)pos.z / CHUNK_SIZE) };

		// Adjust for negative coordinates
		convertedPos.x = pos.x < 0 ? --convertedPos.x : convertedPos.x;
		convertedPos.y = pos.y < 0 ? --convertedPos.y : convertedPos.y;
		convertedPos.z = pos.z < 0 ? --convertedPos.z : convertedPos.z;
		return convertedPos;
	}

	inline DirectX::XMFLOAT3 ChunkToWorldSpace(const DirectX::XMFLOAT3& pos)
	{
		// NOTE! pos should contain whole numbers only
		return { pos.x * CHUNK_SIZE, pos.y * CHUNK_SIZE, pos.z * CHUNK_SIZE };
	}

	inline float Lerp(const float& a, const float& b, const float& ratio)
	{
		return (b - a) * ratio + a;
	}

	inline double Lerp(const double& a, const double& b, const double& ratio)
	{
		return ((1.0 - ratio) * a) + (ratio * b);
		//return (b - a) * ratio + a;
	}

	inline double MapToCubicSCurve(const double x)
	{
		// Parameter "x" must be between [0, 1]
		VX_ASSERT(x >= 0 && x <= 1);
		return (x * x * (3.0 - 2.0 * x));
	}

	inline double MapToQuinticSCurve(const double x)
	{
		// Parameter "x" must be between [0, 1]
		VX_ASSERT(x >= 0 && x <= 1);
		return x * x * x * (x * (x * 6 - 15) + 10);
	}

	inline bool Raycast(const DirectX::XMFLOAT3& rayPos, const DirectX::XMFLOAT3& rayDir, const float maxDist, std::function<bool(const DirectX::XMFLOAT3&)> checkHit, DirectX::XMFLOAT3* outHit)
	{
		using namespace DirectX;

		// DEBUG LOGGING
		Log log;
		log.SetOutputFile("C:/Users/benja/OneDrive/Desktop/raycast_debug.txt");
		log.PrintNLToFile("Testing raycast with pos [%2.2f, %2.2f, %2.2f] and dir [%2.2f, %2.2f, %2.2f]", rayPos.x, rayPos.y, rayPos.z, rayDir.x, rayDir.y, rayDir.z);
		log.PrintNLToFile("[");
		//
		
		XMFLOAT3 rayUnitStepSize =
		{
			rayDir.x == 0.0f ? maxDist : sqrt(1 + ((rayDir.y / rayDir.x) * (rayDir.y / rayDir.x)) + ((rayDir.z / rayDir.x) * (rayDir.z / rayDir.x))),
			rayDir.y == 0.0f ? maxDist : sqrt(((rayDir.x / rayDir.y) * (rayDir.x / rayDir.y)) + 1 + ((rayDir.z / rayDir.y) * (rayDir.z / rayDir.y))),
			rayDir.z == 0.0f ? maxDist : sqrt(((rayDir.x / rayDir.z) * (rayDir.x / rayDir.z)) + ((rayDir.y / rayDir.z) * (rayDir.y / rayDir.z)) + 1)
		};

		XMFLOAT3 mapCheck = 
		{ 
			static_cast<float>(static_cast<int>(rayPos.x)), 
			static_cast<float>(static_cast<int>(rayPos.y)), 
			static_cast<float>(static_cast<int>(rayPos.z))
		};
		// Step is an integer vector
		XMFLOAT3 rayLength1D, step = {0, 0, 0};

		if (rayDir.x < 0)
		{
			step.x = -1;
			rayLength1D.x = (rayPos.x - static_cast<float>(mapCheck.x)) * rayUnitStepSize.x;
		}
		else
		{
			step.x = 1;
			rayLength1D.x = (static_cast<float>(mapCheck.x + 1) - rayPos.x) * rayUnitStepSize.x;
		}

		if (rayDir.y < 0)
		{
			step.y = -1;
			rayLength1D.y = (rayPos.y - static_cast<float>(mapCheck.y)) * rayUnitStepSize.y;
		}
		else
		{
			step.y = 1;
			rayLength1D.y = (static_cast<float>(mapCheck.y + 1) - rayPos.y) * rayUnitStepSize.y;
		}

		if (rayDir.z < 0)
		{
			step.z = -1;
			rayLength1D.z = (rayPos.z - static_cast<float>(mapCheck.z)) * rayUnitStepSize.z;
		}
		else
		{
			// NOTE FOR FUTURE SELF
			// I had to put this check in here because this algorithm wasn't accounting for negative numbers.
			// This meant that when setting the rayLength1D for any particular axis, adding 1 to the mapCheck axis
			// would cause it to be longer than it had to be, and it would sometimes cut corners or do some weird
			// shit like that. The fix is to consider the negative axis separately, but I must do so for the other X and Y
			// axes for this to work 100%!!
			step.z = 1;
			if (rayPos.z >= 0)
			{
				rayLength1D.z = (static_cast<float>(mapCheck.z + 1) - rayPos.z) * rayUnitStepSize.z;
			}
			else
			{
				rayLength1D.z = (static_cast<float>(mapCheck.z - 1) - rayPos.z) * rayUnitStepSize.z;
			}
		}
		
		bool targetFound = false;
		float accDistance = 0.0f;

		log.PrintNLToFile("\tStep [ %2.2f, %2.2f, %2.2f ]", step.x, step.y, step.z);

		uint32_t counter_debug = 0;
		while (accDistance < maxDist)
		{
			// sort the ray length 1D axes
			std::vector<std::tuple<int, float>> sortedAxes = { std::make_tuple(0, rayLength1D.x), std::make_tuple(1, rayLength1D.y), std::make_tuple(2, rayLength1D.z) };
			std::sort(sortedAxes.begin(), sortedAxes.end(), [](const std::tuple<int, float> valOne, const std::tuple<int, float> valTwo) { return std::get<1>(valOne) < std::get<1>(valTwo); });

			// we want to walk along the shortest ray length axis at that particular time
			float shortestAxisDistance = std::get<1>(sortedAxes[0]);
			int shortestAxisIndex = std::get<0>(sortedAxes[0]);

			XMFLOAT3_BRACKET_OP_32(mapCheck, shortestAxisIndex) += XMFLOAT3_BRACKET_OP_32(step, shortestAxisIndex);
			accDistance = shortestAxisDistance;
			XMFLOAT3_BRACKET_OP_32(rayLength1D, shortestAxisIndex) += XMFLOAT3_BRACKET_OP_32(rayUnitStepSize, shortestAxisIndex);

			XMFLOAT3 intermediateRayHit = 
			{
				rayPos.x + (rayDir.x * accDistance),
				rayPos.y + (rayDir.y * accDistance),
				rayPos.z + (rayDir.z * accDistance),
			};

			// Calculate voxel pos
			// NOTE: If we truncate a negative number with decimal larger than 0, the result will be a larger number, 
			// or a number further right on the number line. On the other hand, truncating a positive number results in
			// a smaller number, further left on the number line. Since we always want the truncated number to be smaller,
			// we will check if it's negative and subtract 1.0f if it is. We also consider exact
			XMFLOAT3 voxelPos = 
			{	static_cast<float>(static_cast<int>((static_cast<int>(intermediateRayHit.x) != intermediateRayHit.x) && intermediateRayHit.x < 0.0f ? intermediateRayHit.x - 1.0f : intermediateRayHit.x)),
				static_cast<float>(static_cast<int>((static_cast<int>(intermediateRayHit.y) != intermediateRayHit.y) && intermediateRayHit.y < 0.0f ? intermediateRayHit.y - 1.0f : intermediateRayHit.y)),
				static_cast<float>(static_cast<int>((static_cast<int>(intermediateRayHit.z) != intermediateRayHit.z) && intermediateRayHit.z < 0.0f ? intermediateRayHit.z - 1.0f : intermediateRayHit.z))
			};

			// New target position when hitting block face
			for (uint32_t i = 0; i < 3; i++)
			{
				if (XMFLOAT3_BRACKET_OP_32(intermediateRayHit, i) == XMFLOAT3_BRACKET_OP_32(voxelPos, i))
				{
					if (XMFLOAT3_BRACKET_OP_32(rayDir, i) < 0)
					{
						XMFLOAT3_BRACKET_OP_32(voxelPos, i) -= 1.0f;
					}

					break;
				}
			}
			if (checkHit(voxelPos))
			{
				*outHit = intermediateRayHit;
				DebugRenderer::DrawSphere(1, intermediateRayHit, 0.01f, { 0, 1.0f, 0, 1.0f });
				DebugRenderer::DrawLine(intermediateRayHit, voxelPos, { 0.0f, 0.0f, 0.0f, 1.0f });
				DebugRenderer::DrawSphere(1, voxelPos, 0.01f, { 0, 0, 1.0f, 1.0f });
				VX_LOG("Ray hit at voxel pos [ %2.2f, %2.2f, %2.2f ]", voxelPos.x, voxelPos.y, voxelPos.z);
				log.PrintToFile("\n]");
				return true;
			}
			DebugRenderer::DrawSphere(1, intermediateRayHit, 0.01f, { 0, 0, 0, 1.0f });
			DebugRenderer::DrawLine(intermediateRayHit, voxelPos, { 0.0f, 0.0f, 0.0f, 1.0f });
			DebugRenderer::DrawSphere(1, voxelPos, 0.01f, { 0, 0, 1.0f, 1.0f });

			log.PrintNLToFile("\t[%i]\n\t{", counter_debug);
			log.PrintNLToFile("\t\tIntermediate Ray Hit [ %2.2f, %2.2f, %2.2f ]", intermediateRayHit.x, intermediateRayHit.y, intermediateRayHit.z);
			log.PrintNLToFile("\t\tVoxel Pos [ %2.2f, %2.2f, %2.2f ]", voxelPos.x, voxelPos.y, voxelPos.z);
			log.PrintNLToFile("\t\tRayLength1D [ %2.5f, %2.5f, %2.5f ]", rayLength1D.x, rayLength1D.y, rayLength1D.z);
			log.PrintNLToFile("\t}");

			counter_debug++;
		}
		log.PrintToFile("\n]");

		return false;

	}
}

#endif
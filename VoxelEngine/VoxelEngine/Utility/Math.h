#ifndef _MATH_H
#define _MATH_H

#include <DirectXMath.h>
#include <functional>
#include <algorithm>

#include "../Core/ChunkManager.h"


namespace VX_MATH
{

	// Some utility macros for DirectX::XMFLOAT structs
	// THESE DO NOT HAVE ANY SAFETY CHECKS, USE AT YOUR OWN RISK
#define XMFLOAT3_BRACKET_OP_32(varPtr, index) *((float*)(&varPtr) + index)

#define XMFLOAT3_IS_EQUAL(vecOne, vecTwo) \
	XMFLOAT3_BRACKET_OP_32(vecOne, 0) == XMFLOAT3_BRACKET_OP_32(vecTwo, 0) && \
	XMFLOAT3_BRACKET_OP_32(vecOne, 1) == XMFLOAT3_BRACKET_OP_32(vecTwo, 1) && \
	XMFLOAT3_BRACKET_OP_32(vecOne, 2) == XMFLOAT3_BRACKET_OP_32(vecTwo, 2)


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

#pragma region _COMMENT
		//XMFLOAT3 p = rayPos;
		//XMFLOAT3 step = {0, 0, 0};
		//XMFLOAT3 counter = { 0, 0, 0 };
		//XMFLOAT3 delta = {};

		// Start
		//int i, ix;
		//for (ix = 0, i = 0; i < 3; i++)
		//{

		//	if (XMFLOAT3_BRACKET_OP_32(rayDir, i) > 0)
		//	{
		//		XMFLOAT3_BRACKET_OP_32(step, i) = 1;
		//	}

		//	if (XMFLOAT3_BRACKET_OP_32(rayDir, i) < 0) 
		//	{ 
		//		XMFLOAT3_BRACKET_OP_32(step, i) = -1; 
		//		XMFLOAT3_BRACKET_OP_32(rayDir, i) = -XMFLOAT3_BRACKET_OP_32(rayDir, i); 
		//	}

		//	if (XMFLOAT3_BRACKET_OP_32(rayDir, ix) < XMFLOAT3_BRACKET_OP_32(rayDir, i))
		//	{
		//		ix = i;
		//	}
		//}
		//for (i = 0; i < 3; i++)
		//{
		//	XMFLOAT3_BRACKET_OP_32(counter, i) = XMFLOAT3_BRACKET_OP_32(rayDir, ix);
		//}

		//// Update
		//while (!XMFLOAT3_IS_EQUAL(XMFLOAT3_BRACKET_OP_32(p, ix), ))
		//{
		//	for (int i = 0; i < 3; i++) 
		//	{ 
		//		XMFLOAT3_BRACKET_OP_32(counter, i) -= delta[i];
		//		if (XMFLOAT3_BRACKET_OP_32(counter, i) <= 0)
		//		{ 
		//			XMFLOAT3_BRACKET_OP_32(counter, i) += delta[ix]; XMFLOAT3_BRACKET_OP_32(p, i) += XMFLOAT3_BRACKET_OP_32(step, i);
		//		} 
		//	}
		//	if (checkHit(p))
		//	{
		//		*outHit = p;
		//		return true;
		//	}
		//	return (p[ix] != p1[ix] + step[ix]);
		//}
#pragma endregion
		
		XMFLOAT3 rayUnitStepSize =
		{
			sqrt(1 + ((rayDir.y / rayDir.x) * (rayDir.y / rayDir.x)) + ((rayDir.z / rayDir.x) * (rayDir.z / rayDir.x))),
			sqrt(((rayDir.x / rayDir.y) * (rayDir.x / rayDir.y)) + 1 + ((rayDir.z / rayDir.y) * (rayDir.z / rayDir.y))),
			sqrt(((rayDir.x / rayDir.z) * (rayDir.x / rayDir.z)) + ((rayDir.y / rayDir.z) * (rayDir.y / rayDir.z)) + 1)
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
			step.z = 1;
			rayLength1D.z = (static_cast<float>(mapCheck.z + 1) - rayPos.z) * rayUnitStepSize.z;
		}
		
		bool targetFound = false;
		float accDistance = 0.0f;
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

			if (checkHit(mapCheck))
			{
				*outHit = 
				{ 
					rayPos.x + (rayDir.x * accDistance),
					rayPos.y + (rayDir.y * accDistance),
					rayPos.z + (rayDir.z * accDistance),
				};
				return true;
			}

			// intersection = rayStart + rayDir * accDistance;
		}
		

		return false;

	}
}

#endif
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
	constexpr float E = 2.71828182845904523536f;
	constexpr float PI = 3.14159265358979323846f;
	constexpr float PI_DIV_2 = 1.57079632679489661923f;
	constexpr float PI_DIV_4 = 0.785398163397448309616f;

#define POW2(x) (x) * (x)

	// Some utility macros for DirectX::XMFLOAT structs
	// THESE DO NOT HAVE ANY SAFETY CHECKS, USE AT YOUR OWN RISK
#define XMFLOAT3_BRACKET_OP_32(varPtr, index) *((float*)(&varPtr) + index)

#define XMFLOAT3_IS_EQUAL(vecOne, vecTwo) \
	((XMFLOAT3_BRACKET_OP_32(vecOne, 0) == XMFLOAT3_BRACKET_OP_32(vecTwo, 0)) && \
	(XMFLOAT3_BRACKET_OP_32(vecOne, 1) == XMFLOAT3_BRACKET_OP_32(vecTwo, 1)) && \
	(XMFLOAT3_BRACKET_OP_32(vecOne, 2) == XMFLOAT3_BRACKET_OP_32(vecTwo, 2)))

	template<typename T>
	inline void Clamp(T& value, const T min, const T max)
	{
		if (value < min) value = min;
		else if (value > max) value = max;
	}

	template<typename T>
	inline void Wrap(T& value, const T min, const T max)
	{
		if (value < min) value = max - (min - value);
		else if (value > max) value = min + (value - max);
	}

	template<typename T>
	inline int32_t Sign(const T& value)
	{
		if (value < 0)			return -1;
		else if (value > 0)		return 1;
		else					return 0;
	}

	inline float Decimal(const float value) { return abs(value - static_cast<int>(value)); }

	inline float DegreesToRadians(const float& degrees) { return (degrees * (PI / 180.0f)); }

	inline float RadiansToDegrees(const float& radians) { return (radians * (180.0f / PI)); }

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
		DirectX::XMFLOAT3 convertedPos = { pos.x / CHUNK_SIZE, pos.y / CHUNK_SIZE, pos.z / CHUNK_SIZE };

		// For negative positions, if division does NOT give a decimal we subtract one
		// otherwise, we leave as is
		convertedPos.x = (convertedPos.x < 0.0f && Decimal(convertedPos.x) != 0.0f) ? --convertedPos.x : convertedPos.x;
		convertedPos.y = (convertedPos.y < 0.0f && Decimal(convertedPos.y) != 0.0f) ? --convertedPos.y : convertedPos.y;
		convertedPos.z = (convertedPos.z < 0.0f && Decimal(convertedPos.z) != 0.0f) ? --convertedPos.z : convertedPos.z;

		return { static_cast<float>(static_cast<int32_t>(convertedPos.x)), static_cast<float>(static_cast<int32_t>(convertedPos.y)), static_cast<float>(static_cast<int32_t>(convertedPos.z)) };
	}

	inline DirectX::XMFLOAT3 ChunkToWorldSpace(const DirectX::XMFLOAT3& pos)
	{
		// NOTE! pos should contain whole numbers only
		return { pos.x * CHUNK_SIZE, pos.y * CHUNK_SIZE, pos.z * CHUNK_SIZE };
	}

	template<typename T>
	inline T Lerp(const T& a, const T& b, const T& ratio)
	{
		return (b - a) * ratio + a;
	}

	template<typename T>
	inline T MapToCubicSCurve(const T x)
	{
		// Parameter "x" must be between [0, 1]
		VX_ASSERT(x >= 0.0 && x <= 1.0);
		return (x * x * (3.0 - 2.0 * x));
	}

	template<typename T>
	inline T MapToQuinticSCurve(const T x)
	{
		// Parameter "x" must be between [0, 1]
		VX_ASSERT(x >= 0.0 && x <= 1.0);
		return x * x * x * (x * (x * 6.0 - 15.0) + 10.0);
	}

	inline bool Raycast(const DirectX::XMFLOAT3& rayPos, const DirectX::XMFLOAT3& rayDir, const float maxDist, std::function<bool(const DirectX::XMFLOAT3&)> checkHit, DirectX::XMFLOAT3* outHit)
	{
		using namespace DirectX;
		
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

		if (rayDir.x < 0.0f)
		{
			step.x = -1.0f;
			if (rayPos.x >= 0.0f)	rayLength1D.x = (rayPos.x - static_cast<float>(mapCheck.x)) * rayUnitStepSize.x;
			else					rayLength1D.x = (rayPos.x - static_cast<float>(mapCheck.x - 1.0f)) * rayUnitStepSize.x;
		}
		else
		{
			step.x = 1.0f;
			if (rayPos.x >= 0.0f)	rayLength1D.x = (static_cast<float>(mapCheck.x + 1.0f) - rayPos.x) * rayUnitStepSize.x;
			else					rayLength1D.x = (static_cast<float>(mapCheck.x) - rayPos.x) * rayUnitStepSize.x;
		}

		if (rayDir.y < 0.0f)
		{
			step.y = -1.0f;
			if (rayPos.y >= 0.0f)	rayLength1D.y = (rayPos.y - static_cast<float>(mapCheck.y)) * rayUnitStepSize.y;
			else					rayLength1D.y = (rayPos.y - static_cast<float>(mapCheck.y - 1.0f)) * rayUnitStepSize.y;
		}
		else
		{
			step.y = 1.0f;
			if (rayPos.y >= 0.0f)	rayLength1D.y = (static_cast<float>(mapCheck.y + 1.0f) - rayPos.y) * rayUnitStepSize.y;
			else					rayLength1D.y = (static_cast<float>(mapCheck.y) - rayPos.y) * rayUnitStepSize.y;
		}

		if (rayDir.z < 0.0f)
		{
			step.z = -1.0f;
			if (rayPos.z >= 0.0f)	rayLength1D.z = (rayPos.z - static_cast<float>(mapCheck.z)) * rayUnitStepSize.z;
			else					rayLength1D.z = (rayPos.z - static_cast<float>(mapCheck.z - 1.0f)) * rayUnitStepSize.z;
		}
		else
		{
			step.z = 1.0f;
			if (rayPos.z >= 0.0f)	rayLength1D.z = (static_cast<float>(mapCheck.z + 1.0f) - rayPos.z) * rayUnitStepSize.z;
			else					rayLength1D.z = (static_cast<float>(mapCheck.z) - rayPos.z) * rayUnitStepSize.z;
		}
		
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
				//DebugRenderer::DrawSphere(1, intermediateRayHit, 0.01f, { 0, 1.0f, 0, 1.0f });
				//DebugRenderer::DrawLine(intermediateRayHit, voxelPos, { 0.0f, 0.0f, 0.0f, 1.0f });
				//DebugRenderer::DrawSphere(1, voxelPos, 0.01f, { 0, 0, 1.0f, 1.0f });
				return true;
			}
			//DebugRenderer::DrawSphere(1, intermediateRayHit, 0.01f, { 0, 0, 0, 1.0f });
			//DebugRenderer::DrawLine(intermediateRayHit, voxelPos, { 0.0f, 0.0f, 0.0f, 1.0f });
			//DebugRenderer::DrawSphere(1, voxelPos, 0.01f, { 0, 0, 1.0f, 1.0f });

		}

		return false;

	}
	
}

#endif
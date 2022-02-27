#ifndef _MATH_H
#define _MATH_H

#include <DirectXMath.h>

#include "../Core/ChunkManager.h"

namespace VX_MATH
{
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
}

#endif
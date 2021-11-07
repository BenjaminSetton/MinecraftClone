#ifndef _MATH_H
#define _MATH_H

#include <DirectXMath.h>

//#include "../Core/ChunkManager.h"

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
}

#endif
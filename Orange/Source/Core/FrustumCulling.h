#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "Chunk.h"

// Includes the physics types (plane, aabb, and sphere)
#include "Physics.h"

namespace Orange
{
	#define PASS_STRADDLING_CHUNKS 1

	struct Frustum_Planes
	{
		enum
		{
			LEFT,
			RIGHT,
			TOP,
			BOTTOM,
			FRONT,
			BACK,
			COUNT
		};
	};

	struct Frustum_Vertices 
	{
		enum
		{
			TLN = 0,
			TRN,
			BLN,
			BRN,

			TLF,
			TRF,
			BLF,
			BRF,

			COUNT
		};
	};

	struct Frustum
	{
		// Stores 6 planes that define a frustum
		std::array<Plane, Frustum_Planes::COUNT> planes;

		// Stores 8 points that define a frustum
		std::array<DirectX::XMVECTOR, Frustum_Vertices::COUNT> vertices;
	};

	class FrustumCulling
	{

	public:

		FrustumCulling() = default;
		FrustumCulling(const FrustumCulling& other) = delete;
		~FrustumCulling() = default;

		static void SetFrustum(const Frustum& frustum);
		static const Frustum GetFrustum();

		// Returns TRUE is visible
		// Returns FALSE is not visible
		static bool CalculateChunkPosAgainstFrustum(const DirectX::XMFLOAT3 chunkPosWS);

		static void CalculateFrustum(float FOV, float aspectRatio, float nearPlane, float farPlane, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMFLOAT3& camPos);

		static void Debug_DrawFrustum();

		static void Debug_DrawAABB(const AABB& aabb);

		static const AABB ConvertChunkPosToAABB(const DirectX::XMFLOAT3 chunkPosWS);
	private:


		static int TestAABBAgainstPlane(const AABB& aabb, const Plane& plane);

		static int TestSphereAgainstPlane(const Sphere& sphere, const Plane& plane);

		static Plane CalculatePlaneFromPoints(const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b, const DirectX::XMVECTOR& c);

	private:

		static Frustum m_frustum;
	};
}



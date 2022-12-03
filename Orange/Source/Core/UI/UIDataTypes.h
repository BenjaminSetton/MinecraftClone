#ifndef _UIDATATYPES_H
#define _UIDATATYPES_H

#include "../../Utility/MathTypes.h"

// This file contains many enums / structs that will define how the engine's UI is stored and represented
namespace Orange
{

	static const Vec2 QUAD_COORDINATES[6] =
	{
		Vec2(0, 0), // TL
		Vec2(1, 0), // TR
		Vec2(1, 1), // BR
		Vec2(0, 0), // TL
		Vec2(1, 1), // BR
		Vec2(0, 1), // BL
	};

	// TODO - Switch over to DrawIndexed and use these indices
	static const uint32_t QUAD_INDICES[6] =
	{
		0,	// TL
		1,	// TR
		2,	// BR
		0,	// TL
		2,	// BR
		3	// BL
	};
	
	enum class UIElementType : uint16_t
	{
		INVALID = 0,
		TEXT,
		IMAGE,
		CHECKBOX,
		SLIDER,
		GRAPH,
		CONTAINER
		// TODO - Add more
	};

	enum class RECT_CORNER : uint8_t
	{
		TOP_LEFT = 0,
		BOTTOM_LEFT,
		TOP_RIGHT,
		BOTTOM_RIGHT
	};

	enum class HOR_ALIGNMENT : uint8_t
	{
		LEFT,
		CENTER,
		RIGHT,
	};

	enum class VER_ALIGNMENT : uint8_t
	{
		TOP,
		CENTER,
		BOTTOM,
	};

	// A 2D AABB used for UI (useful for describing a quad's position and size)
	// The internal implementation uses a corner and extents. The (0, 0) space
	// is on the BOTTOM_LEFT of the screen. This means that the bottom left is the min
	// of the rect and top right is the max
	class UIRect
	{
	public:

		UIRect()
		{
			// Initialize these values to some generic / magic numbers
			center = Vec2(215.0f, 215.0f);
			extent = Vec2(200.0f, 200.0f);
		}

		UIRect(const Vec2& ctr, const Vec2& ext)
		{
			center = ctr;
			extent = ext;
		}

		// TODO - Add helper methods for collision detection and whatever else might be useful
		Vec2 GetSize() const
		{ 
			return Vec2(2.0f * extent.x, 2.0f * extent.y); 
		}

		Vec2 GetMin() const
		{
			// TODO - Add a const Vec2 operator-() overload that does not modify any of the Vec2 objects
			Vec2 ctr = center;
			Vec2 ext = extent;
			return ctr - ext;
		}

		Vec2 GetMax() const
		{
			// TODO - Add a const Vec2 operator+() overload that does not modify any of the Vec2 objects
			Vec2 ctr = center;
			Vec2 ext = extent;
			return ctr + ext;
		}

		Vec2 GetCorner(const RECT_CORNER corner) const
		{
			// TODO - Add a const Vec2 operator+() overload that does not modify any of the Vec2 objects
			Vec2 ctr = center;
			Vec2 ext = extent;
			switch (corner)
			{
			case RECT_CORNER::BOTTOM_LEFT:
			{
				return GetMin();
			}
			case RECT_CORNER::BOTTOM_RIGHT:
			{
				return Vec2(ctr.x + ext.x, ctr.y - ext.y);
			}
			case RECT_CORNER::TOP_LEFT:
			{
				return Vec2(ctr.x - ext.x, ctr.y + ext.y);
			}
			case RECT_CORNER::TOP_RIGHT:
			{
				return GetMax();
			}
			}
			
			// We should never get here
			OG_ASSERT_MSG(false, "Please add new case to switch statement above");
			return Vec2(0.0f);
		}

		bool IsPointInRect(const Vec2& pt)
		{
			Vec2 min = GetMin();
			Vec2 max = GetMax();
			if( ( pt.x >= min.x && pt.x <= max.x ) && ( pt.y >= min.y && pt.y <= max.y ) )
			{
				return true;
			}
			return false;
		}

		Vec2 center;
		Vec2 extent;

	private:

	};

}

#endif
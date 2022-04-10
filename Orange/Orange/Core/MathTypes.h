#ifndef _MATH_TYPES_H
#define _MATH_TYPES_H

namespace Orange
{
	class Vec2
	{
	public:

		Vec2() = default;
		Vec2(const float _x, const float _y) { x = _x; y = _y; }
		Vec2(const int32_t _x, const int32_t _y) { x = static_cast<float>(_x), y = static_cast<float>(_y); }
		Vec2(const Vec2& other) { x = other.x; y = other.y; }
		~Vec2() = default;

		Vec2 operator+(const Vec2& other)
		{
			return Vec2(x + other.x, y + other.y);
		}

		Vec2 operator-(const Vec2& other)
		{
			return Vec2(other.x - x, other.y - y);
		}

		void operator+=(const Vec2& other)
		{
			x += other.x;
			y += other.y;
		}

		void operator-=(const Vec2& other)
		{
			x -= other.x;
			y -= other.y;
		}

		float x, y;
	};

	class Vec3
	{
	public:

		Vec3() = default;
		Vec3(const float _x, const float _y, const float _z) { x = _x; y = _y; z = _z; }
		Vec3(const int32_t _x, const int32_t _y, const int32_t _z) { x = static_cast<float>(_x), y = static_cast<float>(_y); z = static_cast<float>(_z); }
		Vec3(const Vec3& other) { x = other.x; y = other.y; z = other.z;  }
		~Vec3() = default;

		Vec3 operator+(const Vec3 & other)
		{
			return Vec3(x + other.x, y + other.y, z + other.z);
		}

		Vec3 operator-(const Vec3 & other)
		{
			return Vec3(other.x - x, other.y - y, other.z - z);
		}

		void operator+=(const Vec3 & other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
		}

		void operator-=(const Vec3 & other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
		}

		float x, y, z;
	};
}


#endif
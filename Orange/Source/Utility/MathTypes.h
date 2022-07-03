#ifndef _MATH_TYPES_H
#define _MATH_TYPES_H

namespace Orange
{
	class Vec2
	{
	public:

		Vec2() = default;
		Vec2(const float _x) { Vec2(_x, _x); }
		Vec2(const float _x, const float _y) { x = _x; y = _y; }
		Vec2(const int32_t _x, const int32_t _y) { x = static_cast<float>(_x), y = static_cast<float>(_y); }
		Vec2(const uint32_t _x, const uint32_t _y) { x = static_cast<float>(_x), y = static_cast<float>(_y); }
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
		Vec3(const float _x) { Vec3(_x, _x, _x); }
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

	class Vec4
	{
	public:

		Vec4() = default;
		Vec4(const float _x) { Vec4(_x, _x, _x, _x); }
		Vec4(const float _x, const float _y, const float _z, const float _w) { x = _x; y = _y; z = _z; w = _w; }
		Vec4(const int32_t _x, const int32_t _y, const int32_t _z, const int32_t _w) {
			x = static_cast<float>(_x), y = static_cast<float>(_y); z = static_cast<float>(_z); w = static_cast<float>(_w); }
		Vec4(const Vec4& other) { x = other.x; y = other.y; z = other.z; w = other.w; }
		~Vec4() = default;

		Vec4 operator+(const Vec4& other)
		{
			return Vec4(x + other.x, y + other.y, z + other.z, w + other.w);
		}

		Vec4 operator-(const Vec4& other)
		{
			return Vec4(other.x - x, other.y - y, other.z - z, other.w - w);
		}

		void operator+=(const Vec4& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;
		}

		void operator-=(const Vec4& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			w -= other.w;
		}

		float x, y, z, w;
	};
}


#endif
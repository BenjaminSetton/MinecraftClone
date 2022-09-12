#ifndef _MATHCONSTANTS_H
#define _MATHCONSTANTS_H

namespace Orange
{
	namespace Math
	{
		template<typename T>
		constexpr T E = static_cast<T>(2.71828182845904);

		template<typename T>
		constexpr T PI = static_cast<T>(3.14159265358979);

		template<typename T>
		constexpr T PI_DIV_2 = static_cast<T>(1.57079632679489);

		template<typename T>
		constexpr T PI_DIV_4 = static_cast<T>(0.78539816339744);
	}

}

#endif
#ifndef _NOISE_H
#define _NOISE_H

#include <vector>

class Noise3D
{
public:

	Noise3D() = delete;
	Noise3D(const Noise3D& other) = delete;
	~Noise3D() = delete;

	static void Init(uint32_t seed);

	static double GetValue(double x, double y, double z);

private:

	static double Grad(const int32_t hash, const double x, const double y, const double z);

	static std::vector<int32_t> m_permutationVec;

	static double m_scale;

};

#endif

#include "../Misc/pch.h"
#include "Noise.h"
#include "Math.h"
#include "Utility.h"

#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>

std::vector<int32_t> Noise3D::m_permutationVec = std::vector<int32_t>();

void Noise3D::Init(uint32_t seed)
{
	m_permutationVec.resize(256);

	// Fill p with values from 0 to 255
	std::iota(m_permutationVec.begin(), m_permutationVec.end(), 0);

	// Initialize a random engine with seed
	std::default_random_engine engine(seed);

	// Shuffle using the above random engine
	std::shuffle(m_permutationVec.begin(), m_permutationVec.end(), engine);

	// Duplicate the permutation vector
	m_permutationVec.resize(512);
	memcpy(&m_permutationVec[256], &m_permutationVec[0], sizeof(int32_t) * 256);
}

double Noise3D::GetValue(double x, double y, double z)
{
	x *= 0.05;
	y *= 0.05;

	using namespace VX_MATH;

	// Find the unit cube that contains the point
	int xc = (int)floor(x) & 255;
	int yc = (int)floor(y) & 255;
	int zc = (int)floor(z) & 255;

	// Find relative x, y,z of point in cube
	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	// Compute fade curves for each of x, y, z
	double u = MapToQuinticSCurve(x);
	double v = MapToQuinticSCurve(y);
	double w = MapToQuinticSCurve(z);

	// Hash coordinates of the 8 cube corners
	int A  = m_permutationVec[xc] + yc;
	int AA = m_permutationVec[A] + zc;
	int AB = m_permutationVec[A + 1] + zc;
	int B  = m_permutationVec[xc + 1] + yc;
	int BA = m_permutationVec[B] + zc;
	int BB = m_permutationVec[B + 1] + zc;

	// Add blended results from 8 corners of cube
	double result = Lerp(w, Lerp(v, Lerp(u, Grad(m_permutationVec[AA], x, y, z), Grad(m_permutationVec[BA], x - 1, y, z)), 
		Lerp(u, Grad(m_permutationVec[AB], x, y - 1, z), Grad(m_permutationVec[BB], x - 1, y - 1, z))), 
		Lerp(v, Lerp(u, Grad(m_permutationVec[AA + 1], x, y, z - 1), 
			Grad(m_permutationVec[BA + 1], x - 1, y, z - 1)), Lerp(u, Grad(m_permutationVec[AB + 1], x, y - 1, z - 1), Grad(m_permutationVec[BB + 1], x - 1, y - 1, z - 1))));
	return (result + 1.0) / 2.0;
}

double Noise3D::Grad(const int32_t hash, const double x, const double y, const double z)
{
	int h = hash & 15;
	// Convert lower 4 bits of hash into 12 gradient directions
	double u = h < 8 ? x : y,
		v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

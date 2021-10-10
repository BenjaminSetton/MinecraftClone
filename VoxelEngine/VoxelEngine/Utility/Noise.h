#pragma once

#include "../../libnoise/inc/noise/noise.h"

#pragma comment(lib, "libnoise.lib")

class Noise2D
{
public:

	Noise2D();
	Noise2D(const Noise2D& other) = default;
	~Noise2D() = default;

	static double GenerateValue(double x, double z);
	
	static int GetSeed();
	static void SetSeed(const int seed);

	static void SetScale(const double sc);

	static const double GetScale();

private:

	static noise::module::Perlin m_generator;
	static double m_scale;

};


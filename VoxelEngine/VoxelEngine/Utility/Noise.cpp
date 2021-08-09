#include "../Misc/pch.h"
#include "Noise.h"

using namespace noise::module;

Perlin Noise2D::m_generator = Perlin();

Noise2D::Noise2D()
{
	m_generator.SetFrequency(10.0f);
}

double Noise2D::GenerateValue(double x, double z)
{
	double val = m_generator.GetValue(x, z, 0.5);
	val = val * 0.5 + 0.5;
	val = val < 0.0 ? 0.0 : val > 1.0 ? 1.0 : val; // Clamp between 0-1
	return val;
}

int Noise2D::GetSeed()
{
	return m_generator.GetSeed();
}

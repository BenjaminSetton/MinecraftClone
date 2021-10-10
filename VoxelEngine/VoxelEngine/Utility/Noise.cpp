#include "../Misc/pch.h"
#include "Noise.h"

using namespace noise::module;

Perlin Noise2D::m_generator = Perlin();
double Noise2D::m_scale = 75.0;

Noise2D::Noise2D()
{
}

double Noise2D::GenerateValue(double x, double z)
{
	// We have to scale down the values
	double val = m_generator.GetValue(x / m_scale, z / m_scale, 0.5);
	val = val * 0.5 + 0.5;
	val = val < 0.0 ? 0.0 : val > 1.0 ? 1.0 : val; // Clamp between 0-1
	return val;
}

int Noise2D::GetSeed() { return m_generator.GetSeed(); }

void Noise2D::SetSeed(const int seed) { m_generator.SetSeed(seed); }

void Noise2D::SetScale(const double sc) { m_scale = sc; }

const double Noise2D::GetScale() { return m_scale; }
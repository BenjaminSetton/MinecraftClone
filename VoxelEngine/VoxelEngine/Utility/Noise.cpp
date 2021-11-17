#include "../Misc/pch.h"
#include "Noise.h"
#include "Math.h"

#include <random>

uint64_t Noise2D::m_seed = 12345123;
double Noise2D::m_scale = 0.01;
uint32_t Noise2D::m_octaveCount = 5;
double Noise2D::m_persistance = 0.3;

Noise2D::Noise2D()
{
}

double Noise2D::GenerateValue(double x, double y)
{
    x *= m_scale;
    y *= m_scale;

    double value = 0.0;
    double currentPersistance = 1.0;
    double currentFrequency = m_scale;

    for(uint32_t currOctave = 0; currOctave < m_octaveCount; currOctave++)
    {
        // Calculate weighed value
        double octaveValue = GenerateValueForOctave_Internal(x * currentFrequency, y * currentFrequency);

        // Add the weighed value to the total
        value += octaveValue * currentPersistance;

        // Increase frequency
        currentFrequency *= 2;

        // Re-calculate persistance for next octave value
        currentPersistance *= m_persistance;

        
    }

    if (value < 0.0f) value = 0.0f;
    else if (value > 1.0f) value = 1.0f;

    return value;

}

int Noise2D::GetSeed() { return m_seed; }
void Noise2D::SetSeed(const int seed) { m_seed = seed; srand(m_seed); }

void Noise2D::SetScale(const double sc) { m_scale = sc; }
const double Noise2D::GetScale() { return m_scale; }

void Noise2D::SetOctaveCount(const uint32_t octaveCount) { m_octaveCount = octaveCount; }
const uint32_t Noise2D::GetOctaveCount() { return m_octaveCount; }

void Noise2D::SetPersistance(const double persistance) { m_persistance = persistance; }
const double Noise2D::GetPersistance() { return m_persistance; }

double Noise2D::DotGridGradient(int ix, int iy, double x, double y)
{
    double gradientX, gradientY;
    // Get gradient from integer coordinates
    RandomGradient(ix, iy, gradientX, gradientY);

    // Compute the distance vector
    double dx = x - (double)ix;
    double dy = y - (double)iy;

    // Compute the dot-product
    return (dx * gradientX + dy * gradientY);
}

void Noise2D::RandomGradient(int ix, int iy, double& out_x, double& out_y)
{
    // Domain: [0, 2 * PI]
    float randMagnitude = (static_cast<float>(rand()) / RAND_MAX) * (3.14159265f * 2.0f);

    out_x = sin(randMagnitude);
    out_y = cos(randMagnitude);
}

/////////////////////////////////////////////////////////
//
//  Perlin noise algorithm taken from Perlin Noise wikipedia page
//  https://en.wikipedia.org/wiki/Perlin_noise
//
/////////////////////////////////////////////////////////
double Noise2D::GenerateValueForOctave_Internal(double x, double y)
{
    // Determine grid cell coordinates
    int x0 = static_cast<int>(x);
    int x1 = x0 + 1;
    int y0 = static_cast<int>(y);
    int y1 = y0 + 1;

    // Determine interpolation weights
    // Could also use higher order polynomial/s-curve here
    float sx = x - x0;
    float sy = y - y0;

    // Interpolate between grid point gradients
    float n0, n1, ix0, ix1, value;

    n0 = DotGridGradient(x0, y0, x, y);
    n1 = DotGridGradient(x1, y0, x, y);
    ix0 = VX_MATH::Lerp(n0, n1, sx);

    n0 = DotGridGradient(x0, y1, x, y);
    n1 = DotGridGradient(x1, y1, x, y);
    ix1 = VX_MATH::Lerp(n0, n1, sx);

    value = VX_MATH::Lerp(ix0, ix1, sy);

    // Normalize value
    value = (value + 1.0f) / 2.0f;

    return value;
}
#include "../Misc/pch.h"
#include "Noise.h"
#include "Math.h"

#include <random>

uint64_t Noise3D::m_seed = 12345123;
double Noise3D::m_frequency = 500.0;
uint32_t Noise3D::m_octaveCount = 5;
double Noise3D::m_persistance = 0.5;

Noise3D::Noise3D()
{
}

double Noise3D::GenerateValue(double x, double y, double z)
{
    x *= m_frequency;
    y *= m_frequency;
    z *= m_frequency;

    double value = 0.0;
    double currentPersistance = 1.0;

    for(uint32_t currOctave = 0; currOctave < m_octaveCount; currOctave++)
    {
        // Calculate weighed value
        double octaveValue = GenerateValueForOctave_Internal(x, y, z);

        // Add the weighed value to the total
        value += octaveValue * currentPersistance;

        // Re-calculate persistance for next octave value
        currentPersistance *= m_persistance;

        
    }

    if (value < 0.0f) value = 0.0f;
    else if (value > 1.0f) value = 1.0f;

    return value;

}

int Noise3D::GetSeed() { return m_seed; }
void Noise3D::SetSeed(const int seed) { m_seed = seed; srand(m_seed); }

void Noise3D::SetFrequency(const double sc) { m_frequency = sc; }
const double Noise3D::GetFrequency() { return m_frequency; }

void Noise3D::SetOctaveCount(const uint32_t octaveCount) { m_octaveCount = octaveCount; }
const uint32_t Noise3D::GetOctaveCount() { return m_octaveCount; }

void Noise3D::SetPersistance(const double persistance) { m_persistance = persistance; }
const double Noise3D::GetPersistance() { return m_persistance; }

double Noise3D::DotGridGradient(int ix, int iy, int iz, double x, double y, double z)
{
    double gradientX, gradientY, gradientZ;
    // Get gradient from integer coordinates
    RandomGradient(ix, iy, iz, gradientX, gradientY, gradientZ);

    // Compute the distance vector
    double dx = x - ix;
    double dy = y - iy;
    double dz = z - iz;

    // Compute the dot-product
    double dotProd = (dx * gradientX + dy * gradientY + dz * gradientZ);
    //VX_ASSERT(dotProd >= -1.0 && dotProd <= 1.0);
    return dotProd;
}

void Noise3D::RandomGradient(int ix, int iy, int iz, double& out_x, double& out_y, double& out_z)
{
    // Domain: [-1, 1]
    float randX = (static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f;
    float randY = (static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f;
    float randZ = (static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f;

    float magnitude = sqrt(randX * randX + randY * randY + randZ * randZ);

    // Normalize the vector
    out_x = randX / magnitude;
    out_y = randY / magnitude;
    out_z = randZ / magnitude;


}

double Noise3D::GenerateValueForOctave_Internal(double x, double y, double z)
{
    // Determine grid cell coordinates
    int x0 = x > 0.0 ? static_cast<int>(x) : static_cast<int>(x) - 1.0;
    int x1 = x0 + 1;
    int y0 = y > 0.0 ? static_cast<int>(y) : static_cast<int>(y) - 1.0;
    int y1 = y0 + 1;
    int z0 = z > 0.0 ? static_cast<int>(z) : static_cast<int>(z) - 1.0;
    int z1 = z0 + 1;

    // Determine interpolation weights
    // Could also use higher order polynomial/s-curve here
    double sx = VX_MATH::MapToCubicSCurve(x - x0);
    double sy = VX_MATH::MapToCubicSCurve(y - y0);
    double sz = VX_MATH::MapToCubicSCurve(z - z0);

    // Interpolate between grid point gradients
    double n0, n1, ix0, ix1, iy0, iy1;

    n0 = DotGridGradient(x0, y0, z0, x, y, z);
    n1 = DotGridGradient(x1, y0, z0, x, y, z);
    ix0 = VX_MATH::Lerp(n0, n1, sx);
    n0 = DotGridGradient(x0, y1, z0, x, y, z);
    n1 = DotGridGradient(x1, y1, z0, x, y, z);
    ix1 = VX_MATH::Lerp(n0, n1, sx);
    iy0 = VX_MATH::Lerp(ix0, ix1, sy);

    n0 = DotGridGradient(x0, y0, z1, x, y, z);
    n1 = DotGridGradient(x1, y0, z1, x, y, z);
    ix0 = VX_MATH::Lerp(n0, n1, sx);
    n0 = DotGridGradient(x0, y1, z1, x, y, z);
    n1 = DotGridGradient(x1, y1, z1, x, y, z);
    ix1 = VX_MATH::Lerp(n0, n1, sx);
    iy1 = VX_MATH::Lerp(ix0, ix1, sy);

    return VX_MATH::Lerp(iy0, iy1, sz);

}
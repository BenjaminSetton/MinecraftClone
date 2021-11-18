#ifndef _NOISE_H
#define _NOISE_H

class Noise3D
{
public:

	Noise3D();
	Noise3D(const Noise3D& other) = default;
	~Noise3D() = default;

	static double GenerateValue(double x, double y, double z);
	
	static int GetSeed();
	static void SetSeed(const int seed);

	static void SetFrequency(const double sc);
	static const double GetFrequency();

	static void SetOctaveCount(const uint32_t octaveCount);
	static const uint32_t GetOctaveCount();

	static void SetPersistance(const double persistance);
	static const double GetPersistance();

private:

	static double DotGridGradient(int ix, int iy, int iz, double x, double y, double z);

	static void RandomGradient(int ix, int iy, int iz, double& out_x, double& out_y, double& out_z);

	static double GenerateValueForOctave_Internal(double x, double y, double z);

	static uint64_t m_seed;
	static double m_frequency;				
	static uint32_t m_octaveCount;		// Optimally greater than 1
	static double m_persistance;		// Between 0 and 1

};

#endif

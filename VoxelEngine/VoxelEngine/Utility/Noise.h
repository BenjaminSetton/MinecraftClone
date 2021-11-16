#ifndef _NOISE_H
#define _NOISE_H

class Noise2D
{
public:

	Noise2D();
	Noise2D(const Noise2D& other) = default;
	~Noise2D() = default;

	static double GenerateValue(double x, double y);
	
	static int GetSeed();
	static void SetSeed(const int seed);

	static void SetScale(const double sc);

	static const double GetScale();

private:

	static double DotGridGradient(int ix, int iy, double x, double y);

	static void RandomGradient(int ix, int iy, double& out_x, double& out_y);

	static uint64_t m_seed;
	static double m_scale;

};

#endif

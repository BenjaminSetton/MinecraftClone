#include "Timer.h"

Timer::Timer()
{
	m_start = std::chrono::steady_clock::now();
}

Timer::~Timer()
{
	auto m_end = std::chrono::steady_clock::now();

	float duration = std::chrono::duration_cast<std::chrono::microseconds>(m_end - m_start).count();

	std::cout << "Timer took " << duration / 1000.0f << " ms (" << duration << " us)";
}
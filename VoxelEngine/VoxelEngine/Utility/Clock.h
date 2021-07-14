#ifndef _CLOCK_H
#define _CLOCK_H

#include <chrono>

class Clock
{
public:

	enum class TimePrecision
	{
		SECONDS,
		DECISECONDS,
		CENTISECONDS,
		MILLISECONDS,
		MICROSECONDS = 6,
		NANOSECONDS = 9
	};

	// Delete constructor, destructor and copy constructor for static clock class
	Clock();
	~Clock();
	Clock(const Clock&);

	static void Initialize();

	static void Signal();

	static float GetDeltaTime(const TimePrecision& precision);

	static float GetTimeSinceStart(const TimePrecision& precision);

private:

	static std::chrono::steady_clock::time_point m_start;
	static std::chrono::steady_clock::time_point m_lastSignal;

	// The delta time between signals stored in seconds
	static float m_deltaTime;

	// A bool to check whether the clock has been initialized or not
	static bool m_initialized;
};

#endif


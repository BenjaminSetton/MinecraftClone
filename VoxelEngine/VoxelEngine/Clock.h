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
		MICROSECONDS,
		NANOSECONDS
	};

	// Delete constructor, destructor and copy constructor for static clock class
	Clock() = delete;
	~Clock() = delete;
	Clock(const Clock&) = delete;

	static void Initialize();

	static void Signal();

	static float GetDeltaTime(const TimePrecision& precision);

	static float GetTimeSinceStart(const TimePrecision& precision);

private:

	static std::chrono::steady_clock::time_point m_start;
	static std::chrono::steady_clock::time_point m_lastSignal;

	// The delta time between signals stored in seconds
	static float m_deltaTime;
};

#endif


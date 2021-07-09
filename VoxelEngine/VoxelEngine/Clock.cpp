#include "Clock.h"

// Initialize the static Clock member variables
std::chrono::steady_clock::time_point Clock::m_start = std::chrono::steady_clock::time_point();
std::chrono::steady_clock::time_point Clock::m_lastSignal = std::chrono::steady_clock::time_point();
float Clock::m_deltaTime = 0;

void Clock::Initialize()
{
	m_start = std::chrono::steady_clock::now();
}

void Clock::Signal() 
{
	// Calculate delta time
	m_deltaTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - m_lastSignal).count();

	//Change the last signal to now()
	m_lastSignal = std::chrono::steady_clock::now();
}

float Clock::GetDeltaTime(const TimePrecision& precision)
{
	return m_deltaTime * powf(10.0f, static_cast<float>(precision));
}

float Clock::GetTimeSinceStart(const TimePrecision& precision)
{
	float res = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - m_start).count();
	return res * powf(10.0f, static_cast<float>(precision));
}
#include "../Misc/pch.h"

#include "Clock.h"

// Initialize the static Clock member variables
std::chrono::steady_clock::time_point Clock::m_start = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point Clock::m_lastSignal = std::chrono::steady_clock::time_point();
float Clock::m_deltaTime = 0;
bool Clock::m_initialized = false;


Clock::Clock(){}
Clock::~Clock(){}
Clock::Clock(const Clock&){}

void Clock::Initialize()
{
	//m_start = std::chrono::steady_clock::now();
}

void Clock::Signal() 
{
	// A check to fix the initial deltaTime issue
	if(!m_initialized)
	{
		m_initialized = true;
		m_lastSignal = std::chrono::steady_clock::now();
		return;
	}

	// Calculate delta time
	m_deltaTime = static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - m_lastSignal).count());

	//Change the last signal to now()
	m_lastSignal = std::chrono::steady_clock::now();
}

float Clock::GetDeltaTime(const TimePrecision& precision)
{
	// Since we start with microsecond precision, we subtract 6 to obtain 0 is seconds is input as precision
	return m_deltaTime * powf(10.0f, static_cast<float>(precision) - 9.0f);
}

float Clock::GetTimeSinceStart(const TimePrecision& precision)
{
	float res = static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - m_start).count());
	// Since we start with microsecond precision, we subtract 6 to obtain 0 is seconds is input as precision
	return res * powf(10.0f, static_cast<float>(precision) - 9.0f);
}
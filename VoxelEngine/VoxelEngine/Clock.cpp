#include "pch.h"

#include "Clock.h"

// Initialize the static Clock member variables
std::chrono::steady_clock::time_point Clock::m_start = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point Clock::m_lastSignal = std::chrono::steady_clock::time_point();
float Clock::m_deltaTime = 0;


Clock::Clock(){}
Clock::~Clock(){}
Clock::Clock(const Clock&){}

void Clock::Initialize()
{
	//m_start = std::chrono::steady_clock::now();
}

void Clock::Signal() 
{
	// Calculate delta time
	m_deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_lastSignal).count();

	//Change the last signal to now()
	m_lastSignal = std::chrono::steady_clock::now();
}

float Clock::GetDeltaTime(const TimePrecision& precision)
{
	// Since we start with microsecond precision, we subtract 6 to obtain 0 is seconds is input as precision
	return m_deltaTime * powf(10.0f, static_cast<float>(precision) - 6.0f);
}

float Clock::GetTimeSinceStart(const TimePrecision& precision)
{
	float res = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_start).count();
	// Since we start with microsecond precision, we subtract 6 to obtain 0 is seconds is input as precision
	return res * powf(10.0f, static_cast<float>(precision) - 6.0f);
}
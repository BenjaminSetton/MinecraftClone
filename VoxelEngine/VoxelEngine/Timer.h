#pragma once

#include <chrono>
#include <iostream>

class Timer
{
public:

	Timer();

	~Timer();

private:
	std::chrono::steady_clock::time_point m_start;
};


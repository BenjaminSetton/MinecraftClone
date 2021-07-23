#ifndef _SCOPETIMER_H
#define _SCOPETIMER_H


#include <chrono>
#include <iostream>
#include "../../imgui/imgui.h"

// A small scope timer class that gets destroyed when it goes out of scope
// and outputs the time it was active

class ScopeTimer
{
public:

	ScopeTimer();
	ScopeTimer(std::string name);
	ScopeTimer(const ScopeTimer& other) = default;

	~ScopeTimer();

private:
	std::chrono::steady_clock::time_point m_start;
	std::string m_name;
};

#endif


#include "../Misc/pch.h"

#include "ScopeTimer.h"

ScopeTimer::ScopeTimer() : m_name(std::string("Unnamed"))
{
	m_start = std::chrono::steady_clock::now();
}

ScopeTimer::ScopeTimer(std::string name) : m_name(name)
{
	m_start = std::chrono::steady_clock::now();
}

ScopeTimer::~ScopeTimer()
{
	auto m_end = std::chrono::steady_clock::now();

	float duration = std::chrono::duration_cast<std::chrono::microseconds>(m_end - m_start).count();

	//std::cout << m_name << " took " << duration / 1000.0f << " ms (" << duration << " us)\n";

	ImGui::Begin("Debug Panel");
	ImGui::Text("\"%s\" scope took %2.3f ms (%3.0f us)", m_name.c_str(), duration / 1000.0f, duration);
	ImGui::End();
}
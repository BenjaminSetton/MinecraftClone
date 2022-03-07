#include "../Misc/pch.h"

#include "ScopeTimer.h"
#include "Log.h"

ScopeTimer::ScopeTimer(std::string name, const int mode, float* outDeltaTime) : 
	m_name(name), m_mode(mode), m_timerVariable(outDeltaTime)
{
	m_start = std::chrono::steady_clock::now();
}

ScopeTimer::~ScopeTimer()
{
	auto m_end = std::chrono::steady_clock::now();

	float duration = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(m_end - m_start).count());

	if (m_timerVariable)
	{
		(*m_timerVariable) = duration / 1000.0f;
		m_timerVariable = nullptr;
		return;
	}

	switch(m_mode)
	{
	case 0:
	{
		ImGui::Begin("Debug Panel");
		//ImGui::Text("\"%s\" scope took %2.3f ms (%3.0f us)", m_name.c_str(), duration / 1000.0f, duration);
		ImGui::Text("\"%s\" runs at %3.3f FPS (%2.3f ms)", m_name.c_str(), 1000000.0f / duration, duration / 1000.0f);
		ImGui::End();
		break;
	}
	case 1:
	{
		Log log;
		log << m_name << " took " << duration / 1000.0f << " ms";
		log.End();
		break;
	}
	}
}
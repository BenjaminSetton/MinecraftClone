#include "../Misc/pch.h"
#include "DayNightCycle.h"

#include "../../imgui/imgui.h"

constexpr float SUNRISE_THRESHHOLD = 0.05f;
constexpr float SUNSET_THRESHHOLD = 0.05f;

using namespace DirectX;

XMFLOAT3 DayNightCycle::m_lightPos = { 0.0f, 0.0f, 0.0f };
XMFLOAT3 DayNightCycle::m_lightDir = {-1, 0, 0 };
XMFLOAT4 DayNightCycle::m_lightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
float DayNightCycle::m_cycleDuration = 60.0f;
DayNightCycle::Cycle DayNightCycle::m_cycle = Cycle::DAY;
DayNightCycle::Time DayNightCycle::m_time = Time::SUNRISE;

// #ff8c00
DirectX::XMFLOAT4 DayNightCycle::m_sunriseColor = { 1.0f, 0.549f, 0.0f, 1.0f };

// #f0dc82
DirectX::XMFLOAT4 DayNightCycle::m_middayColor = { 0.941f, 0.863f, 0.51f, 1.0f};

// #fd5e53
DirectX::XMFLOAT4 DayNightCycle::m_sunsetColor = { 0.992f, 0.369f, 0.325f, 1.0f };

// #152852
DirectX::XMFLOAT4 DayNightCycle::m_midnightColor = { 0.082f, 0.157f, 0.322f, 1.0f };
float DayNightCycle::m_elapsedTime = 0.0f;


void DayNightCycle::Update(const float& dt)
{
	m_elapsedTime += dt;
	if (m_elapsedTime >= m_cycleDuration) m_elapsedTime -= m_cycleDuration;

	float timePct = m_elapsedTime / m_cycleDuration; // Ranges from 0-1
	
	// Set the light pos
	m_lightDir = { -cos(2.0f * XM_PI * timePct), -(sin(2.0f * XM_PI * timePct)), 0 };

	float normDot = (XMVector3Dot(XMLoadFloat3(&m_lightDir), { 0.0f, 1.0f, 0.0f }).m128_f32[0]);

	if(m_lightDir.y < 0) // There was a change in Cycle (night->day or day->night)
	{ 
		m_lightDir.y *= -1.0f;
		m_lightDir.x *= -1.0f;

		// Change variables
		m_cycle = m_cycle == Cycle::DAY ? Cycle::NIGHT : Cycle::DAY;
		
		if(m_cycle == Cycle::DAY)
		{
			if (normDot < SUNRISE_THRESHHOLD) m_time = Time::SUNRISE;
			else if (normDot < 1.0f - SUNRISE_THRESHHOLD) m_time = Time::DAYTIME;
			else m_time = Time::MIDDAY;
		}
		else
		{
			if (normDot < SUNSET_THRESHHOLD) m_time = Time::SUNSET;
			else if (normDot < 1.0f - SUNSET_THRESHHOLD) m_time = Time::NIGHTTIME;
			else m_time = Time::MIDNIGHT;
		}
	}

	// Set the light color - interpolate between midnight and midday

	XMFLOAT4 startColor;
	XMFLOAT4 endColor;

	// Interpolate from SUNRISE to DAYTIME
	if(m_time == Time::SUNRISE)
	{
		startColor = m_sunriseColor;
		endColor = m_middayColor;
	}
	else if(m_time == Time::MIDDAY)
	{
		startColor = m_sunsetColor;
		endColor = m_middayColor;
	}
	else if(m_time == Time::SUNSET)
	{
		startColor = m_sunsetColor;
		endColor = m_midnightColor;
	}
	else if(m_time == Time::MIDNIGHT)
	{
		startColor = m_sunriseColor;
		endColor = m_midnightColor;
	}

	XMStoreFloat4(&m_lightColor, XMVectorLerp(XMLoadFloat4(&startColor), XMLoadFloat4(&endColor), normDot));
	
	
	// Set the light position
	m_lightPos = { -m_lightDir.x, -m_lightDir.y, 0};

	// IMGUI DEBUG PANEL
	ImGui::Begin("Day/Night Cycle Debug");
	ImGui::Text("Light Position: %2.2f, %2.2f, %2.2f", m_lightPos.x, m_lightPos.y, m_lightPos.z);
	ImGui::Text("Light Direction: %2.2f, %2.2f, %2.2f", m_lightDir.x, m_lightDir.y, m_lightDir.z);
	ImGui::Text("Light Color: %2.2f, %2.2f, %2.2f, 1.0", m_lightColor.x, m_lightColor.y, m_lightColor.z);
	ImGui::End();
}

const DayNightCycle::Cycle DayNightCycle::GetCycle() { return m_cycle; }

const DayNightCycle::Time DayNightCycle::GetTime() { return m_time; }

const DirectX::XMFLOAT3 DayNightCycle::GetLightPosition() { return m_lightPos; }

const DirectX::XMFLOAT3 DayNightCycle::GetLightDirection() { return m_lightDir; }

const DirectX::XMFLOAT4 DayNightCycle::GetLightColor() { return m_lightColor; }


#include "../Misc/pch.h"
#include "DayNightCycle.h"

#include "../../imgui/imgui.h"

constexpr float SUNRISE_THRESHHOLD = 0.05f;
constexpr float SUNSET_THRESHHOLD = 0.05f;

constexpr float BODY_POS_SCALE = 50.0f;

using namespace DirectX;

XMFLOAT3 DayNightCycle::m_sunPos = { 0.0f, 0.0f, 0.0f };
XMFLOAT3 DayNightCycle::m_moonPos = { 0.0f, 0.0f, 0.0f };

DirectionalLight DayNightCycle::m_sun = DirectionalLight({ -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
float DayNightCycle::m_cycleDuration = 30.0f;
DayNightCycle::Cycle DayNightCycle::m_cycle = Cycle::DAY;
DayNightCycle::Time DayNightCycle::m_time = Time::SUNRISE;

// Sun colors
DirectX::XMFLOAT4 DayNightCycle::m_sSunriseColor = { 1.0f, 0.549f, 0.0f, 1.0f };
DirectX::XMFLOAT4 DayNightCycle::m_sMiddayColor = { 0.991f, 0.913f, 0.76f, 1.0f};
DirectX::XMFLOAT4 DayNightCycle::m_sSunsetColor = { 0.992f, 0.369f, 0.325f, 1.0f };

// Moon colors
DirectX::XMFLOAT4 DayNightCycle::m_mSunsetColor = { 0.202f, 0.277f, 0.420f, 1.0f };
DirectX::XMFLOAT4 DayNightCycle::m_mMidnightColor = { 0.082f, 0.157f, 0.322f, 1.0f };
DirectX::XMFLOAT4 DayNightCycle::m_mSunriseColor = { 0.169f, 0.189f, 0.387f, 1.0f };

float DayNightCycle::m_elapsedTime = 0.0f;


void DayNightCycle::Update(const float& dt)
{
	static bool timeChanged = false;

	m_elapsedTime += dt;

	// There was a change in Cycle (night->day or day->night)
	if (m_elapsedTime >= m_cycleDuration)
	{
		m_elapsedTime -= m_cycleDuration;
		m_cycle = m_cycle == Cycle::DAY ? Cycle::NIGHT : Cycle::DAY;
		timeChanged = true;
	}

	float timePct = m_elapsedTime / m_cycleDuration; // Ranges from 0-1
	
	// Set the light dir - sun rises east, sets west
	XMFLOAT3 lightDir = { -cos(XM_PI * timePct), -(abs(sin(XM_PI * timePct))), 0 };
	m_sun.SetDirection(lightDir);

	XMVECTOR dirToDot = {lightDir.x, -abs(lightDir.y), lightDir.z, 1.0f};
	float normDot = (XMVector3Dot(dirToDot, { 0.0f, 1.0f, 0.0f }).m128_f32[0]);

	if(timeChanged)
	{
		
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
		timeChanged = false;
	}

	// Set the light color - interpolate between midnight and midday

	XMFLOAT4 startSunColor = {};
	XMFLOAT4 endSunColor = {};
	XMFLOAT4 startMoonColor = {};
	XMFLOAT4 endMoonColor = {};

	// Interpolate from SUNRISE to DAYTIME
	if(m_time == Time::SUNRISE)
	{
		startSunColor = m_sSunriseColor;
		endSunColor = m_sMiddayColor;
	}
	else if(m_time == Time::MIDDAY)
	{
		startSunColor = m_sSunsetColor;
		endSunColor = m_sMiddayColor;
	}
	else if(m_time == Time::SUNSET)
	{
		startMoonColor = m_mSunsetColor;
		endMoonColor = m_mMidnightColor;
	}
	else if(m_time == Time::MIDNIGHT)
	{
		startSunColor = m_sunriseColor;
		endSunColor = m_midnightColor;
	}
	XMFLOAT4 sunCol;
	XMFLOAT4 moonCol;
	XMStoreFloat4(&sunCol, XMVectorLerp(XMLoadFloat4(&startSunColor), XMLoadFloat4(&endSunColor), normDot));
	m_sun.SetColor(sunCol);
	m_moon.SetColor(moonCol);
	
	// Set the light position
	m_lightPos = { -m_lightDir.x, -m_lightDir.y, 0};

	// IMGUI DEBUG PANEL
	ImGui::Begin("Day/Night Cycle Debug");
	ImGui::Text("Light Direction: %2.2f, %2.2f, %2.2f", m_lightDir.x, m_lightDir.y, m_lightDir.z);
	ImGui::Text("Light Position: %2.2f, %2.2f, %2.2f", m_lightPos.x, m_lightPos.y, m_lightPos.z);
	ImGui::Text("Light Color: %2.2f, %2.2f, %2.2f, 1.0", m_lightColor.x, m_lightColor.y, m_lightColor.z);
	const char* time = m_cycle == Cycle::DAY ? "Day" : "Night";
	ImGui::Text("Time Elapsed: %2.3f seconds (%1.2f)", m_elapsedTime, timePct);
	ImGui::Text(time);
	ImGui::End();
}

const DayNightCycle::Cycle DayNightCycle::GetCycle() { return m_cycle; }

const DayNightCycle::Time DayNightCycle::GetTime() { return m_time; }

const DirectX::XMFLOAT3 DayNightCycle::GetLightPosition(const CelestialBody body) 
{
	if (body == CelestialBody::SUN) return m_sunPos;
	else return m_moonPos;
}

const DirectX::XMFLOAT3 DayNightCycle::GetLightDirection(const CelestialBody body)
{
	if (body == CelestialBody::SUN) return m_sun.GetDirection();
	else return m_moon.GetDirection();
}

const DirectX::XMFLOAT4 DayNightCycle::GetLightColor(const CelestialBody body)
{
	if (body == CelestialBody::SUN) return m_sun.GetColor();
	else return m_moon.GetColor();
}

const float DayNightCycle::GetLightAmbient(const CelestialBody body)
{
	if (body == CelestialBody::SUN) return m_sunAmbient;
	else return m_moonAmbient;
}


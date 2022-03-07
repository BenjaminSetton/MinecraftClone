#include "../Misc/pch.h"
#include "DayNightCycle.h"
#include "../Utility/Utility.h"

#include "../Utility/DebugRenderer.h"

#include "../Utility/ImGuiLayer.h"


using namespace DirectX;


constexpr float SUNRISE_THRESHHOLD = 0.1f;
constexpr float SUNSET_THRESHHOLD = 0.1f;
constexpr float BODY_POS_SCALE = 50.0f;

// Sun colors
constexpr XMFLOAT4 ce_sunColors[4] =
{
	{ 1.0f,   0.549f, 0.0f,   1.0f },	// SUNRISE
	{ 0.991f, 0.913f, 0.89f,  1.0f },	// MIDDAY
	{ 0.992f, 0.369f, 0.325f, 1.0f },	// SUNSET
	{ 0.0f,   0.0f,   0.0f,   1.0f }	// MIDNIGHT
};

constexpr XMFLOAT4 ce_moonColors[4] =
{
	{ 0.169f, 0.189f, 0.387f, 1.0f },	// SUNRISE
	{ 0.0f,   0.0f,   0.0f,   1.0f },	// MIDDAY
	{ 0.202f, 0.277f, 0.420f, 1.0f },	// SUNSET
	{ 0.082f, 0.157f, 0.322f, 1.0f }	// MIDNIGHT
};

constexpr XMFLOAT4 ce_skyColors[4] =
{
	{ 1.0f,   0.549f, 0.0f,   1.0f },	// SUNRISE
	{ 0.529f, 0.807f, 0.922f, 1.0f },	// MIDDAY
	{ 0.992f, 0.369f, 0.325f, 1.0f },	// SUNSET
	{ 0.063f, 0.063f, 0.275f, 1.0f }	// MIDNIGHT
};

// Ambient values
constexpr float ce_sunAmbient = 0.2f;
constexpr float ce_moonAmbient = 0.07f;

// Z celestial body offset
constexpr float ce_zOffset = 0.25f;

constexpr float ce_cycleDuration = 30.0f;


XMFLOAT3 DayNightCycle::m_sunPos = { 0.0f, 0.0f, 0.0f };
XMFLOAT3 DayNightCycle::m_moonPos = { 0.0f, 0.0f, 0.0f };

DirectionalLight DayNightCycle::m_sun = DirectionalLight({ -0.5f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
DirectionalLight DayNightCycle::m_moon = DirectionalLight({ 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
DayNightCycle::Cycle DayNightCycle::m_cycle = Cycle::DAY;
DayNightCycle::Time DayNightCycle::m_time = Time::SUNRISE;

XMFLOAT4 DayNightCycle::m_skyColor = { 0.0f, 0.0f, 0.0f, 0.0f };

float DayNightCycle::m_elapsedTime = 0.0f;


void DayNightCycle::Update(const float& dt)
{
	if (Cycle_Data::pauseCycle) return;

	m_elapsedTime += dt;

	if(m_elapsedTime >= ce_cycleDuration * 2.0f)
	{
		m_elapsedTime -= ce_cycleDuration * 2.0f;
	}

	float timePct = m_elapsedTime / ce_cycleDuration; // Ranges from 0-2

	if (timePct < 1.0f) m_cycle = Cycle::DAY;
	else if (timePct >= 1.0f && timePct <= 2.0f) m_cycle = Cycle::NIGHT;
	else VX_ASSERT(false);

	
	// Set the light dir - sun rises east, sets west
	XMFLOAT3 sunDir = { -cos(XM_PI * timePct), -sin(XM_PI * timePct), ce_zOffset };
	XMStoreFloat3(&sunDir, XMVector3Normalize(XMLoadFloat3(&sunDir)));
	XMFLOAT3 moonDir = { -sunDir.x, -sunDir.y, -sunDir.z };

	// Set the light directions
	m_sun.SetDirection(sunDir);
	m_moon.SetDirection(moonDir);

	// Set the light positions (for shadow map use)
	m_sunPos = { -sunDir.x * BODY_POS_SCALE, -sunDir.y * BODY_POS_SCALE, -sunDir.z * BODY_POS_SCALE };
	m_moonPos = { -moonDir.x * BODY_POS_SCALE, -moonDir.y * BODY_POS_SCALE, -moonDir.z * BODY_POS_SCALE };

	XMVECTOR dirToDot = { sunDir.x, abs(sunDir.y), sunDir.z };
	float normDot = (XMVector3Dot(dirToDot, XMVector3Normalize({ 0.0f, 1.0f, ce_zOffset })).m128_f32[0]);


	if(m_cycle == Cycle::DAY) // normDot
	{
		if (normDot < SUNRISE_THRESHHOLD && timePct < 0.5f) m_time = Time::SUNRISE;
		else if (normDot < SUNSET_THRESHHOLD && timePct > 0.5f) m_time = Time::SUNSET;
		else if (normDot < 1.0f - SUNRISE_THRESHHOLD) m_time = Time::DAYTIME;
		else m_time = Time::MIDDAY;
	}
	else
	{
		if (normDot < 1.0f - SUNSET_THRESHHOLD) m_time = Time::NIGHTTIME;
		else m_time = Time::MIDNIGHT;
	}


	// Set the light color - interpolate between midnight and midday
	int startIndex, endIndex = 0;
	float lerpRatio;

	startIndex = static_cast<int>(timePct / 0.5f);
	endIndex = startIndex + 1 >= 4 ? 0 : startIndex + 1;
	lerpRatio = (timePct / 0.5f) - static_cast<int>(timePct / 0.5f);

	// Set the sun's color
	XMFLOAT4 sunCol;
	DirectX::XMStoreFloat4(&sunCol, XMVectorLerp(XMLoadFloat4(&ce_sunColors[startIndex]), XMLoadFloat4(&ce_sunColors[endIndex]), lerpRatio));
	m_sun.SetColor(sunCol);

	// Set the moon's color
	XMFLOAT4 moonCol;
	DirectX::XMStoreFloat4(&moonCol, XMVectorLerp(XMLoadFloat4(&ce_moonColors[startIndex]), XMLoadFloat4(&ce_moonColors[endIndex]), lerpRatio));
	m_moon.SetColor(moonCol);

	// Set the sky's color
	DirectX::XMStoreFloat4(&m_skyColor, XMVectorLerp(XMLoadFloat4(&ce_skyColors[startIndex]), XMLoadFloat4(&ce_skyColors[endIndex]), lerpRatio));


	// IMGUI DEBUG PANEL
#pragma region _IMGUI_DEBUG

	Cycle_Data::sunColor = m_sun.GetColor();
	Cycle_Data::moonColor = m_moon.GetColor();

	Cycle_Data::sunDir = sunDir;
	Cycle_Data::moonDir = moonDir;
	Cycle_Data::sunPos = m_sunPos;
	Cycle_Data::moonPos = m_moonPos;

	Cycle_Data::elapsedTime = m_elapsedTime;
	Cycle_Data::timePct = timePct;

	Cycle_Data::cycle = m_cycle == Cycle::DAY ? "Day" : "Night";
	
	switch (m_time)
	{
	case Time::DAYTIME:
		Cycle_Data::time = "DAYTIME";
		break;
	case Time::MIDDAY:
		Cycle_Data::time = "MIDDAY";
		break;
	case Time::MIDNIGHT:
		Cycle_Data::time = "MIDNIGHT";
		break;
	case Time::NIGHTTIME:
		Cycle_Data::time = "NIGHTTIME";
		break;
	case Time::SUNRISE:
		Cycle_Data::time = "SUNRISE";
		break;
	case Time::SUNSET:
		Cycle_Data::time = "SUNSET";
		break;
	}

#pragma endregion

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
	if (body == CelestialBody::SUN) return ce_sunAmbient;
	else return ce_moonAmbient;
}

const XMFLOAT4 DayNightCycle::GetSkyColor() { return m_skyColor; }


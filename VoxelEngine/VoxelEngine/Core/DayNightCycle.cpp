#include "../Misc/pch.h"
#include "DayNightCycle.h"
#include "../Utility/Utility.h"

#include "../../imgui/imgui.h"

constexpr float SUNRISE_THRESHHOLD = 0.1f;
constexpr float SUNSET_THRESHHOLD = 0.1f;

constexpr float BODY_POS_SCALE = 50.0f;

// Sun colors
constexpr DirectX::XMFLOAT4 ce_sunColors[4] =
{
	{ 1.0f, 0.549f, 0.0f, 1.0f },		// SUNRISE
	{ 0.991f, 0.913f, 0.89f, 1.0f },	// MIDDAY
	{ 0.992f, 0.369f, 0.325f, 1.0f },	// SUNSET
	{ 0.0f, 0.0f, 0.0f, 1.0f }			// MIDNIGHT
};

constexpr DirectX::XMFLOAT4 ce_moonColors[4] =
{
	{ 0.169f, 0.189f, 0.387f, 1.0f },	// SUNRISE
	{ 0.0f, 0.0f, 0.0f, 1.0f },			// MIDDAY
	{ 0.202f, 0.277f, 0.420f, 1.0f },	// SUNSET
	{ 0.082f, 0.157f, 0.322f, 1.0f }	// MIDNIGHT
};

// Ambient values
constexpr float ce_sunAmbient = 0.2f;
constexpr float ce_moonAmbient = 0.07f;

// Z celestial body offset
constexpr float ce_zOffset = 0.25f;

constexpr float ce_cycleDuration = 30.0f;

using namespace DirectX;

XMFLOAT3 DayNightCycle::m_sunPos = { 0.0f, 0.0f, 0.0f };
XMFLOAT3 DayNightCycle::m_moonPos = { 0.0f, 0.0f, 0.0f };

DirectionalLight DayNightCycle::m_sun = DirectionalLight({ -0.5f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
DirectionalLight DayNightCycle::m_moon = DirectionalLight({ 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
DayNightCycle::Cycle DayNightCycle::m_cycle = Cycle::DAY;
DayNightCycle::Time DayNightCycle::m_time = Time::SUNRISE;

float DayNightCycle::m_elapsedTime = 0.0f;
float DayNightCycle::m_inflexionTime = 0.0f;


void DayNightCycle::Update(const float& dt)
{
	//static float timePct = 0.0f;
	bool inflexionHappened = false;

	ImGui::Begin("Day/Night Cycle Debug");

	//ImGui::SliderFloat("TimePct", &timePct, 0.0f, 2.0f, "%2.2f", 1.0f);

	m_elapsedTime += dt;
	m_inflexionTime += dt;

	// There was a change in Cycle (night->day or day->night)

	if(m_inflexionTime >= 0.5f)
	{
		inflexionHappened = true;
		m_inflexionTime -= 0.5f;
	}

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

	XMFLOAT4 sunCol;
	DirectX::XMStoreFloat4(&sunCol, XMVectorLerp(XMLoadFloat4(&ce_sunColors[startIndex]), XMLoadFloat4(&ce_sunColors[endIndex]), lerpRatio));
	m_sun.SetColor(sunCol);

	XMFLOAT4 moonCol;
	DirectX::XMStoreFloat4(&moonCol, XMVectorLerp(XMLoadFloat4(&ce_moonColors[startIndex]), XMLoadFloat4(&ce_moonColors[endIndex]), lerpRatio));
	m_moon.SetColor(moonCol);



	// IMGUI DEBUG PANEL
	XMFLOAT4 sunColor = m_sun.GetColor();
	XMFLOAT4 moonColor = m_moon.GetColor();

	ImGui::Text("Sun/Moon Direction: (%2.2f, %2.2f, %2.2f) / (%2.2f, %2.2f, %2.2f)", 
		sunDir.x, sunDir.y, sunDir.z, moonDir.x, moonDir.y, moonDir.z);
	ImGui::Text("Sun/Moon Position: (%2.2f, %2.2f, %2.2f) / (%2.2f, %2.2f, %2.2f)",
		m_sunPos.x, m_sunPos.y, m_sunPos.z, m_moonPos.x, m_moonPos.y, m_moonPos.z);

	ImGui::Text("Sun Color "); ImGui::SameLine();
	ImGui::ColorButton("Sun Color", {sunColor.x, sunColor.y, sunColor.z, 1.0f}, ImGuiColorEditFlags_NoAlpha);

	ImGui::Text("Moon Color "); ImGui::SameLine();
	ImGui::ColorButton("Moon Color", { moonColor.x, moonColor.y, moonColor.z, 1.0f }, ImGuiColorEditFlags_NoAlpha);

	ImGui::Text("NormDot: %2.2f", normDot);
	ImGui::Text("LerpRatio: %2.2f", lerpRatio);
	ImGui::Text("Time Elapsed: %2.3f seconds (%1.2f)", m_elapsedTime, timePct * 100.0f);
	const char* cycle = m_cycle == Cycle::DAY ? "Day" : "Night";
	ImGui::Text(cycle);
	const char* time = "";
	switch(m_time)
	{
	case Time::DAYTIME:
		time = "DAYTIME";
		break;
	case Time::MIDDAY:
		time = "MIDDAY";
		break;
	case Time::MIDNIGHT:
		time = "MIDNIGHT";
		break;
	case Time::NIGHTTIME:
		time = "NIGHTTIME";
		break;
	case Time::SUNRISE:
		time = "SUNRISE";
		break;
	case Time::SUNSET:
		time = "SUNSET";
		break;
	}
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
	if (body == CelestialBody::SUN) return ce_sunAmbient;
	else return ce_moonAmbient;
}


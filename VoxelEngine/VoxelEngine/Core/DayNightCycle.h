#ifndef _DAY_NIGHT_CYCLE_H
#define _DAY_NIGHT_CYCLE_H

#include <DirectXMath.h>

class DayNightCycle
{
public:
	enum class Cycle
	{
		DAY,
		NIGHT
	};

	enum class Time
	{
		SUNRISE = 0,
		DAYTIME,
		MIDDAY,
		SUNSET,
		NIGHTTIME,
		MIDNIGHT
	};

	static void Update(const float& dt);

	static const Cycle GetCycle();
	static const Time GetTime();
	static const DirectX::XMFLOAT3 GetLightPosition();
	static const DirectX::XMFLOAT3 GetLightDirection();
	static const DirectX::XMFLOAT4 GetLightColor();

private:

	static DirectX::XMFLOAT3 m_lightPos;
	static DirectX::XMFLOAT3 m_lightDir;
	static DirectX::XMFLOAT4 m_lightColor;
	static float m_cycleDuration;
	static Cycle m_cycle;
	static Time m_time;

	// For internal use only
	static float m_elapsedTime;
	static DirectX::XMFLOAT4 m_sunriseColor;
	static DirectX::XMFLOAT4 m_middayColor;
	static DirectX::XMFLOAT4 m_sunsetColor;
	static DirectX::XMFLOAT4 m_midnightColor;
};

#endif


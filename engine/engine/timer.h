#pragma once
#include <ctime>

class CTimer
{
private:
	INT64 m_startTime;
	INT64 m_lastDelta;
	INT64 m_lastTime;
	INT64 m_frequency;
	float m_gameScale;

public:
	CTimer()
		: m_lastDelta(0)
		, m_gameScale(1.f)
	{
		QueryPerformanceFrequency( reinterpret_cast<LARGE_INTEGER*>(&m_frequency) );
		QueryPerformanceCounter( reinterpret_cast<LARGE_INTEGER*>(&m_startTime) );
		m_lastTime = m_startTime;
		m_frequency *= 1000000;
	}

	void SetGameScale(float const scale)
	{
		m_gameScale = scale;
	}

	void Tick()
	{
		INT64 const prevTime = m_lastTime;
		QueryPerformanceCounter( reinterpret_cast<LARGE_INTEGER*>(&m_lastTime) );
		m_lastDelta = m_lastTime - prevTime;
	}

	INT64 TimeFromStart() const
	{
		INT64 currentTime = 0;
		QueryPerformanceCounter( reinterpret_cast<LARGE_INTEGER*>(&currentTime) );
		return currentTime - m_startTime;
	}

	INT64 LastDelta() const
	{
		return m_lastDelta;
	}

	float Delta() const
	{
		INT64 const lastDeltaUS = m_lastDelta * 1000000;
		return static_cast<float>(lastDeltaUS) / static_cast<float>(m_frequency);
	}

	float GameDelta() const
	{
		return m_gameScale * Delta();
	}

	float GetSeconds( INT64 const time ) 
	{ 
		INT64 const timeUS = time * 1000000;
		return static_cast<float>(timeUS) / static_cast<float>(m_frequency);
	}
};

extern CTimer GTimer;
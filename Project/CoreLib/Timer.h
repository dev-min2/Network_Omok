#pragma once

#include <Windows.h>

enum TimeType
{
	TT_deltaTime = 0,
	TT_fps,
	TT_fpsTime,
	TT_gameTime
};

class Timer
{
public:
	Timer();
	~Timer();
public:
	bool	Init();
	bool	Update();
	bool	Render();
	bool	Release();
public:
	int		GetFPS();
private:
	LARGE_INTEGER m_frequency;
	LARGE_INTEGER m_curTick;
	LARGE_INTEGER m_prevTick;
	float		  m_timeTypes[4];
	int			  m_fps;
	float		  m_fpsTimer;
	int			  m_fpsCounter;
	
};


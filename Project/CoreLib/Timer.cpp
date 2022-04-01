#include "Timer.h"
float g_deltaTime = 0.0f;
float g_gameTime = 0.0f;
Timer::Timer() : m_fps(0),m_fpsCounter(0),m_fpsTimer(0.0f)
{
}
Timer::~Timer()
{
}
bool Timer::Init()
{
    ::QueryPerformanceFrequency(&m_frequency);
    ::QueryPerformanceCounter(&m_prevTick);
    return true;
}

bool Timer::Update()
{
    ::QueryPerformanceCounter(&m_curTick);

    // 델타타임.
    m_timeTypes[TT_deltaTime] = (m_curTick.QuadPart - m_prevTick.QuadPart) / static_cast<float>(m_frequency.QuadPart);
    m_timeTypes[TT_gameTime] += m_timeTypes[TT_deltaTime];
    

    g_deltaTime = m_timeTypes[TT_deltaTime];
    g_gameTime  += m_timeTypes[TT_gameTime];

    m_fpsTimer += m_timeTypes[TT_deltaTime];
    if (m_fpsTimer >= 1.0f) // 1초가 지나면
    {
        m_fps = m_fpsCounter;
        m_fpsCounter = 0;
        m_fpsTimer -= 1.0f;
    }
    ++m_fpsCounter;


    m_prevTick = m_curTick; // 이전틱을 현재틱으로 변경.
    return true;
}

bool Timer::Render()
{
    return true;
}

bool Timer::Release()
{
    return true;
}

int Timer::GetFPS()
{
    return m_fps;
}

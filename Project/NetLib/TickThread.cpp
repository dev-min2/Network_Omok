#include "TickThread.h"
#include "ThreadManager.h"
TickThread::TickThread() : m_hThread(INVALID_HANDLE_VALUE),m_isStarted(false),m_threadID(0), m_waitTick(0), m_hQuitEvent(::WSACreateEvent())
{
}

TickThread::~TickThread()
{
    ::CloseHandle(m_hQuitEvent);
}

void TickThread::BeginThread(DWORD waitTick)
{
    if (m_isStarted == true)
        return;
    m_waitTick = waitTick;
    m_isStarted = true;
    m_hThread = ThreadManager::GetInstance()->Spawn(TickThread::TickThreadRunner, this, &m_threadID);
}

void TickThread::DestoryThread()
{
    if (m_isStarted)
    {
        ::SetEvent(m_hQuitEvent);
    }
}

unsigned int WINAPI TickThread::TickThreadRunner(LPVOID parameter)
{
    TickThread* thread = static_cast<TickThread*>(parameter);
    thread->Run();
	return 0;
}

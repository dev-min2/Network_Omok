#include "Thread.h"
#include "ThreadManager.h"
Thread::Thread() : m_isStarted(false),m_threadID(0),m_hThread(INVALID_HANDLE_VALUE),m_hThreadKillEvent(::WSACreateEvent())
{
}

Thread::~Thread()
{
    ::CloseHandle(m_hThreadKillEvent);
    // 핸들닫기는 쓰레드매니저에서 처리.
}


void Thread::BeginThread()
{
    if (m_isStarted == true)
        return;
    m_isStarted = true;
    m_hThread = ThreadManager::GetInstance()->Spawn(Thread::ThreadRunner, this,&m_threadID);
}

void Thread::DestroyThread()
{

    if (m_isStarted)
    {
        ::SetEvent(m_hThreadKillEvent);
        m_isStarted = false;
    }
}



unsigned int WINAPI Thread::ThreadRunner(LPVOID parameter)
{
    Thread* thread = static_cast<Thread*>(parameter);
    thread->Run();

    return 0;
}

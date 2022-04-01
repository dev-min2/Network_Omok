#include "ThreadManager.h"

ThreadManager* ThreadManager::m_instance = nullptr;

ThreadManager* ThreadManager::GetInstance()
{
    if (m_instance == nullptr)
        m_instance = new ThreadManager();
    return m_instance;
}

void ThreadManager::ReleaseInstance()
{
    if (m_instance != nullptr)
        delete m_instance;
    m_instance = nullptr;
}

void ThreadManager::Join()
{
    // 생성된 쓰레드들이 종료될때까지 대기.
    ListThreadPos pos;

    pos = m_listThread.begin();
    while (pos != m_listThread.end())
    {
        ::WaitForSingleObject(*pos, INFINITE);
        
        ::CloseHandle(*pos);
        pos = m_listThread.erase(pos); // 지우는 이유는 대기에서 빠져나왔으므로(빠져나왔다는건 killEvent가 Set되었다)
    }

}

HANDLE ThreadManager::Spawn(_beginthreadex_proc_type startFunc, LPVOID parameter,uInt32* threadId)
{
    
    HANDLE threadHandle;

    threadHandle = reinterpret_cast<HANDLE>(::_beginthreadex(nullptr, 0, startFunc, parameter, 0, threadId));
    
    m_listThread.push_back(threadHandle);

    return threadHandle;
}

ThreadManager::ThreadManager()
{
}

ThreadManager::~ThreadManager()
{
    while (m_listThread.size() > 0)
        m_listThread.erase(m_listThread.begin());
}

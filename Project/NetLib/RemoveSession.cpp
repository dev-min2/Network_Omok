#include "RemoveSession.h"
#include "SessionManager.h"
#include "Session.h"

RemoveSession* RemoveSession::m_instance = nullptr;

RemoveSession::RemoveSession() 
{
}

RemoveSession::~RemoveSession()
{
}

RemoveSession* RemoveSession::GetInstance()
{
	if (m_instance == nullptr)
		m_instance = new RemoveSession();
	return m_instance;
}

void RemoveSession::ReleaseInstance()
{
	if (m_instance != nullptr)
	{
		SAFE_DELETE(m_instance);
	}
}

void RemoveSession::Run()
{
	while (true)
	{
		::WaitForSingleObject(m_hQuitEvent, m_waitTick);

		auto iter = SessionManager::GetInstance()->GetSessions().GetBegin();
		for (; iter != SessionManager::GetInstance()->GetSessions().GetEnd(); )
		{
			// 연결이 끊겼고 완료 큐에서 I/O를 다 빼왔을 때.
			if (!((*iter)->GetIsConnected()) && (*iter)->IsEndIO() && (*iter)->IsRecv()) 
			{
				iter = SessionManager::GetInstance()->RemoveSession(iter);
				continue;
			}
			++iter;
		}
	}
}

void RemoveSession::InitRemoveSessionThread(DWORD waitTick)
{
	TickThread::BeginThread(waitTick);
}

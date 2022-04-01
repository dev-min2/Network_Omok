#include "SessionManager.h"
#include "Session.h"
#include <thread>

SessionManager* SessionManager::m_instance = nullptr;


SessionManager* SessionManager::GetInstance()
{
	if (m_instance == nullptr)
		m_instance = new SessionManager();
	return m_instance;
}

void SessionManager::ReleaseInstance()
{
	if (m_instance != nullptr)
		SAFE_DELETE(m_instance);
}


// 여러 쓰레드 접근가능.
std::list<Session*>::iterator SessionManager::RemoveSession(std::list<Session*>::iterator& iter)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	{
		Session* session = (*iter);
		session->EventBeforeDelete(); // 삭제하기전 컨텐츠단에서 처리해야 할 작업 처리.
		auto tempIter = iter;
		char* str = ::inet_ntoa(session->m_clientInfo.m_clientAddr.sin_addr);
		ClientInfo& client = session->m_clientInfo;
		session->OnDestroy();
		SAFE_DELETE(session);
		std::list<Session*>::iterator delIter = m_sessions.Erase(tempIter);
		std::cout << "[Server] 클라이언트 " << str << "연결 종료 현재 클라이언트 수 : " << m_sessions.GetSize() << std::endl;

		/*if (m_sessions.size() == 0) // 모든 쓰레드 종료 테스트
		{
			for (int i = 0; i < WorkerThreadCount; ++i)
				::PostQueuedCompletionStatus(CompletionHandler::GetInstance()->GetWorkerIOCPHandle(), 0, 0, 0);
		}*/
		if (m_sessions.GetSize() == 0)
			std::cout << "[Server] 현재 클라이언트 수 0" << std::endl;

		return delIter;
	}
}

SessionManager::SessionManager() 
{
	
}

SessionManager::~SessionManager()
{

	while (true)
	{
		if (m_sessions.GetSize() > 0)
		{
			auto it = (*m_sessions.GetBegin())->m_clientInfo.m_myIter;
			delete *m_sessions.GetBegin();
			m_sessions.Erase(it);
		}
		else
			break;
	}
	
}

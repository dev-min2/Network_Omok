#include "WorkerThread.h"
#include "CompletionHandler.h"
#include "SessionManager.h"
#include "Session.h"
#include "WinError.h"
WorkerThread::WorkerThread() : m_isClose(false)
{

}
WorkerThread::~WorkerThread()
{
	
}
void WorkerThread::Run()
{
	DWORD transferBytes = 0;
	Session* session = nullptr;
	LPOVERLAPPED pOverlapped;
	::ZeroMemory(&pOverlapped, sizeof(LPOVERLAPPED));
	bool ret;
	HANDLE hIocp = CompletionHandler::GetInstance()->GetWorkerIOCPHandle();

	while (true)
	{
		if (::WaitForSingleObject(m_hThreadKillEvent, 1) == WAIT_OBJECT_0)
			break;

		ret = ::GetQueuedCompletionStatus(hIocp, &transferBytes, (ULONG_PTR*)&session, &pOverlapped, INFINITE);

		if (session == nullptr || pOverlapped == nullptr) 
		{
			if (ret && session == nullptr && pOverlapped == nullptr) // 서버 종료.
				Thread::DestroyThread(); // Set signal threadKillEvent
			continue;
		}
		
		// I/O횟수를 줄인다.
		OVERLAPPEDEX* pOverlappedEx = reinterpret_cast<OVERLAPPEDEX*>(pOverlapped);
		
		switch (pOverlappedEx->m_ioType)
		{
		case IO_TYPE::IO_SEND:
			session->m_ioSendCnt.fetch_sub(1);
			break;
		case IO_TYPE::IO_RECV:
			session->m_ioRecvCnt.fetch_sub(1);
			break;
		default:
			continue;
		}

		int errCode = ::WSAGetLastError();
		if (ret || (errCode == GQCS_NO_ERROR && !ret)) // 64번에러는 에러는 아니다.
		{
			if (transferBytes == 0) // 연결종료요청.
			{
				SessionManager::GetInstance()->RemoveSession(session->m_clientInfo.m_myIter);
				continue;
			}
			session->Dispatch(transferBytes, pOverlappedEx);
		}
		else
		{
			std::cout << "[Server] GetQueuedCompletionStatus() - WorkerThread | Run() 에러. 사유 : " << errCode << std::endl;
		}
	}
	

	// 워커쓰레드 빠져나오고 Delete처리.
	std::lock_guard<std::mutex> lock(m_mutex);
	{
		if (!m_isClose)
		{
			CompletionHandler::GetInstance()->GetAcceptorThread()->DestroyThread();
			CompletionHandler::ReleaseInstance();
			SessionManager::ReleaseInstance();
			m_isClose = true;
		}
	}
}

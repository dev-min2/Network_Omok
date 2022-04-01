#include "CompletionHandler.h"

CompletionHandler* CompletionHandler::m_instance = nullptr;

CompletionHandler* CompletionHandler::GetInstance()
{
	if (m_instance == nullptr)
		m_instance = new CompletionHandler();
	return m_instance;
}

void CompletionHandler::ReleaseInstance()
{
	if (m_instance != nullptr)
	{
		SAFE_DELETE(m_instance);
	}
}

bool CompletionHandler::Init(Thread* acceptorThread)
{
	if (!m_isInit)
	{
		m_acceptorThread = acceptorThread;
		//IOCP°´Ã¼ »ý¼º
		m_hWorkerIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, WorkerThreadCount);
		if (m_hWorkerIOCP == INVALID_HANDLE_VALUE)
			return false;

		int32 i;
		for (i = 0; i < WorkerThreadCount; ++i)
		{
			m_workerThread[i] = new WorkerThread();
			m_workerThread[i]->BeginThread();
		}

		m_isInit = true;
	}
	return true;
}

HANDLE CompletionHandler::GetWorkerIOCPHandle()
{
	return m_hWorkerIOCP;
}

bool CompletionHandler::AddHandleToIOCP(HANDLE handle, ULONG_PTR keyValue)
{
	::CreateIoCompletionPort(handle, m_hWorkerIOCP, keyValue, 0);
	return true;
}

CompletionHandler::CompletionHandler() : m_hWorkerIOCP(INVALID_HANDLE_VALUE), m_isInit(false), m_acceptorThread(nullptr)
{
	for (int32 i = 0; i < WorkerThreadCount; ++i)
		m_workerThread[i] = nullptr;
}

CompletionHandler::~CompletionHandler()
{
	for (int32 i = 0; i < WorkerThreadCount; ++i)
	{
		SAFE_DELETE(m_workerThread[i]);
	}
	
}

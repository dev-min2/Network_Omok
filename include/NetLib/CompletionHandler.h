#pragma once

#include "WorkerThread.h"
#include "TickThread.h"
#define WorkerThreadCount 6

class CompletionHandler final
{
private:
	CompletionHandler();
	~CompletionHandler();
public:
	static CompletionHandler* GetInstance();
	static void				  ReleaseInstance();
public:
	bool					  Init(Thread* acceptorThread);
	HANDLE					  GetWorkerIOCPHandle();
	bool					  AddHandleToIOCP(HANDLE handle, ULONG_PTR keyValue);
	Thread*					  GetAcceptorThread() { return m_acceptorThread; }

	static CompletionHandler* m_instance;
	HANDLE					  m_hWorkerIOCP;
	WorkerThread*			  m_workerThread[WorkerThreadCount];
	Thread*					  m_acceptorThread;
	bool					  m_isInit;
};


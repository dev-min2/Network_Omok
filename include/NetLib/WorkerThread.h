#pragma once
#include "Thread.h"
class WorkerThread final : public Thread
{
	friend class CompletionHandler;
	friend class Session;
private:
	WorkerThread();
	~WorkerThread();
public:
	void		Run() override;
private:
	bool		m_isClose;
	std::mutex	m_mutex;
};


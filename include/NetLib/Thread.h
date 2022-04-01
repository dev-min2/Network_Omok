#pragma once
#include "std.h"
class Thread
{
public:
	Thread();
	virtual ~Thread();
public:
	virtual void Run() abstract;
	void		 BeginThread();
	void		 DestroyThread();
	bool		 IsRun() { return m_isStarted; }
protected:
	WSAEVENT	 m_hThreadKillEvent;
	HANDLE		 m_hThread;
private:
	bool		 m_isStarted;
	uInt32		 m_threadID;
private:
	static unsigned int WINAPI ThreadRunner(LPVOID parameter);
};


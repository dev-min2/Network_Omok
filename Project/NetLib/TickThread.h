#pragma once
#include "std.h"
class TickThread
{
public:
	TickThread();
	virtual ~TickThread();
protected:
	virtual void Run() abstract;
	void		 BeginThread(DWORD waitTick);
public:
	void		 DestoryThread();
	bool		 IsRun() { return m_isStarted; }
protected:
	HANDLE		 m_hThread;
	DWORD		 m_waitTick;
	WSAEVENT     m_hQuitEvent;
private:
	bool		 m_isStarted;
	uInt32		 m_threadID;
private:
	static unsigned int WINAPI TickThreadRunner(LPVOID parameter);
};


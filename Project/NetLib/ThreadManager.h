#pragma once
#include "std.h"
class ThreadManager final
{
private:
	ThreadManager();
	~ThreadManager();
	using ListThreadPos = std::list<HANDLE>::iterator;
public:
	static ThreadManager*	GetInstance();
	static void				ReleaseInstance();
public:
	void					Join();	
	HANDLE					Spawn(_beginthreadex_proc_type startFunc,LPVOID parameter, uInt32* threadId);

private:
	static ThreadManager*	m_instance;
	std::list<HANDLE>		m_listThread;
};

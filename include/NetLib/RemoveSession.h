#pragma once
#include "TickThread.h"
#include "SyncQueue.h"
#include <queue>
class Session;
class RemoveSession final : public TickThread // (waitTick / 1000)마다 처리 Session 삭제확인.
{
private:
	RemoveSession();
	~RemoveSession();
public:
	static RemoveSession* GetInstance();
	static void			  ReleaseInstance();
public:
	void				  Run() override;
	void				  InitRemoveSessionThread(DWORD waitTick);
private: 
	static RemoveSession* m_instance;
};


#pragma once
#include "NetWorkClient.h"
#define NETWORK_MSG WM_USER+1
class AsyncIO : public NetWorkClient
{
public:
	static AsyncIO* GetInstance()
	{
		if (m_instance == nullptr)
			m_instance = new AsyncIO;
		return m_instance;
	}
	void	DeleteInstance() { if (m_instance != nullptr) delete m_instance; m_instance = nullptr; }
	bool	Connect();
	void	NetUpdate();
	LRESULT  MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	static AsyncIO* m_instance;
	bool			m_bConnected = false;
};





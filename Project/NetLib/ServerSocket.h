#pragma once
#include "std.h"
class ServerSocket final
{
public:
	ServerSocket();
	~ServerSocket();
public:
	SOCKET		GetListenSocket() { return m_listenSocket; }
public:
	bool		InitServer(const char* ipAddr, uInt32 portNum, uInt32 backlogCnt = SOMAXCONN);
	void		CloseServer();
	ClientInfo	Accept();
private:
	SOCKET		m_listenSocket;
};


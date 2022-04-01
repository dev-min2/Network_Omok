#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <Windows.h>
#include <memory>
#include <map>
#include <vector>
#include <list>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <mutex>
#include <atomic>
#include <thread>
#include <queue>
#include <mstcpip.h>
#include "Types.h"
#include <stack>
#include <algorithm>
#include <functional>
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"mswsock.lib")
#define PACKET_BUFFER_SIZE 2048
#define PACKET_HEADER_SIZE 4
#define MAX_SESSION_NUM 5000
#define SAFE_DELETE(del) if(del) delete del; del = nullptr;
#define SAFE_ARR_DELETE(del) if (del) delete[] del; del = nullptr;
#define GQCS_NO_ERROR 64
class Session;

struct ClientInfo
{
	SOCKET		m_clientSocket;
	SOCKADDR_IN m_clientAddr;
	int			m_addrLen;
	std::list<Session*>::iterator m_myIter;
	ClientInfo() : m_clientSocket(INVALID_SOCKET), m_addrLen(0), m_myIter() { ::ZeroMemory(&m_clientAddr, sizeof(m_clientAddr)); }
};
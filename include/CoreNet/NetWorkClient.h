#pragma once
#include "std.h"
#include "Packet.h"
class NetWorkClient
{
public:
	std::list<Packet*>	m_packetPool; // recv처리되어서 담기는 패킷 풀. 꺼내와서 딜리트 해야한다.
	//std::list<DEFAULT_PACKET*>	m_packetPool; // recv처리되어서 담기는 패킷 풀. 꺼내와서 딜리트 해야한다.
protected:
	NetWorkClient() {}
	virtual ~NetWorkClient() 
	{
		if (m_packetPool.size() > 0)
		{
			auto iter = m_packetPool.begin();
			for (; iter != m_packetPool.end(); ++iter)
				delete* iter;
		}
	}
public:
	bool	InitClientNet();
	bool	CloseClientNet();
	int		SendPacket(Packet& packet);
	int		RecvPacket();
protected:
	bool	m_initCheck = false;
	SOCKET  m_clientSock = INVALID_SOCKET;
private:
	int		RecvProcess(int recvBytes);
private:
	char	m_recvBuffer[PACKET_BUFFER_SIZE] = { 0, };
	int		m_beginPos = 0;
	int		m_endPos = 0;
	int		m_writePos = 0;
	int		m_readPos = 0;
	

};


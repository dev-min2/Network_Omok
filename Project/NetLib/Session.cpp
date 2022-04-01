#include "Session.h"
#include "CompletionHandler.h"
#include "SessionManager.h"

void Session::OnCreate()
{
	if (m_clientInfo.m_clientSocket == INVALID_SOCKET)
		return;

	m_sendBuffer.Create();
	m_receiveBuffer.Create();
	m_isConnect = true;
	WaitForPacketReceive(); // ���� Recv�� �ɾ���� �Ѵ�.
	
}

void Session::OnDestroy()
{
	CloseConnection();
}

void Session::Dispatch(uInt32 transferredBytes, OVERLAPPEDEX* ov)
{
	switch (ov->m_ioType)
	{
	case IO_TYPE::IO_RECV:
		DispatchReceive(transferredBytes, ov);
		break;
	case IO_TYPE::IO_SEND:
		//TODO Send Process
		SendMsgCheck(transferredBytes, ov); // �� ���� �����Ͱ� �ִٸ� ó��.
		break;
	default:
		for(int32 i = 0; i < WorkerThreadCount; ++i)
			::PostQueuedCompletionStatus(CompletionHandler::GetInstance()->GetWorkerIOCPHandle(), 0, NULL, nullptr);
		break;
	}
}


bool Session::SendMsgCheck(uInt32 transferredBytes, OVERLAPPEDEX* ov)
{
	
	if (!m_isConnect)
	{
		std::cout << "[Server] Ŭ���̾�Ʈ ���� ����. Session | SendMsgCheck" << std::endl;
		return false;
	}
	if (m_isSend) //Send�� Session�� ��쿡�� ����.
	{
		if (ov->m_wsaBuf.len == transferredBytes)
		{
			m_sendBuffer.ReleaseBuffer(transferredBytes);
			m_isSend = false;
			::WSASetEvent(m_sendEvent); // Set-Signaled
		}
		else // �� �������.
		{
			SendLessProcess(transferredBytes);
		}
	}
	return true;
}


void Session::CloseConnection()
{
	::shutdown(m_clientInfo.m_clientSocket, SD_BOTH);
	::closesocket(m_clientInfo.m_clientSocket);
	::CloseHandle(m_sendEvent);
	m_clientInfo.m_clientSocket = INVALID_SOCKET;
	m_isConnect = false;
}

void Session::SetSockOptKeepAlive()
{
	// ���� ����(������ ������ ����)Ž��. https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=corinet&logNo=220771742989
	tcp_keepalive tcpk;
	DWORD ret;
	tcpk.onoff = 1; // KeepAlive On.
	tcpk.keepalivetime = 10000; // 10�ʸ��� KeepAlive��ȣ. -> Default�� 2�ð�.
	tcpk.keepaliveinterval = 1000; // KeepAlive��ȣ�� ������ �����̾����� �ٽ� ����.
	::WSAIoctl(m_clientInfo.m_clientSocket, SIO_KEEPALIVE_VALS, &tcpk, sizeof(tcp_keepalive), 0, 0, &ret, nullptr, nullptr);
	
}

bool Session::IsEndIO()
{
	if (m_ioRecvCnt == 0 && m_ioSendCnt == 0) 
		return true;
	return false;
}


Session::Session(ClientInfo& client) : m_clientInfo(client),m_receivedPacketSize(0), m_isConnect(false), m_isSend(false),
 m_sendEvent(::WSACreateEvent()),m_ioSendCnt(0),m_ioRecvCnt(0),m_isRecv(false)
{
	/*
	bool opt = true;
	::setsockopt(m_clientInfo.m_clientSocket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&opt, sizeof(opt));*/

	::WSASetEvent(m_sendEvent); // Set-Signaled. send�� Session�� ���������� Send�� �Ұ���.
	

	::ZeroMemory(&m_overlappedRecv, sizeof(OVERLAPPEDEX));
	::ZeroMemory(&m_overlappedSend, sizeof(OVERLAPPEDEX));
}

Session::~Session()
{
	
}

bool Session::WaitForPacketReceive()
{
	if (!m_isConnect)
		return false;
	if (m_clientInfo.m_clientSocket == INVALID_SOCKET)
		return false;

	DWORD readBytes = 0;
	DWORD flag = 0;
	DWORD bufSize = PACKET_BUFFER_SIZE - m_receivedPacketSize;

	::ZeroMemory(&m_overlappedRecv, sizeof(OVERLAPPEDEX));
	char* m_recvBuffer = m_receiveBuffer.AllocateToBuffer(PACKET_BUFFER_SIZE); // �ϳͿ� ���ŵ� �� �ִ� ����ũ��(2048)
	m_overlappedRecv.m_wsaBuf.buf = m_recvBuffer; // ������ receiveBuffer.
	m_overlappedRecv.m_wsaBuf.len = PACKET_BUFFER_SIZE;
	m_overlappedRecv.m_ioType	  = IO_TYPE::IO_RECV;

	int32 ret = ::WSARecv(m_clientInfo.m_clientSocket, 
		&m_overlappedRecv.m_wsaBuf, 1, &readBytes, &flag,
		(LPOVERLAPPED)&m_overlappedRecv.m_overlapped, nullptr);


	if (ret == SOCKET_ERROR && ::WSAGetLastError() != ERROR_IO_PENDING)
	{
		int errorCode = ::WSAGetLastError();
		std::cout << "WSARecv ���� | Session | WaitForPacketReceive()" << errorCode << std::endl;
		if (errorCode == 10054)
		{
			std::cout << "[Server] Ŭ���̾�Ʈ������ ������ ����" << std::endl;
		}
		// ������ ���´�.
		m_isConnect = false;
		return false;
	}
	m_ioRecvCnt.fetch_add(1);
	return true;
}

void Session::BroadCast(Packet& packet) // ��ο��� �Ѹ���.
{
	
	auto it = SessionManager::GetInstance()->GetSessions().GetBegin();
	for (; it != SessionManager::GetInstance()->GetSessions().GetEnd(); ++it)
	{
		Session* session = *it;
		SendPacketToOtherUser(packet, session);
	}

	std::cout << "[Server] BroadCast(" << SessionManager::GetInstance()->GetSessions().GetSize() << ")! " << packet.GetPacketLen() << " ����Ʈ �۽�.." << std::endl;
	
}

void Session::SendPacket(Packet& packet)
{
	if (!m_isConnect)
		return;
	::WaitForSingleObject(m_sendEvent, INFINITE); 
	::WSAResetEvent(this->m_sendEvent);
	m_isSend = true;
	
	uInt32 packetLen = packet.GetDefaultPacket().m_ph.m_len;
	DWORD sendBytes = 0, flag = 0;

	char* sendBuffer = m_sendBuffer.AllocateToBuffer(packetLen); // SendMsgCheck���� �����ʿ�.
	::CopyMemory(sendBuffer, (char*)&packet.GetDefaultPacket(), packetLen);

	m_overlappedSend.m_wsaBuf.buf = sendBuffer;
	m_overlappedSend.m_wsaBuf.len = packetLen;
	m_overlappedSend.m_ioType = IO_TYPE::IO_SEND;
	int ret = ::WSASend(m_clientInfo.m_clientSocket, &m_overlappedSend.m_wsaBuf, 1, &sendBytes, flag, &m_overlappedSend.m_overlapped, nullptr);
	if (ret == SOCKET_ERROR && ::WSAGetLastError() != ERROR_IO_PENDING)
	{
		int errorCode = ::WSAGetLastError();
		std::cout << "[Server] WSASend ���� | User | SendPacket() ���� : " << errorCode << std::endl;
		if (errorCode == 10054)
		{
			std::cout << "[Server] Ŭ���̾�Ʈ������ ������ ����" << std::endl;
		}

		// ������ ���´�. 
		this->SetIsConnect(false);
		return;
	}
	char* address = ::inet_ntoa(m_clientInfo.m_clientAddr.sin_addr);
	std::cout << "[Server] Ŭ���̾�Ʈ " << address << "���� " << packetLen << " ����Ʈ �۽�.." << std::endl;
	this->m_ioSendCnt.fetch_add(1);
}

void Session::SendPacketToOtherUser(Packet& packet, Session* session)
{
	if (!session->m_isConnect)
		return;
	::WaitForSingleObject(session->m_sendEvent, INFINITE); 
	::WSAResetEvent(session->m_sendEvent);
	session->m_isSend = true;

	uInt32 packetLen = packet.GetDefaultPacket().m_ph.m_len;
	DWORD sendBytes = 0, flag = 0;

	//    SendMsgCheck���� �����ʿ�.
	char* sendBuffer = session->m_sendBuffer.AllocateToBuffer(packetLen); 
	::CopyMemory(sendBuffer, (char*)&packet.GetDefaultPacket(), packetLen);

	session->m_overlappedSend.m_wsaBuf.buf	= sendBuffer;
	session->m_overlappedSend.m_wsaBuf.len	= packetLen;
	session->m_overlappedSend.m_ioType		= IO_TYPE::IO_SEND;
	int ret = ::WSASend(session->m_clientInfo.m_clientSocket, &session->m_overlappedSend.m_wsaBuf, 1, &sendBytes, flag, &session->m_overlappedSend.m_overlapped, nullptr);
	if (ret == SOCKET_ERROR && ::WSAGetLastError() != ERROR_IO_PENDING)
	{
		int errorCode = ::WSAGetLastError();
		std::cout << "[Server] WSASend ���� | User | SendPacketToOtherUser() ���� : " << errorCode << std::endl;
		if (errorCode == 10054)
		{
			std::cout << "[Server] Ŭ���̾�Ʈ������ ������ ����" << std::endl;
		}
		// ������ ���´�. 
		session->SetIsConnect(false);
		return;	
	}

	char* address = ::inet_ntoa(session->m_clientInfo.m_clientAddr.sin_addr);
	std::cout << "[Server] Ŭ���̾�Ʈ " << address << "���� " << packetLen << " ����Ʈ �۽�.." << std::endl;
	session->m_ioSendCnt.fetch_add(1);
}

void Session::SendLessProcess(uInt32 transferredBytes)
{
	if (!m_isConnect)
		return;
	//::WSAResetEvent(m_sendLessEvent); // Set Non-Signaled.

	m_isSend = true;

	DWORD sendBytes = 0;
	DWORD flags = 0;

	
	// �� ����������.
	char* pLessPtr = m_sendBuffer.ReleaseBuffer(transferredBytes);//&m_overlappedSend.m_wsaBuf.buf[transferredBytes];
	m_overlappedSend.m_wsaBuf.buf = pLessPtr;
	m_overlappedSend.m_wsaBuf.len = (m_overlappedSend.m_wsaBuf.len - transferredBytes);
	m_overlappedSend.m_ioType     = IO_TYPE::IO_SEND;

	int32 ret = ::WSASend(m_clientInfo.m_clientSocket, &m_overlappedSend.m_wsaBuf, 1, &sendBytes, flags, &m_overlappedSend.m_overlapped, nullptr);
	if (ret == SOCKET_ERROR && ::WSAGetLastError() != ERROR_IO_PENDING)
	{
		int errorCode = ::WSAGetLastError();
		std::cout << "[Server] WSASend ���� | User | SendLessProcess() ���� : " << errorCode << std::endl;
		if (errorCode == 10054)
		{
			std::cout << "[Server] Ŭ���̾�Ʈ������ ������ ����" << std::endl;
		}
		// ������ ���´�. 
		this->SetIsConnect(false);
		return;
	}
	std::cout << "[Server] Ŭ���̾�Ʈ " << m_clientInfo.m_clientSocket << "���� " << " �� �۽ŵ� ����Ʈ " << m_overlappedSend.m_wsaBuf.len << "�۽�.." << std::endl;
	m_ioSendCnt.fetch_add(1);
}

// ��Ŷó�� �κ�.
void Session::DispatchReceive(uInt32 transferredBytes, OVERLAPPEDEX* ov)
{
	if (!m_isConnect)
		return;
	m_isRecv.store(true);
	Packet packet;
	
	// ���ŵ� ũ�� ���.
 	m_receivedPacketSize += transferredBytes;
	packet.m_receivedSize += transferredBytes;

	// ������ ���� ���ŵ� ����Ʈ�� ���ٸ�. �ٷ� ������ ������ packet���� ����.
	// ���ŵ� ��� ��Ŷ�� ��� ó��
	packet.CopyToPacketBuffer(m_receiveBuffer.GetCurrentReadPos(), m_receivedPacketSize);

	while (m_receivedPacketSize > 0) // 0�� �ɶ����� ó��.
	{
		// ����ε� �ϳ��� ��Ŷ���� üũ.
		if (packet.IsValidPacketHeader() && m_receivedPacketSize >= packet.GetPacketLen())
		{
			uInt16 len = packet.GetPacketLen(); // len���� �ڸ� ���� packet.GetPacketLen.
			std::cout << "[Server] Ŭ���̾�Ʈ�� ���� " << len << " ����Ʈ ����.." << std::endl;
			PacketParsing(packet); // ����� �����ߴٸ� �Ľ�. 
			m_receivedPacketSize -= len;
			char* nextBuffer = m_receiveBuffer.ReleaseBuffer(len); // ���� ��Ŷ.
			packet.m_receivedSize = m_receivedPacketSize;
			if (m_receivedPacketSize > 0) // ���� ���� ����Ʈ�� ���Ҵٸ�. �̾ ó��.
			{
				packet.CopyToPacketBuffer(nextBuffer, m_receivedPacketSize);
			}
		}
		else
		{
			break;
		}
	}
	if(m_receivedPacketSize <= 0) // ��� ó�� �Ǿ���.
		m_receiveBuffer.ReleaseBuffer(m_receiveBuffer.GetAllocatedBytes());
	else
	{
		// ���� �� ó���� ��Ŷ�� �����ִ�.(�� ����)
		char* pRemain = m_receiveBuffer.ReleaseBuffer(m_receivedPacketSize); // 20
		m_receiveBuffer.ReleaseBuffer(m_receiveBuffer.GetAllocatedBytes() - m_receivedPacketSize);
		char* pWrite = m_receiveBuffer.AllocateToBuffer(m_receivedPacketSize);
		::CopyMemory(pWrite, pRemain, m_receivedPacketSize);
	}
	if (m_isConnect)
	{
		WaitForPacketReceive(); // ����ؼ� Recv ��û
		m_isRecv.store(false);
	}
}

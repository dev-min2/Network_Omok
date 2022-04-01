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
	WaitForPacketReceive(); // 먼저 Recv를 걸어줘야 한다.
	
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
		SendMsgCheck(transferredBytes, ov); // 덜 보낸 데이터가 있다면 처리.
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
		std::cout << "[Server] 클라이언트 연결 끊김. Session | SendMsgCheck" << std::endl;
		return false;
	}
	if (m_isSend) //Send한 Session의 경우에만 해제.
	{
		if (ov->m_wsaBuf.len == transferredBytes)
		{
			m_sendBuffer.ReleaseBuffer(transferredBytes);
			m_isSend = false;
			::WSASetEvent(m_sendEvent); // Set-Signaled
		}
		else // 덜 보낸경우.
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
	// 유령 세션(연결이 끊어진 세션)탐지. https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=corinet&logNo=220771742989
	tcp_keepalive tcpk;
	DWORD ret;
	tcpk.onoff = 1; // KeepAlive On.
	tcpk.keepalivetime = 10000; // 10초마다 KeepAlive신호. -> Default는 2시간.
	tcpk.keepaliveinterval = 1000; // KeepAlive신호를 보내고 답장이없으면 다시 전송.
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

	::WSASetEvent(m_sendEvent); // Set-Signaled. send한 Session은 연속적으로 Send를 불가능.
	

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
	char* m_recvBuffer = m_receiveBuffer.AllocateToBuffer(PACKET_BUFFER_SIZE); // 하넌에 수신될 수 있는 버퍼크기(2048)
	m_overlappedRecv.m_wsaBuf.buf = m_recvBuffer; // 세션의 receiveBuffer.
	m_overlappedRecv.m_wsaBuf.len = PACKET_BUFFER_SIZE;
	m_overlappedRecv.m_ioType	  = IO_TYPE::IO_RECV;

	int32 ret = ::WSARecv(m_clientInfo.m_clientSocket, 
		&m_overlappedRecv.m_wsaBuf, 1, &readBytes, &flag,
		(LPOVERLAPPED)&m_overlappedRecv.m_overlapped, nullptr);


	if (ret == SOCKET_ERROR && ::WSAGetLastError() != ERROR_IO_PENDING)
	{
		int errorCode = ::WSAGetLastError();
		std::cout << "WSARecv 실패 | Session | WaitForPacketReceive()" << errorCode << std::endl;
		if (errorCode == 10054)
		{
			std::cout << "[Server] 클라이언트측에서 연결을 끊음" << std::endl;
		}
		// 연결을 끊는다.
		m_isConnect = false;
		return false;
	}
	m_ioRecvCnt.fetch_add(1);
	return true;
}

void Session::BroadCast(Packet& packet) // 모두에게 뿌린다.
{
	
	auto it = SessionManager::GetInstance()->GetSessions().GetBegin();
	for (; it != SessionManager::GetInstance()->GetSessions().GetEnd(); ++it)
	{
		Session* session = *it;
		SendPacketToOtherUser(packet, session);
	}

	std::cout << "[Server] BroadCast(" << SessionManager::GetInstance()->GetSessions().GetSize() << ")! " << packet.GetPacketLen() << " 바이트 송신.." << std::endl;
	
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

	char* sendBuffer = m_sendBuffer.AllocateToBuffer(packetLen); // SendMsgCheck에서 해제필요.
	::CopyMemory(sendBuffer, (char*)&packet.GetDefaultPacket(), packetLen);

	m_overlappedSend.m_wsaBuf.buf = sendBuffer;
	m_overlappedSend.m_wsaBuf.len = packetLen;
	m_overlappedSend.m_ioType = IO_TYPE::IO_SEND;
	int ret = ::WSASend(m_clientInfo.m_clientSocket, &m_overlappedSend.m_wsaBuf, 1, &sendBytes, flag, &m_overlappedSend.m_overlapped, nullptr);
	if (ret == SOCKET_ERROR && ::WSAGetLastError() != ERROR_IO_PENDING)
	{
		int errorCode = ::WSAGetLastError();
		std::cout << "[Server] WSASend 실패 | User | SendPacket() 사유 : " << errorCode << std::endl;
		if (errorCode == 10054)
		{
			std::cout << "[Server] 클라이언트측에서 연결을 끊음" << std::endl;
		}

		// 연결을 끊는다. 
		this->SetIsConnect(false);
		return;
	}
	char* address = ::inet_ntoa(m_clientInfo.m_clientAddr.sin_addr);
	std::cout << "[Server] 클라이언트 " << address << "에게 " << packetLen << " 바이트 송신.." << std::endl;
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

	//    SendMsgCheck에서 해제필요.
	char* sendBuffer = session->m_sendBuffer.AllocateToBuffer(packetLen); 
	::CopyMemory(sendBuffer, (char*)&packet.GetDefaultPacket(), packetLen);

	session->m_overlappedSend.m_wsaBuf.buf	= sendBuffer;
	session->m_overlappedSend.m_wsaBuf.len	= packetLen;
	session->m_overlappedSend.m_ioType		= IO_TYPE::IO_SEND;
	int ret = ::WSASend(session->m_clientInfo.m_clientSocket, &session->m_overlappedSend.m_wsaBuf, 1, &sendBytes, flag, &session->m_overlappedSend.m_overlapped, nullptr);
	if (ret == SOCKET_ERROR && ::WSAGetLastError() != ERROR_IO_PENDING)
	{
		int errorCode = ::WSAGetLastError();
		std::cout << "[Server] WSASend 실패 | User | SendPacketToOtherUser() 사유 : " << errorCode << std::endl;
		if (errorCode == 10054)
		{
			std::cout << "[Server] 클라이언트측에서 연결을 끊음" << std::endl;
		}
		// 연결을 끊는다. 
		session->SetIsConnect(false);
		return;	
	}

	char* address = ::inet_ntoa(session->m_clientInfo.m_clientAddr.sin_addr);
	std::cout << "[Server] 클라이언트 " << address << "에게 " << packetLen << " 바이트 송신.." << std::endl;
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

	
	// 덜 보낸데이터.
	char* pLessPtr = m_sendBuffer.ReleaseBuffer(transferredBytes);//&m_overlappedSend.m_wsaBuf.buf[transferredBytes];
	m_overlappedSend.m_wsaBuf.buf = pLessPtr;
	m_overlappedSend.m_wsaBuf.len = (m_overlappedSend.m_wsaBuf.len - transferredBytes);
	m_overlappedSend.m_ioType     = IO_TYPE::IO_SEND;

	int32 ret = ::WSASend(m_clientInfo.m_clientSocket, &m_overlappedSend.m_wsaBuf, 1, &sendBytes, flags, &m_overlappedSend.m_overlapped, nullptr);
	if (ret == SOCKET_ERROR && ::WSAGetLastError() != ERROR_IO_PENDING)
	{
		int errorCode = ::WSAGetLastError();
		std::cout << "[Server] WSASend 실패 | User | SendLessProcess() 사유 : " << errorCode << std::endl;
		if (errorCode == 10054)
		{
			std::cout << "[Server] 클라이언트측에서 연결을 끊음" << std::endl;
		}
		// 연결을 끊는다. 
		this->SetIsConnect(false);
		return;
	}
	std::cout << "[Server] 클라이언트 " << m_clientInfo.m_clientSocket << "에게 " << " 덜 송신된 바이트 " << m_overlappedSend.m_wsaBuf.len << "송신.." << std::endl;
	m_ioSendCnt.fetch_add(1);
}

// 패킷처리 부분.
void Session::DispatchReceive(uInt32 transferredBytes, OVERLAPPEDEX* ov)
{
	if (!m_isConnect)
		return;
	m_isRecv.store(true);
	Packet packet;
	
	// 수신된 크기 계산.
 	m_receivedPacketSize += transferredBytes;
	packet.m_receivedSize += transferredBytes;

	// 이전에 남은 수신된 바이트가 없다면. 바로 버퍼의 내용을 packet으로 복사.
	// 수신된 모든 패킷을 모두 처리
	packet.CopyToPacketBuffer(m_receiveBuffer.GetCurrentReadPos(), m_receivedPacketSize);

	while (m_receivedPacketSize > 0) // 0이 될때까지 처리.
	{
		// 제대로된 하나의 패킷인지 체크.
		if (packet.IsValidPacketHeader() && m_receivedPacketSize >= packet.GetPacketLen())
		{
			uInt16 len = packet.GetPacketLen(); // len들어가는 자리 원래 packet.GetPacketLen.
			std::cout << "[Server] 클라이언트로 부터 " << len << " 바이트 수신.." << std::endl;
			PacketParsing(packet); // 제대로 수신했다면 파싱. 
			m_receivedPacketSize -= len;
			char* nextBuffer = m_receiveBuffer.ReleaseBuffer(len); // 다음 패킷.
			packet.m_receivedSize = m_receivedPacketSize;
			if (m_receivedPacketSize > 0) // 아직 남은 바이트가 남았다면. 이어서 처리.
			{
				packet.CopyToPacketBuffer(nextBuffer, m_receivedPacketSize);
			}
		}
		else
		{
			break;
		}
	}
	if(m_receivedPacketSize <= 0) // 모두 처리 되었음.
		m_receiveBuffer.ReleaseBuffer(m_receiveBuffer.GetAllocatedBytes());
	else
	{
		// 아직 덜 처리된 패킷이 남아있다.(덜 수신)
		char* pRemain = m_receiveBuffer.ReleaseBuffer(m_receivedPacketSize); // 20
		m_receiveBuffer.ReleaseBuffer(m_receiveBuffer.GetAllocatedBytes() - m_receivedPacketSize);
		char* pWrite = m_receiveBuffer.AllocateToBuffer(m_receivedPacketSize);
		::CopyMemory(pWrite, pRemain, m_receivedPacketSize);
	}
	if (m_isConnect)
	{
		WaitForPacketReceive(); // 계속해서 Recv 요청
		m_isRecv.store(false);
	}
}

#pragma once
#include "std.h"
#include "Packet.h"
#include "RingBuffer.h"
// 이 클래스의 객체는 클라이언트와 통신.

enum class IO_TYPE
{
	IO_SEND,
	IO_RECV,
};
struct OVERLAPPEDEX
{
	WSAOVERLAPPED	m_overlapped;
	WSABUF			m_wsaBuf;
	IO_TYPE			m_ioType;
};

class Session
{	
	friend class SessionManager;
	friend class WorkerThread;
public:
	//Session 클래스의 사용이 시작/종료되는 시점
	void	OnCreate();
	void	OnDestroy();

	void	Dispatch(uInt32 transferredBytes, OVERLAPPEDEX* ov); // 분석 (IO_TYPE)
	
	void	CloseConnection();

	SOCKET	GetSocketValue() { return m_clientInfo.m_clientSocket; }
	bool	GetIsConnected() { return m_isConnect; }
	void	SetIsConnect(bool value) { m_isConnect.exchange(value); }
	void	SetSockOptKeepAlive();
	bool	IsEndIO();
	bool	IsSend() { return m_isSend; }
	bool	IsRecv() { return m_isRecv; }
public:
	std::atomic<uInt32>& GetIoSendCnt() { return m_ioSendCnt; }
protected:
	//Session Manager를 제외한 다른 외부에서 임의로 생성 불가능
	Session() = delete;
	Session(ClientInfo& client);
	virtual ~Session();

public:						  
	ClientInfo				  m_clientInfo;
	// 수신된 패킷 처리
protected:

	// ---------- RECV -------------
	virtual void			  PacketParsing(Packet& packet) abstract; // 패킷 해석 자식클래스에서 재정의.
	virtual void			  EventBeforeDelete() abstract; // 연결종료할 때 컨텐츠단에서 처리하는 로직.
	bool					  WaitForPacketReceive(); // 패킷 대기

	// ----------SEND---------------
	void					  BroadCast(Packet& packet);
	void					  SendPacket(Packet& packet);
	void					  SendPacketToOtherUser(Packet& packet, Session* session);
	void					  SendLessProcess(uInt32 transferredBytes);

	// ----------DISPATCH-----------
	bool					  SendMsgCheck(uInt32 transferredBytes, OVERLAPPEDEX* ov);
	void					  DispatchReceive(uInt32 transferredBytes, OVERLAPPEDEX* ov); 
private:
	void					  ClearWSABUF();
protected:
	OVERLAPPEDEX			  m_overlappedSend;
	OVERLAPPEDEX			  m_overlappedRecv;

	RingBuffer				  m_receiveBuffer;
	RingBuffer				  m_sendBuffer;

	uInt32					  m_receivedPacketSize;
	std::atomic<bool>		  m_isConnect;
	std::mutex				  m_mutex;
protected:
	std::atomic<uInt32>		  m_ioRecvCnt;
	std::atomic<uInt32>		  m_ioSendCnt;
protected:
	std::atomic<bool>		  m_isSend;
	std::atomic<bool>		  m_isRecv;
	WSAEVENT				  m_sendEvent;
};


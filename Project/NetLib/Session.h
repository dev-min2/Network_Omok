#pragma once
#include "std.h"
#include "Packet.h"
#include "RingBuffer.h"
// �� Ŭ������ ��ü�� Ŭ���̾�Ʈ�� ���.

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
	//Session Ŭ������ ����� ����/����Ǵ� ����
	void	OnCreate();
	void	OnDestroy();

	void	Dispatch(uInt32 transferredBytes, OVERLAPPEDEX* ov); // �м� (IO_TYPE)
	
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
	//Session Manager�� ������ �ٸ� �ܺο��� ���Ƿ� ���� �Ұ���
	Session() = delete;
	Session(ClientInfo& client);
	virtual ~Session();

public:						  
	ClientInfo				  m_clientInfo;
	// ���ŵ� ��Ŷ ó��
protected:

	// ---------- RECV -------------
	virtual void			  PacketParsing(Packet& packet) abstract; // ��Ŷ �ؼ� �ڽ�Ŭ�������� ������.
	virtual void			  EventBeforeDelete() abstract; // ���������� �� �������ܿ��� ó���ϴ� ����.
	bool					  WaitForPacketReceive(); // ��Ŷ ���

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


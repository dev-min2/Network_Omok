#pragma once
#include "std.h"
#include "CompletionHandler.h"
#include "SyncList.h"
class Session;

class SessionManager final
{
private:
	SessionManager();
	~SessionManager();
public:
	static SessionManager*			GetInstance();
	static void						ReleaseInstance();
public:
	template <typename T>
	void							CreateNewSession(ClientInfo& client);
public:
	std::list<Session*>::iterator	RemoveSession(std::list<Session*>::iterator& iter);
	SyncList<Session*>&				GetSessions() { return m_sessions; }
private:
	static SessionManager*			m_instance;
	SyncList<Session*>				m_sessions;
	std::mutex						m_mutex;
};

template <typename T>
void SessionManager::CreateNewSession(ClientInfo& client) // Acceptor쓰레드에서만 접근.
{
	T* user = new T(client); // 유저생성.
	user->SetSockOptKeepAlive();
	//m_sessions.push_front(user); // 유저 삽입.
	m_sessions.PushFront(user);
	user->m_clientInfo.m_myIter = m_sessions.GetBegin();//m_sessions.begin();
	char* str = ::inet_ntoa(client.m_clientAddr.sin_addr);
	//iocp등록
	CompletionHandler::GetInstance()->AddHandleToIOCP((HANDLE)client.m_clientSocket, (ULONG_PTR)user);
	std::cout << "[Server] 클라이언트 연결완료. : " << str << " 현재 클라이언트 수 : " << m_sessions.GetSize() << std::endl;
	user->OnCreate(); // recv.
}

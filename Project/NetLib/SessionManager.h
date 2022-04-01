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
void SessionManager::CreateNewSession(ClientInfo& client) // Acceptor�����忡���� ����.
{
	T* user = new T(client); // ��������.
	user->SetSockOptKeepAlive();
	//m_sessions.push_front(user); // ���� ����.
	m_sessions.PushFront(user);
	user->m_clientInfo.m_myIter = m_sessions.GetBegin();//m_sessions.begin();
	char* str = ::inet_ntoa(client.m_clientAddr.sin_addr);
	//iocp���
	CompletionHandler::GetInstance()->AddHandleToIOCP((HANDLE)client.m_clientSocket, (ULONG_PTR)user);
	std::cout << "[Server] Ŭ���̾�Ʈ ����Ϸ�. : " << str << " ���� Ŭ���̾�Ʈ �� : " << m_sessions.GetSize() << std::endl;
	user->OnCreate(); // recv.
}

#pragma once
#include "Thread.h"
#include "ServerSocket.h"
#include "SessionManager.h"

template<typename T> // 여기서 타입을 넘겨준다.
class Acceptor final : public Thread
{
public:
	Acceptor();
	~Acceptor();
public:
	
	void Run() override; // 가상 템플릿 멤버함수는 안된다.
	bool Init(const char* ipAddr, uInt32 portNum);

private:
	ServerSocket	m_server;	
};
template<typename T>
Acceptor<T>::Acceptor() 
{
}
template<typename T>
Acceptor<T>::~Acceptor()
{
    m_server.CloseServer(); 
}

template<typename T> // 여기서 타입을 넘겨준다.
void Acceptor<T>::Run()
{
    while (true)
    {
        ClientInfo client;
        // 종료 이벤트가 signaled상태가 되면 빠져 나온다.
        if (::WaitForSingleObject(m_hThreadKillEvent, 1) == WAIT_OBJECT_0)
            break;

        client = m_server.Accept();
        if (client.m_clientSocket != INVALID_SOCKET)
            SessionManager::GetInstance()->CreateNewSession<T>(client);
    }
}
template<typename T>
bool Acceptor<T>::Init(const char* ipAddr, uInt32 portNum)
{
    if (m_server.InitServer(ipAddr, portNum) == false)
        return false;

    Thread::BeginThread();
    return true;
}

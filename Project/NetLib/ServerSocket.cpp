#include "ServerSocket.h"
ServerSocket::ServerSocket() : m_listenSocket(INVALID_SOCKET)
{
}

ServerSocket::~ServerSocket()
{
    if (m_listenSocket != INVALID_SOCKET)
    {
        ::shutdown(m_listenSocket, SD_BOTH);
        ::closesocket(m_listenSocket);
    }
    ::WSACleanup();
}

bool ServerSocket::InitServer(const char* ipAddr, uInt32 portNum, uInt32 backlogCnt)
{
    WSADATA wsaData;

    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return false;

    SOCKADDR_IN serverAddr;
    int32 reuse = 1;
    ::ZeroMemory(&serverAddr, sizeof(SOCKADDR));
    serverAddr.sin_family = AF_INET;
    if (::strcmp(ipAddr, "127.0.0.1") == 0)
        serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
    else
        serverAddr.sin_addr.s_addr = ::inet_addr(ipAddr);
    serverAddr.sin_port = ::htons(portNum);

    m_listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_listenSocket == INVALID_SOCKET)
    {
        ::WSACleanup();
        return false;
    }
    // 서버 포트 재사용.
    ::setsockopt(m_listenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));

    if (::bind(m_listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        ::WSACleanup();
        return false;
    }

    if (::listen(m_listenSocket, backlogCnt) == SOCKET_ERROR)
    {
        ::WSACleanup();
        return false;
    }
    std::cout << "[Server] Server 시작" << std::endl;
    return true;
}

void ServerSocket::CloseServer()
{
    if (m_listenSocket != INVALID_SOCKET)
    {
        ::shutdown(m_listenSocket, SD_BOTH);
        ::closesocket(m_listenSocket);
    }
    ::WSACleanup();
}

ClientInfo ServerSocket::Accept() // Acceptor에 할당된 쓰레드(1개)가 처리하는 곳.
{
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(m_listenSocket, &fd);

    TIMEVAL time;
    time.tv_usec = 1000;
    time.tv_sec = 1;

    ClientInfo client;
    SOCKADDR_IN addr;
    ::ZeroMemory(&addr, sizeof(addr));
    int32 len = sizeof(SOCKADDR_IN);
    if (::select(0, &fd, nullptr, nullptr, &time) == SOCKET_ERROR)
        return client;

    if (FD_ISSET(m_listenSocket, &fd))
    {
        client.m_clientSocket = ::accept(m_listenSocket, (SOCKADDR*)&addr, &len);
        client.m_clientAddr = addr;
        return client;
    }        
    return client;
}

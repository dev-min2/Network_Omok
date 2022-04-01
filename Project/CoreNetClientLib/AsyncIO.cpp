#include "AsyncIO.h"
#include <string>
AsyncIO* AsyncIO::m_instance = nullptr;
bool AsyncIO::Connect() 
{
    SOCKADDR_IN serverAddr;
    ::ZeroMemory(&serverAddr, sizeof(SOCKADDR_IN));
    if(::WSAAsyncSelect(m_clientSock, g_WindowHandle, NETWORK_MSG,
        FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
    {
        ::MessageBoxA(g_WindowHandle, "AsyncSelect 실패", "경고", 0);
        return false;
    }

    serverAddr.sin_family      = AF_INET;
    //serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1"); // 192.168.0.12
    serverAddr.sin_addr.s_addr = ::inet_addr("192.168.0.107"); // 192.168.0.107
    serverAddr.sin_port        = ::htons(9199); // 10000

    //serverAddr.sin_addr.s_addr = ::inet_addr(SERVER_IP);
    //serverAddr.sin_port = ::htons(9123);

    //int ret = ::WSAConnect(m_clientSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr), nullptr, nullptr, nullptr, nullptr);
    int ret = ::connect(m_clientSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    if (ret == SOCKET_ERROR)
    {
        int errorCode = ::WSAGetLastError();
        if (errorCode == WSAEWOULDBLOCK)
        {
            while (true)
            {
                if (::WSAGetLastError() == WSAEISCONN)
                    break;
                ::connect(m_clientSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
            }
        }
        else
        {
            std::string s1 = "Connect 실패 사유 : ";
            s1 += std::to_string(errorCode);
            ::MessageBoxA(g_WindowHandle, s1.c_str(), "경고", 0);
            return false;
        }
        
    }
    //::MessageBoxA(g_WindowHandle, "Connect 성공!!", "공지", 0);
    return true;
}

void AsyncIO::NetUpdate()
{
}

LRESULT AsyncIO::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) // 네트워크 메시지 처리. 여기서 굳이 안해도 되긴함
    {
    case NETWORK_MSG:
    {
        WORD wRet = WSAGETSELECTEVENT(lParam);
        switch (wRet)
        {
        case FD_CONNECT :
        {
            m_bConnected = true;
        }break;
        case FD_CLOSE :
        {
            m_bConnected = false;
        }break;
        case FD_READ :
        {
            // Recv처리
            NetWorkClient::RecvPacket();
        }break;
        case FD_WRITE:
        {

        }break;
        default:
            break;
        }
    }
    }


    return 0;
}

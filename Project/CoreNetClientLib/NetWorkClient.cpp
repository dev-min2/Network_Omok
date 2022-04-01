#include "NetWorkClient.h"
#include "Protocol.h"
#include <iostream>
bool NetWorkClient::InitClientNet()
{
    if (m_initCheck)
        return false;
    WSADATA wsa;
    if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        return false;
    }

    m_clientSock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_clientSock == INVALID_SOCKET)
    {
        return false;
    }

    m_initCheck = true;
    return true;
}

bool NetWorkClient::CloseClientNet()
{
    ::shutdown(m_clientSock, SD_BOTH);
    ::closesocket(m_clientSock);
    ::WSACleanup();
    return false;
}

int NetWorkClient::SendPacket(Packet& packet)
{
    char* pMsg = (char*)&packet.GetDefaultPacket();
    int sendSize = 0;
    do
    {
        int recvBytes = ::send(m_clientSock, &pMsg[sendSize], packet.GetPacketLen() - sendSize, 0);
        if (recvBytes == SOCKET_ERROR)
        {
            ::MessageBoxA(g_WindowHandle, "Ŭ���̾�Ʈ �۽� ����", "���", 0);
            return -1;
        }
        sendSize += recvBytes;
    } while (sendSize < packet.GetPacketLen());

    return sendSize;
}

int NetWorkClient::RecvPacket()
{
    int recvBytes = ::recv(m_clientSock, m_recvBuffer, PACKET_HEADER_SIZE, 0);
    if (recvBytes == 0) // ��������
    {
        ::MessageBoxA(g_WindowHandle, "������ ������ ����.", "����", 0);
        ::closesocket(m_clientSock);
        return 0;
    }
    if (recvBytes == SOCKET_ERROR)
    {
        //::MessageBoxA(g_WindowHandle, "Ŭ���̾�Ʈ ���� ����", "���", 0);
        return -1;
    }
    recvBytes = RecvProcess(recvBytes);

    return recvBytes;
}

int NetWorkClient::RecvProcess(int recvBytes)
{
    int recvSize = recvBytes;
    if (recvBytes < PACKET_HEADER_SIZE)
    {
        do
        {
            int bytes = ::recv(m_clientSock, m_recvBuffer, PACKET_HEADER_SIZE - recvSize, 0);
            if (bytes == SOCKET_ERROR)
            {
                int error = ::WSAGetLastError();
                std::cout << error << std::endl;
            }
            recvSize += bytes;
        } while (recvSize < PACKET_HEADER_SIZE);
    }
    m_writePos += recvSize; // PACKET_HEADER_SIZE

    // ��Ŷ ���ũ�� �޾ƿ�.
    DEFAULT_PACKET packet;
    ::CopyMemory(&packet.m_ph, &m_recvBuffer[m_readPos], m_writePos);
    m_readPos += m_writePos;
    if (m_readPos == m_writePos)
    {
        m_readPos = 0;
        m_writePos = 0;
    }
    recvSize = 0;
    do
    {

        if ((packet.m_ph.m_len - PACKET_HEADER_SIZE) == 0)
            break;
        int bytes = ::recv(m_clientSock, packet.m_dataField, packet.m_ph.m_len - PACKET_HEADER_SIZE - recvSize, 0);
        recvSize += bytes;
    } while (recvSize < packet.m_ph.m_len - PACKET_HEADER_SIZE);

    // ��Ŷ�ؼ� �Ϸ�.
    Packet* p1 = new Packet();
    ::CopyMemory(&p1->GetDefaultPacket(), &packet, packet.m_ph.m_len);
    m_packetPool.push_back(p1);

    return recvSize + PACKET_HEADER_SIZE;
}


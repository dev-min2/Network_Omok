#include "Packet.h"

#include "Packet.h"

Packet::Packet() : m_pReadPos(&m_packet.m_dataField[0]), m_pWritePos(&m_packet.m_dataField[0]), m_pBeginPos(&m_packet.m_dataField[0]),
m_pEndPos(&m_packet.m_dataField[PACKET_BUFFER_SIZE - 1]), m_receivedSize(0)
{
    ::ZeroMemory(&m_packet, sizeof(DEFAULT_PACKET));
    m_packet.m_ph.m_len = PACKET_HEADER_SIZE;
    m_packet.m_ph.m_type = NULL;
}

Packet::Packet(uInt16 type) : m_pReadPos(&m_packet.m_dataField[0]), m_pWritePos(&m_packet.m_dataField[0]), m_pBeginPos(&m_packet.m_dataField[0]),
m_pEndPos(&m_packet.m_dataField[PACKET_BUFFER_SIZE - 1]), m_receivedSize(0)
{

    ::ZeroMemory(&m_packet, sizeof(DEFAULT_PACKET));
    m_packet.m_ph.m_len = PACKET_HEADER_SIZE;
    m_packet.m_ph.m_type = type;
}

Packet::~Packet()
{
}

void Packet::Add(const BYTE* pData, int32 size)
{
    m_packet.m_ph.m_len += size; // ��Ŷũ��(����)
    if (m_pWritePos + size > m_pEndPos) // �Ѿ�� ���.
    {
        int32 readLen = (int32)(m_pWritePos - m_pReadPos); // �����ͱ���.
        ::CopyMemory(m_pBeginPos, &m_packet.m_ph, PACKET_HEADER_SIZE);
        ::CopyMemory(m_pBeginPos + PACKET_HEADER_SIZE, m_pReadPos, readLen);

        m_pReadPos = m_pBeginPos + PACKET_HEADER_SIZE;
        m_pWritePos = m_pReadPos + readLen; // �ؿ� �̾ ����.
    }
    ::CopyMemory(m_pWritePos, pData, size); // ������ ����.
    m_pWritePos += size; // ��ġ �ű��.
}

void Packet::AddWString(std::wstring& data, uInt32 size)
{
    m_packet.m_ph.m_len += size; // ��Ŷũ��(����)
    if (m_pWritePos + size > m_pEndPos) // �Ѿ�� ���.
    {
        int32 readLen = (int32)(m_pWritePos - m_pReadPos); // �����ͱ���.
        ::CopyMemory(m_pBeginPos, &m_packet.m_ph, PACKET_HEADER_SIZE);
        ::CopyMemory(m_pBeginPos + PACKET_HEADER_SIZE, m_pReadPos, readLen);

        m_pReadPos = m_pBeginPos + PACKET_HEADER_SIZE;
        m_pWritePos = m_pReadPos + readLen; // �ؿ� �̾ ����.
    }
    ::CopyMemory(m_pWritePos, data.c_str(), size); // ������ ����.
    m_pWritePos += size;
    ::CopyMemory(m_pWritePos, L"\0", 2);
    m_pWritePos += 2; // ��ġ �ű��.
    m_packet.m_ph.m_len += 2;
}

void Packet::Get(BYTE* pData, int32 size)
{
    // pData�� �����͸� ����ִ´�.
    m_packet.m_ph.m_len -= size;

    ::CopyMemory(pData, m_pReadPos, size);
    m_pReadPos += size;
    if (m_pReadPos == m_pWritePos) // �� ���� ���. �� �� ó������ �ٽ� �ű��.
    {
        m_pReadPos = m_pBeginPos;
        m_pWritePos = m_pBeginPos;
    }
}

Packet& Packet::operator<<(int32 data)
{
    Add((BYTE*)&data, sizeof(int32));
    return *this;
}

Packet& Packet::operator<<(int16 data)
{
    Add((BYTE*)&data, sizeof(int16));
    return *this;
}

void Packet::CopyToPacketBuffer(char* buffer, uInt32 size)
{
    ::CopyMemory(&m_packet, buffer, size);


    m_pReadPos  = &m_packet.m_dataField[0];
    m_pWritePos = &m_packet.m_dataField[0];
    m_pBeginPos = &m_packet.m_dataField[0];
}

Packet& Packet::operator<<(int64 data)
{
    Add((BYTE*)&data, sizeof(int64));
    return *this;
}

Packet& Packet::operator<<(float data)
{
    Add((BYTE*)&data, sizeof(float));
    return *this;
}

Packet& Packet::operator<<(BYTE data)
{
    Add(&data, sizeof(BYTE));
    return *this;
}

Packet& Packet::operator<<(char* data) // ���ڿ�
{
    
    uInt32 size = static_cast<uInt32>(::strlen(data));
    Add((BYTE*)data, size + 1); // �ι��� �߰�!
    return *this;
}

Packet& Packet::operator<<(uInt32 data)
{
    Add((BYTE*)&data, sizeof(uInt32)); // �ι��� �߰�!
    return *this;
}

Packet& Packet::operator<<(std::string data)
{
    Add((BYTE*)data.c_str(), static_cast<int32>(data.size() + 1));
    return *this;
}

Packet& Packet::operator<<(std::wstring data)
{
    // TODO: ���⿡ return ���� �����մϴ�.
    uInt32 len = static_cast<uInt32>(data.size() * 2);
    AddWString(data, len);
    return *this;
}


Packet& Packet::operator>>(int64& data)
{
    Get((BYTE*)&data, sizeof(int64));
    return *this;
}

Packet& Packet::operator>>(int32& data)
{
    // data�� �����͸� �־��ش�.
    Get((BYTE*)&data, sizeof(int32));
    return *this;
}

Packet& Packet::operator>>(int16& data)
{
    Get((BYTE*)&data, sizeof(int16));
    return *this;
}

Packet& Packet::operator>>(float& data)
{
    Get((BYTE*)&data, sizeof(float));
    return *this;
}

Packet& Packet::operator>>(BYTE& data)
{
    Get((BYTE*)&data, sizeof(BYTE));
    return *this; 
}

Packet& Packet::operator>>(char** data)
{
    uInt32 len = static_cast<uInt32>(strlen(m_pReadPos) + 1); // �ι��� ����. (strlen�� �ι��ڸ� ������ ��ȯ)
    //Get((BYTE*)data, len);
    GetString(data, len);
    return *this;
}

Packet& Packet::operator>>(uInt32& data)
{
    Get((BYTE*)&data, sizeof(uInt32));
    return *this;
}

Packet& Packet::operator>>(std::string& data)
{
    uInt32 len = static_cast<uInt32>(strlen(m_pReadPos) + 1);
    Get((BYTE*)data.c_str(), len);
    return *this;
}
Packet& Packet::operator>>(std::wstring& data)
{
    GetWString(data);
    return *this;
}

void Packet::GetString(char** pData, int32 size)
{
    *pData = m_pReadPos;
    m_pReadPos += size;
    if (m_pReadPos == m_pWritePos) // �� ���� ���. �� �� ó������ �ٽ� �ű��.
    {
        m_pReadPos = m_pBeginPos;
        m_pWritePos = m_pBeginPos;
    }
}


void Packet::GetWString(std::wstring& data)
{
    data += (wchar_t*)m_pReadPos;
    m_pReadPos += (data.size() * 2 + 2);
    if (m_pReadPos == m_pWritePos)
    {
        m_pReadPos = m_pBeginPos;
        m_pWritePos = m_pBeginPos;
    }

}




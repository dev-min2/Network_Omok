#pragma once
#include <iostream>
#include <Windows.h>
#include "Types.h"

struct PACKET_HEADER
{
	uInt16 m_len;  // ��Ŷ ũ��
	uInt16 m_type; // ��Ŷ Ÿ��(��������)
};

struct DEFAULT_PACKET
{
	PACKET_HEADER m_ph;								// ��Ŷ ���
	char		  m_dataField[PACKET_BUFFER_SIZE];	// ��Ŷ ������ ����.
};

class Packet final
{
private:
	char*					m_pReadPos;		// ���� �����͸� ���� ��ġ.
	char*					m_pWritePos;	// ���� �����͸� �� ��ġ
	char*					m_pBeginPos;	// ó�� ��ġ. (������ �ʵ���)
	char*					m_pEndPos;		// ������ ��ġ (������ �ʵ���)
	DEFAULT_PACKET			m_packet;
public:
	void					CopyToPacketBuffer(char* buffer, uInt32 size);
public:
	uInt32						m_receivedSize;
public:
	inline bool				IsValidPacketHeader()		   { return m_receivedSize >= PACKET_HEADER_SIZE; }
	inline DEFAULT_PACKET&	GetDefaultPacket()			   { return m_packet; }
	inline void				SetProtocolID(uInt16 protocol) { m_packet.m_ph.m_type = protocol; }
	inline uInt16			GetProtocolID()				   { return m_packet.m_ph.m_type; }
	inline uInt16			GetPacketLen()				   { return m_packet.m_ph.m_len; }
private:
	void					Add(const BYTE* pData, int32 size); 
	void					AddWString(std::wstring& data, uInt32 size);
	void					Get(BYTE* pData, int32 size);

	// coutó�� ���������� ����ϱ� ���� ��ȯ�� ����(�ڱ��ڽ�)���� �Ѵ�.
public:
	Packet&					operator << (int64 data);
	Packet&					operator << (int32 data);
	Packet&					operator << (int16 data);
	Packet&					operator << (float data);
	Packet&					operator << (BYTE data);
	Packet&					operator << (char* data);
	Packet&					operator << (uInt32 data);
	Packet&					operator << (std::string data);
	Packet&					operator << (std::wstring data);
public:
	Packet&					operator >> (int64& data);
	Packet&					operator >> (int32& data);
	Packet&					operator >> (int16& data);
	Packet&					operator >> (float& data);
	Packet&					operator >> (BYTE& data);
	Packet&					operator >> (char** pData);
	Packet&					operator >> (uInt32& data);
	Packet&					operator >> (std::string& data);
	Packet&					operator >> (std::wstring& data);
private:
	void					GetString(char** pData,int32 size);
	void					GetWString(std::wstring&);
public:
	Packet();
	Packet(uInt16 type);
	~Packet();
	Packet(const Packet& packet) = delete;
	void operator=(const Packet& packet) = delete;
};

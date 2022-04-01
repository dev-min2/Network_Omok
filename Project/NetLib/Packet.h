#pragma once
#include "std.h"

struct PACKET_HEADER
{
	uInt16 m_len;  // 패킷 크기
	uInt16 m_type; // 패킷 타입(프로토콜)
};

struct DEFAULT_PACKET
{
	PACKET_HEADER m_ph;								// 패킷 헤더
	char		  m_dataField[PACKET_BUFFER_SIZE];	// 패킷 데이터 영역.
};

class Packet final
{
private:
	char*					m_pReadPos;		// 현재 데이터를 읽을 위치.
	char*					m_pWritePos;	// 현재 데이터를 쓸 위치
	char*					m_pBeginPos;	// 처음 위치. (데이터 필드의)
	char*					m_pEndPos;		// 마지막 위치 (데이터 필드의)
	DEFAULT_PACKET			m_packet;
public:
	void					CopyToPacketBuffer(char* buffer, uInt32 size);
public:
	uInt32					m_receivedSize;
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

	// cout처럼 연속적으로 사용하기 위해 반환이 참조(자기자신)여야 한다.
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

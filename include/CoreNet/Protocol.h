#pragma once
#include <Windows.h>
#include "Types.h"
// 구조체는 멤버중 크기가 가장 큰 자료형을 기준으로 배수만큼 커진다.
// 즉 구조체의 크기는 변수의 크기에 패딩 비트가 더해짐
 // 패딩 비트란 구조체 크기를 가장 큰 자료형 기준으로 배수만큼 만들어주기 위해 추가되는 비트.
// 이렇게 패딩비트를 추가하는 이유는 운영체제(32/64bit)에서 4바이트 기준으로 데이터를 처리하는것이 가장 빠른 속도를 가지기 때문.
// 네트워크 통신에서는 정확한 통신 패킷 사이즈를 지정하기 위해서 위와 같은 패딩비트는 사용하면 안된다.
#define PACKET_HEADER_SIZE 4
#define PACKET_BUF_SIZE 2048
#pragma pack(push,1) // 구조체 크기를 1바이트 단위로 정렬
//struct PACKET_HEADER
//{
//	WORD m_len;  // 패킷 크기
//	WORD m_type; // 패킷 타입(프로토콜)
//};
//
//struct DEFAULT_PACKET
//{
//	PACKET_HEADER m_ph;							// 패킷 헤더
//	char		  m_dataField[PACKET_BUF_SIZE];	// 패킷 데이터 영역.
//};

enum PROTOCOL
{
	CS_PLAYER_LOGIN = 1001,
	SC_PLAYER_LOGIN,

	CS_PLAYER_LIST,
	SC_PLAYER_LIST,

	SC_PLAYER_START_POS,

	CS_PLAYER_LOGOUT,
	SC_PLAYER_LOGOUT,

	CS_PLAYER_MOVE,
	SC_PLAYER_MOVE,
};
#pragma pack(push,1)


struct SC_PLAYER_STARTPOS_PACKET
{
	uInt32 ID;
	float posX;
	float posY;
};

struct CS_PLAYER_MOVE_PACKET
{
	float	posX;
	float	posY;
};

struct SC_PLAYER_MOVE_PACKET
{
	uInt32	ID;
	float		posX;
	float		posY;
};
struct CS_PLAYER_LOGIN_PACKET
{
	float posX;
	float posY;
};
struct SC_PLAYER_LOGIN_PACKET
{
	uInt32 myID;
};
struct CS_PLAYER_LIST_PACKET
{

};
struct SC_PLAYER_LIST_PACKET
{
	uInt32 playerNums;
};
struct SC_PLAYER_LIST_CHECK
{
	uInt32 playerID;
	float  posX;
	float  posY;
};
struct CS_PLAYER_LOGOUT_PACKET
{
	uInt32 playerID;
};
struct SC_PLAYER_LOGOUT_PACKET
{
	uInt32 playerID;
};



struct PRO_CHAT_MSG
{
	char* name;
	char* msg;
};
#pragma pack(pop)
#define		PROTOCOL_CHAT_MSG 1000
#define		PROTOCOL_HEARTBEATS 2000

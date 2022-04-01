#pragma once
#include <Windows.h>
#include "Types.h"
// ����ü�� ����� ũ�Ⱑ ���� ū �ڷ����� �������� �����ŭ Ŀ����.
// �� ����ü�� ũ��� ������ ũ�⿡ �е� ��Ʈ�� ������
 // �е� ��Ʈ�� ����ü ũ�⸦ ���� ū �ڷ��� �������� �����ŭ ������ֱ� ���� �߰��Ǵ� ��Ʈ.
// �̷��� �е���Ʈ�� �߰��ϴ� ������ �ü��(32/64bit)���� 4����Ʈ �������� �����͸� ó���ϴ°��� ���� ���� �ӵ��� ������ ����.
// ��Ʈ��ũ ��ſ����� ��Ȯ�� ��� ��Ŷ ����� �����ϱ� ���ؼ� ���� ���� �е���Ʈ�� ����ϸ� �ȵȴ�.
#define PACKET_HEADER_SIZE 4
#define PACKET_BUF_SIZE 2048
#pragma pack(push,1) // ����ü ũ�⸦ 1����Ʈ ������ ����
//struct PACKET_HEADER
//{
//	WORD m_len;  // ��Ŷ ũ��
//	WORD m_type; // ��Ŷ Ÿ��(��������)
//};
//
//struct DEFAULT_PACKET
//{
//	PACKET_HEADER m_ph;							// ��Ŷ ���
//	char		  m_dataField[PACKET_BUF_SIZE];	// ��Ŷ ������ ����.
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

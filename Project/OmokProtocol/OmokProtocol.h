#pragma once
#include <iostream>
#include <cstdint>
using int64 = std::int64_t;
using int32 = std::int32_t;
using int16 = std::int16_t;
using int8 = std::int8_t;
using uInt64 = std::uint64_t;
using uInt32 = std::uint32_t;
using uInt16 = std::uint16_t;
using uInt8 = std::uint8_t;


#define OK 3900
#define NO 2450
#define MAX_ROOM 100

enum PlayerScene
{
	PS_CHANNEL = 200,
	PS_GAMESCENE = 300
};
enum RETVALUE
{
	RV_OK = 0,
	RV_OVERLAP,
	RV_AlreadyLogin,
	RV_ERROR
};
enum class OmProtocol
{
	CS_REQU_ACCOUNT = 1000,
	SC_REQU_ACCOUNT,

	CS_REQU_LOGIN,
	SC_REQU_LOGIN,
	SC_REQU_PLAYER_LIST,

	CS_REQU_CREATE_ROOM,
	SC_REQU_CREATE_ROOM,

	CS_REQU_ROOM_ENTRY,
	SC_REQU_ROOM_ENTRY,
	SC_REQU_PLAYER_ENTRY,

	CS_REQU_PLAYER_READY,
	SC_REQU_PLAYER_READY,

	CS_REQU_PLAYER_START,
	SC_REQU_PLAYER_START,

	CS_REQU_PUT_STONE,
	SC_REQU_PUT_STONE,
	
	SC_REQU_FINISH,

	CS_REQU_PLAYER_ROOM_EXIT, 



	SC_REQU_HOST_PLAYER_ROOM_EXIT,  
	SC_REQU_ENTER_PLAYER_ROOM_EXIT, 
	SC_REQU_ROOMLIST,
	SC_REQU_PLAYER_EXIT // �÷��̾� ��������
};
#pragma pack(push,1)
/// <summary>
/// Ŭ��-���� 
/// �������� ��û. �ߺ����̵� üũ�ʿ�.
/// </summary>
struct CS_REQU_ACCOUNT_PACKET
{
	std::wstring id;
	std::wstring ps;
	std::wstring age;
};
struct SC_REQU_ACCOUNT_PACKET
{
	uInt32 ret;
};
/// <summary>
/// Ŭ�� - ����
/// �α��� ��û. ���̵� ��ȸ�ʿ�.
//  UserPlayDB�� ���´�.
/// </summary>
struct CS_REQU_LOGIN_PACKET
{
	std::wstring id;
	std::wstring ps;
};
struct SC_REQU_LOGIN_PACKET
{
	uInt32 ret;
	std::wstring id;
	int32  totalPlayGameCount;
	int32  winGameCount;
	int32  loseGameCount;
	float  winRate;
};
struct SC_REQU_PLAYER_LIST_PACKET
{
	uInt32	playerCount;
};

/// <summary>
/// Ŭ�� - ����
/// Ŭ�� : �� ���� ��û. ��û�� �÷��̾��� id�� �� ����,��й�ȣ�� ������.
/// ���� : �� �ε���,������ Ȯ�� �� ����. (��� �����鿡�� BroadCast)
/// </summary>
struct CS_REQU_CREATE_ROOM_PACKET
{
	std::wstring hostPlayerId;
	std::wstring roomTitle;
	std::wstring roomPs;
};
struct SC_REQU_CREATE_ROOM_PACKET
{
	std::wstring hostPlayerId;
	std::wstring roomTitle;
	uInt32		 roomIndex;
};

/// <summary>
/// Ŭ�� - ����
/// Ŭ�� : �� ���� ��û. �� �ڸ� �ִ���.
/// ���� : ���� ���� üũ �� hoistId��ȯ
/// ���� : ���� ������ �� host���� ������ ������ id�۽�
/// </summary>
struct CS_REQU_ROOM_ENTRY_PACKET
{
	std::wstring roomTitle;
	std::wstring id; //
};
struct SC_REQU_ROOM_ENTRY_PACKET
{
	uInt32		 entry; // ���� ����. ( 0 : false , 1 : true )
	std::wstring hostId;
	std::wstring enterId;
	std::wstring roomTitle;
};
struct SC_REQU_PLAYER_ENTRY_PACKET
{
	std::wstring enterId;
};

/// <summary>
/// Ŭ�� - ����
/// Ŭ�� : ���� ���� �÷��̾ �����ư�� Ŭ��.
/// ���� : ���� ����� Enter������ Ready���� üũ.
/// </summary>
struct CS_REQU_PLAYER_READY_PACKET
{
	std::wstring hostPlayerId;
};
struct SC_REQU_PLAYER_READY_PACKET
{

};

/// <summary>
/// Ŭ�� - ����
/// Ŭ�� : ���̵�, ���� ���� ��ġ(x,y),(�ش� ��ġ�� ���� ���� �� �ִ����� Ŭ�󿡼� üũ, ���� �������� ���� �۽�)
/// ���� : ���� ���� ���� �������� �ش� ��ġ �۽�(SendPacket)
/// </summary>
struct CS_REQU_PUT_STONE_PACKET
{
	std::wstring putId;
	std::wstring anotherId;
	std::wstring roomTitle;
	uInt32 putY;
	uInt32 putX;
	uInt32 stoneType;
	
};
struct SC_REQU_PUT_STONE_PACKET
{	
	uInt32 putY;
	uInt32 putX;
	uInt32 stoneType;
};

/// <summary>
/// ����
/// �̱� ������ ���̵�� ������ �۽�.
/// </summary>
struct SC_REQU_FINISH_PACKET
{
	std::wstring winPlayer;
	std::wstring roomTitle;
};

/// <summary>
/// Ŭ�� - ����
/// Ŭ�� : ���� ��ŸƮ ��û
/// ���� : �� ���� Ȯ�� �� ���� BroadCast. -> ���� Ŭ�󿡼� �ؽ��� �����ʿ�.
/// </summary>
struct CS_REQU_PLAYER_START_PACKET
{
	std::wstring roomTitle;
};
struct SC_REQU_PLAYER_START_PACKET
{
	uInt32		 isStart;
	std::wstring roomTitle;
	std::wstring hostId;
};

/// <summary>
/// Ŭ��->����
/// Ŭ�� : �뿡�� �������ư�� ���� ������ ���.
/// �������ῡ�� ���� �Լ��� �״�� ����Ѵ�.
/// </summary>
struct CS_REQU_PLAYER_ROOM_EXIT_PACKET
{
	std::wstring exitRoomTitle;
	std::wstring exitPlayerId;
	//uInt32		 isHost;
};

/// <summary>
/// ����
/// Enter������ �ִµ� Host������ ������ ���
/// </summary>
struct SC_REQU_HOST_PLAYER_ROOM_EXIT_PACKET
{
	std::wstring titleName;
	std::wstring exitHostId;
	uInt32		 emptyRoom;
	std::wstring newHostID;
};
/// <summary>
/// Host������ �ִµ� Enter������ ������ ���.
/// </summary>
struct SC_REQU_ENTER_PLAYER_ROOM_EXIT_PACKET
{
	std::wstring titleName;
	std::wstring exitEnterId;
	//std::wstring exitPlayerID;
};
/// <summary>
/// ���� �α��� �� �� ��� �ش� �������� �۽�.
/// ���� �� roomCount��ŭ title,hostName�� std::wstring�� �о�鿩���Ѵ�.
/// roomIndex�� roomPage�� �޾ƿ;��Ѵ�.
/// </summary>
struct SC_REQU_ROOMLIST_PACKET
{
	uInt32 roomCount;
	//std::wstring titleAndhosts[MAX_ROOM];
};
/// <summary>
/// ����
/// ������ ���������� �� ������ ��Ŷ.
/// </summary>
struct SC_REQU_PLAYER_EXIT_PACKET
{
	std::wstring exitPlayerId;
};

#pragma pack(pop)
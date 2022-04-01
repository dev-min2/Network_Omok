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
	SC_REQU_PLAYER_EXIT // 플레이어 게임종료
};
#pragma pack(push,1)
/// <summary>
/// 클라-서버 
/// 계정생성 요청. 중복아이디 체크필요.
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
/// 클라 - 서버
/// 로그인 요청. 아이디 조회필요.
//  UserPlayDB값 들고온다.
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
/// 클라 - 서버
/// 클라 : 방 생성 요청. 요청한 플레이어의 id와 방 제목,비밀번호를 보낸다.
/// 서버 : 방 인덱스,페이지 확인 후 답장. (모든 유저들에게 BroadCast)
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
/// 클라 - 서버
/// 클라 : 방 입장 요청. 들어갈 자리 있는지.
/// 서버 : 입장 가능 체크 및 hoistId반환
/// 서버 : 입장 가능할 시 host에게 입장한 유저의 id송신
/// </summary>
struct CS_REQU_ROOM_ENTRY_PACKET
{
	std::wstring roomTitle;
	std::wstring id; //
};
struct SC_REQU_ROOM_ENTRY_PACKET
{
	uInt32		 entry; // 입장 여부. ( 0 : false , 1 : true )
	std::wstring hostId;
	std::wstring enterId;
	std::wstring roomTitle;
};
struct SC_REQU_PLAYER_ENTRY_PACKET
{
	std::wstring enterId;
};

/// <summary>
/// 클라 - 서버
/// 클라 : 게임 씬의 플레이어가 레디버튼을 클릭.
/// 서버 : 같은 방안의 Enter유저의 Ready만을 체크.
/// </summary>
struct CS_REQU_PLAYER_READY_PACKET
{
	std::wstring hostPlayerId;
};
struct SC_REQU_PLAYER_READY_PACKET
{

};

/// <summary>
/// 클라 - 서버
/// 클라 : 아이디, 돌을 놓은 위치(x,y),(해당 위치에 돌을 놓을 수 있는지는 클라에서 체크, 상대방 유저에게 정보 송신)
/// 서버 : 돌을 놓고 상대방 유저에게 해당 위치 송신(SendPacket)
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
/// 서버
/// 이긴 유저의 아이디와 방제목 송신.
/// </summary>
struct SC_REQU_FINISH_PACKET
{
	std::wstring winPlayer;
	std::wstring roomTitle;
};

/// <summary>
/// 클라 - 서버
/// 클라 : 게임 스타트 요청
/// 서버 : 룸 상태 확인 후 승인 BroadCast. -> 이후 클라에서 텍스쳐 변경필요.
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
/// 클라->서버
/// 클라 : 룸에서 나가기버튼을 통해 나오는 경우.
/// 강제종료에서 쓰인 함수를 그대로 사용한다.
/// </summary>
struct CS_REQU_PLAYER_ROOM_EXIT_PACKET
{
	std::wstring exitRoomTitle;
	std::wstring exitPlayerId;
	//uInt32		 isHost;
};

/// <summary>
/// 서버
/// Enter유저가 있는데 Host유저가 나가는 경우
/// </summary>
struct SC_REQU_HOST_PLAYER_ROOM_EXIT_PACKET
{
	std::wstring titleName;
	std::wstring exitHostId;
	uInt32		 emptyRoom;
	std::wstring newHostID;
};
/// <summary>
/// Host유저가 있는데 Enter유저가 나가는 경우.
/// </summary>
struct SC_REQU_ENTER_PLAYER_ROOM_EXIT_PACKET
{
	std::wstring titleName;
	std::wstring exitEnterId;
	//std::wstring exitPlayerID;
};
/// <summary>
/// 서버 로그인 시 방 목록 해당 유저에게 송신.
/// 읽을 때 roomCount만큼 title,hostName의 std::wstring을 읽어들여야한다.
/// roomIndex와 roomPage도 받아와야한다.
/// </summary>
struct SC_REQU_ROOMLIST_PACKET
{
	uInt32 roomCount;
	//std::wstring titleAndhosts[MAX_ROOM];
};
/// <summary>
/// 서버
/// 유저가 접속종료할 때 보내는 패킷.
/// </summary>
struct SC_REQU_PLAYER_EXIT_PACKET
{
	std::wstring exitPlayerId;
};

#pragma pack(pop)
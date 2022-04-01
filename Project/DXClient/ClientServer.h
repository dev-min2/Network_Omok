#pragma once
#include "Types.h"
#include "OmokProtocol.h"
class Packet;
class ClientServer
{
public:
	static	ClientServer*	GetInstance();
	static	void			ReleaseInstance();
public:
	bool					Init();
	bool					Update();
	bool					Release();
public:
	bool					GetIsLogin() { return m_isLogin; }
	bool					GetIsGameScene() { return m_isGameScene; }
private:
	void					ProcessRecvPacket();

	// 계정생성
private:
	void					RecvCreateAccountPacket(uInt32 answer);
	void					RecvLoginResult(Packet* answer);
	void					RecvPlayerList(Packet* packet);
	void					RecvCreateRoom(SC_REQU_CREATE_ROOM_PACKET& answer);
	
	void					RecvRoomList(Packet* packet,uInt32 roomCount);
	void					RecvEnterUserInRoom(Packet* packet); // 입장유저
	void					RecvEnterPlayerReady(Packet* packet);
	void					RecvGameStart(Packet* packet);
	void					RecvPutStone(Packet* packet);
	void					RecvFinishGame(Packet* packet);

	
	void					RecvHostPlayerExit(Packet* packet); // 호스트 변경(Enter유저존재)
	void					RecvEnterPlayerExit(Packet* packet); // Enter유저 Exit
	void					RecvPlayerExit(Packet* packet); // 플레이어 종료
public:
	void					SetLoginScene(bool value) { m_isLoginScene = value; }
private:
	ClientServer() = default;
	~ClientServer() = default;
private:
	static ClientServer*	m_pInstance;
	bool					m_isLoginScene = true;
	bool					m_isLogin = false;
	bool					m_isGameScene = false;
};


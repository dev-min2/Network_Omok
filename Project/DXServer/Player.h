#pragma once
#include "std.h"
class PlayerManager;
class UserInfoDB;
class User;


class Player
{
	enum OmokType
	{
		OT_EMPTY = 0,
		OT_BLACK,
		OT_WHITE
	};
	friend class PlayerManager;
public:
	Player(User* user);
	~Player();
public:
	void			SetPlayerID(std::wstring& name);
	UserInfoDB*		GetPlayerInfo() { return m_playerInfo; }
	void			SetPlayerInfo(UserInfoDB& info);
	User*			GetClient() { return m_client; }
	void			SetPlayerScene(uInt32 sceneId);
	uInt32			GetPlayerScene() { return m_playerScene; }
	void			SetPlayGame(bool value) { m_isPlayGame = value; }
	bool			GetIsPlayGame() { return m_isPlayGame; }
	void			PutStone(uInt32 stoneType,uInt32 putY,uInt32 putX);
	bool			CheckFinish(uInt32 stoneType, uInt32 putY, uInt32 putX);
	void			ClearOmok();
public:
	void			WinGame();
	void			LoseGame();
private:
	float			CalculateRate(uInt32 winCount, uInt32 loseCount);
private:
	bool			CheckRULD(uInt32 stoneType,uInt32 putY,uInt32 putX); // ¿ì»ó,ÁÂÇÏ Ã¼Å©
	bool			CheckRRLL(uInt32 stoneType,uInt32 putY,uInt32 putX); // ¿ì»ó,ÁÂÇÏ Ã¼Å©
	bool			CheckRDLU(uInt32 stoneType,uInt32 putY,uInt32 putX); // ¿ì»ó,ÁÂÇÏ Ã¼Å©
	bool			CheckDDUU(uInt32 stoneType,uInt32 putY,uInt32 putX); // ¿ì»ó,ÁÂÇÏ Ã¼Å©
	
private:
	UserInfoDB*		m_playerInfo;
	User*			m_client;
	bool			m_isPlayGame;
	uInt32			m_playerScene;
	uInt32			m_omok[19][19];
};


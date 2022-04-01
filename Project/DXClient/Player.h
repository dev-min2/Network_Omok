#pragma once
#include "Types.h"
#include <string>
#include <list>
class GameScene;
class PlayerManager;
struct UserInfo
{
	UserInfo() : m_totalPlayGameCount(0),m_winGameCount(0),m_loseGameCount(0),m_winRate(0.0f) { }
	std::wstring m_name;
	int32		 m_totalPlayGameCount;
	int32		 m_winGameCount;
	int32		 m_loseGameCount;
	float		 m_winRate;
};
class Player
{
	friend class PlayerManager;
public:
	Player(std::wstring& name,int32 total, int32 win, int32 lose, float rate);
	~Player();
public:
	UserInfo		GetPlayerInfo() { return m_userInfo; }
	bool			IsPlayerInGameScene() { return m_isGameScene; }
	void			SetPlayerInGameScene(bool value) { m_isGameScene = value; }

	void			SetGameScene(GameScene* pGameScene) { m_pPlayerInGameScene = pGameScene; }

	void			SetPlayGame(bool value) { m_isPlayingGame = value; }
	bool			GetPlayGame() { return m_isPlayingGame; }

	GameScene*		GetPlayerGameScene() { return m_pPlayerInGameScene; }
	GameScene**		GetDoublePtrGameScene() { return &m_pPlayerInGameScene; }
public:
	void			WinGame();
	void			LoseGame();
private:
	float			CalculateRate(uInt32 winCount, uInt32 loseCount);
private:
	UserInfo		m_userInfo;
	bool			m_isGameScene;
	bool			m_isPlayingGame;
	GameScene*		m_pPlayerInGameScene;



};


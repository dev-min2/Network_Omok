#pragma once
#include "Scene.h"
#include "Vector3.h"
#include "Types.h"
#include "Stone.h"
class Sound;
class GameDlg;
class OmokGround;
class Player;
class StartBtn;
class ReadyBtn;
class ExitBtn;
#define MAX_X 19
#define MAX_Y 19
class GameScene : public Scene
{
	friend class OmokGround;
public:
	GameScene();
	~GameScene() override;
public:
	bool							Load(std::wstring file) override;
	bool							Init() override;
	bool							Update() override;
	bool							Render() override;
	bool							Release() override;
public:
	void							SetHostPlayer(Player* pPlayer) { m_pHostPlayer = pPlayer; }
	Player*							GetHostPlayer() { return m_pHostPlayer; }

	void							SetEnterPlayer(Player* pPlayer) { m_pEnterPlayer = pPlayer; }
	Player*							GetEnterPlayer() { return m_pEnterPlayer; }

	void							SetIsHost(bool value) { m_isHost = value; } // 레디 or 스타트버튼 확인용으로만 쓰인다.(자기화면용)
	bool							GetIsHost() { return m_isHost; }

	void							SetGameStart(bool value) { m_isGameStart = value; }
	bool							GetGameStart() { return m_isGameStart; }

	void							SetReady(bool value) { m_isReady = value; }
	bool							GetIsReady() { return m_isReady; }
	void							SetTitleName(std::wstring& title) { m_titleName = title; }
	std::wstring					GetTitleName() { return m_titleName; }

	bool							GetIsMyTurn() { return m_isMyTurn; }
	void							SetIsMyTurn(bool value) { m_isMyTurn = value; }

	void							ReSetCollision() override;
public:
	void							PutStone(StoneType type,uInt32 posY,uInt32 posX);
	OmokGround*						GetOmokGround() { return m_pOmokGround; }
	void							ClearOmok();
private:
	bool							CreateGameDlg();
	bool							CreateOmokGround();
	bool							CreateBtn();
	bool							CreatePlayerImage();
	void							CreateOmokDistance();
	bool							CreatePlayerStoneImage();
private:
	void							RenderPlayerImage();

private:
	std::pair<bool, Vector2>		m_omokPair[MAX_Y][MAX_X];
private:
	Sound*							m_pBackGroundMusic;
	GameDlg*						m_pGamedlg;
	OmokGround*						m_pOmokGround;
	Player*							m_pHostPlayer;
	Player*							m_pEnterPlayer;

	StartBtn*						m_pStartBtn;
	ReadyBtn*						m_pReadyBtn;
	ExitBtn*						m_pExitBtn;

	Object2D*						m_pBlackStoneImage;
	Object2D*						m_pWhiteStoneImage;

	Object2D*						m_pHostPlayerImage;
	Object2D*						m_pEnterPlayerImage;
	Object2D*						m_pReadyImage;

	std::wstring					m_titleName;

	bool							m_isReady;
	bool							m_isHost;
	bool							m_isGameStart;
	bool							m_isMyTurn;
	bool							m_isChangeStone;
};


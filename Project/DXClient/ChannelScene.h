#pragma once
#include <string>
#include "Scene.h"
#include "Types.h"


class Sound;
class QuickStartUI;
class RoomMakeUI;
class Player;
class RoomUI;

#define MAX_INFO 5
class ChannelScene : public Scene
{
	enum PlayerInfo
	{
		PI_ID = 0,
		PI_TOTAL,
		PI_WIN,
		PI_LOSE,
		PI_RATE
	};
public:
	ChannelScene();
	~ChannelScene() override;
public:
	bool				Init() override;
	bool				Update() override;
	bool				Render() override;
	bool				Release() override;
public:
	bool				CreateBackgroundImage();
	bool				CreateRoomMakeUI();
	bool				CreateQuickStartUI();
	bool				CreatePlayerImage();
	bool				CreateRoom(std::wstring& hostName, std::wstring& titleName, uInt32 roomIndices, uInt32 playerCountInRoom, bool isPlayGame);
	void				DeleteRoom(std::wstring& titleName);
	void				SetPlayer(Player* pPlayer) { m_pPlayer = pPlayer; }
	Player*				GetPlayer() { return m_pPlayer; }
	void				CreateRoomUIs(uInt32 roomCount,std::vector<std::wstring>& hosts, std::vector<std::wstring>& titles, std::vector<uInt32>& roomIndices, std::vector<uInt32>& playerCountInRoom,std::vector<uInt32>& roomState);
	void				RenderRoomUIs();
	

public:
	std::unordered_map<std::wstring, RoomUI*>& GetRoomMap() { return m_roomMap; }
	RoomMakeUI*			GetRoomMakeUI() { return m_pRoomMakeUI; }
	void				SetInfo(Player* pPlayer);
private:
	void				CreateInfo();
	void				RenderPlayerInfo();
private:
	Sound*				m_pBackgroundMusic;
	Object2D*			m_pBackgroundImage;
	QuickStartUI*		m_pQuickStartUI;
	RoomMakeUI*			m_pRoomMakeUI;
	Object2D*			m_pPlayerImage;
	
	uInt32				m_curRoomPage;
	Player*				m_pPlayer;

	
	std::unordered_map<std::wstring, RoomUI*> m_roomMap; // title

	std::wstring		m_playerInfoBuffer[MAX_INFO];
	RECT				m_playerInfoRect[MAX_INFO];
};


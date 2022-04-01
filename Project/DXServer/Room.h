#pragma once
#include "Types.h"
#include <string>
class RoomManager;
class Player;
class Room
{
	friend class RoomManager;
public:
	Room(uInt32 roomIndex,std::wstring& roomTitle, std::wstring& roomPs);
	~Room();
public:
	Player*			GetHostPlayer() { return m_pHostPlayer; }
	Player*			GetEnterPlayer() { return m_pEnterPlayer; }
	void			SetHostPlayer(Player* pPlayer) { m_pHostPlayer = pPlayer; }
	void			SetEnterPlayer(Player* pPlayer) { m_pEnterPlayer = pPlayer; }
	void			IncreasePlayer() { ++m_playerCount; }
	void			DecreasePlayer() { --m_playerCount; }
	std::wstring	GetTitleName() { return m_roomTitleName; }
	uInt32			GetRoomIndex() { return m_roomIndex; }
	void			SetRoomIndex(uInt32 index) { m_roomIndex = index; }
	uInt32			GetPlayerCount() { return m_playerCount; }
	bool			GetRoomState() { return m_isPlayGame; }
	void			SetRoomState(bool value) { m_isPlayGame = value; }
	bool			IsFull();
private:
	uInt32						m_roomIndex;
	uInt32						m_playerCount;
	Player*						m_pHostPlayer;
	Player*						m_pEnterPlayer;
	bool						m_isPlayGame;
	std::wstring				m_roomTitleName;
	std::wstring				m_roomPs;
};


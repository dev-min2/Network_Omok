#include "Room.h"
#include "Player.h"
Room::Room(uInt32 roomIndex, std::wstring& roomTitle, std::wstring& roomPs) : m_roomIndex(roomIndex),
	 m_roomTitleName(roomTitle), m_roomPs(roomPs),m_pEnterPlayer(nullptr),m_pHostPlayer(nullptr), m_isPlayGame(false), m_playerCount(1)
{

}
Room::~Room()
{
}

bool Room::IsFull()
{
	if (m_pHostPlayer != nullptr && m_pEnterPlayer != nullptr)
		return true;
	return false;
}

#pragma once
#include "std.h"
class Room;
#define ROOM_MAX_PAGE 10
class RoomManager
{
public:
	static	RoomManager*					GetInstance();
	static	void							ReleaseInstance();
private:
	RoomManager();
	~RoomManager();
public:
	std::unordered_map<std::wstring, Room*>& GetRoomMap() { return m_roomMap; }
	uInt32									 GetRoomCount() { return m_roomCount++; }
	Room*									 FindRoom(std::wstring& titleName);
	bool									 DeleteRoom(std::wstring& titleName);
	void									 InsertRoom(std::wstring& title, Room* room);
	bool									 EntryRoom(std::wstring& title,std::wstring& enterId);
private:
	static	RoomManager*					m_pInstance;
	std::unordered_map<std::wstring, Room*> m_roomMap;
	std::atomic<uInt32>						m_roomCount;
	std::mutex								m_mutex;
};


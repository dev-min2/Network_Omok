#pragma once
#include "std.h"
class Player;
class PlayerManager
{
public:
	static	PlayerManager*						GetInstance();
	static	void								ReleaseInstance();
public:
	std::unordered_map<std::wstring, Player*>&	GetPlayerMap() { return m_playerMap; }
	Player*										FindPlayer(std::wstring& id);
	void										DeletePlayer(std::wstring& id);
	void										InsertPlayer(std::wstring& id, Player* pPlayer);
	uInt32										GetPlayerCount() { return m_playerMap.size(); }
private:
	static	PlayerManager*						m_pInstance;
	std::unordered_map<std::wstring, Player*>	m_playerMap;
	std::mutex									m_mutex;
};


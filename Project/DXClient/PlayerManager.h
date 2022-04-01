#pragma once
#include "Types.h"
#include <unordered_map>
class Player;
class PlayerManager
{
public:
	static PlayerManager*							GetInstance();
	static void										ReleaseInstance();
public:
	Player*											FindPlayer(std::wstring& playerID);
	void											InsertPlayer(std::wstring playerID, Player* pPlayer);
	void											DeletePlayer(std::wstring& playerID);
	void											DeleteScene();
private:
	std::unordered_map<std::wstring, Player*>	    m_playerMap;
	static	PlayerManager*							m_pInstance;
};


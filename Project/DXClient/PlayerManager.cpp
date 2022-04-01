#include "PlayerManager.h"
#include "GameScene.h"
#include "Player.h"
#include "std.h"
PlayerManager* PlayerManager::m_pInstance = nullptr;

PlayerManager* PlayerManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new PlayerManager();
	}
	return m_pInstance;
}
void PlayerManager::ReleaseInstance()
{
	if (m_pInstance->m_playerMap.size() > 0)
	{
		/*for (auto delPlayer : m_pInstance->m_playerMap)
		{
			SAFE_DELETE(delPlayer.second);
		}*/
		m_pInstance->m_playerMap.clear();
	}
	if (m_pInstance != nullptr)
	{
		SAFE_DELETE(m_pInstance);
	}
}


Player* PlayerManager::FindPlayer(std::wstring& playerID)
{
	auto find = m_playerMap.find(playerID);
	if (find != m_playerMap.end())
		return find->second;
	return nullptr;
}
void PlayerManager::InsertPlayer(std::wstring playerID, Player* pPlayer)
{
	m_playerMap.insert(std::make_pair(playerID, pPlayer));
}
void PlayerManager::DeletePlayer(std::wstring& playerID)
{
	auto find = m_playerMap.find(playerID);
	if (find != m_playerMap.end())
	{
		SAFE_DELETE(find->second);
		m_playerMap.erase(find);
	}
	
}

void PlayerManager::DeleteScene()
{
	if (m_playerMap.size() > 0)
	{
		for (auto del : m_playerMap)
		{
			/*GameScene** pGameScene = del.second->GetDoublePtrGameScene();
			if (*pGameScene != nullptr)
			{
				(*pGameScene)->Release();
				SAFE_DELETE(*pGameScene);
			}*/

			SAFE_DELETE(del.second);
		}
	}
	m_playerMap.clear();
}

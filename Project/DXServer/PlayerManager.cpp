
#include "PlayerManager.h"
#include "std.h"
#include "Player.h"
PlayerManager* PlayerManager::m_pInstance = nullptr;

PlayerManager* PlayerManager::GetInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new PlayerManager();
    return m_pInstance;
}

void PlayerManager::ReleaseInstance()
{
    auto iter = m_pInstance->m_playerMap.begin();
    for (; iter != m_pInstance->m_playerMap.end(); )
    {
        SAFE_DELETE(iter->second);
    }

    if (m_pInstance != nullptr)
    {
        SAFE_DELETE(m_pInstance);
    }
}

void PlayerManager::InsertPlayer(std::wstring& id, Player* pPlayer)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    {
        m_playerMap.insert(std::make_pair(id, pPlayer));
    }
}

Player* PlayerManager::FindPlayer(std::wstring& id)
{
    auto find = m_playerMap.find(id);
    if (find != m_playerMap.end())
        return find->second;
    return nullptr;
}

void PlayerManager::DeletePlayer(std::wstring& id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    {
        auto find = m_playerMap.find(id);
        if (find != m_playerMap.end())
        {
            SAFE_DELETE(find->second);
            m_playerMap.erase(find);
        }
    }
    
}

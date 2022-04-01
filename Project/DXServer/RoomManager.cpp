#include "RoomManager.h"
#include "Room.h"
#include "Player.h"
#include "PlayerManager.h"
#include "OdbcContents.h"
RoomManager* RoomManager::m_pInstance = nullptr;

RoomManager* RoomManager::GetInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new RoomManager();
    return m_pInstance;
}

void RoomManager::ReleaseInstance()
{
    auto iter = m_pInstance->m_roomMap.begin();
    for (; iter != m_pInstance->m_roomMap.end(); )
    {
        SAFE_DELETE(iter->second);
    }
    if (m_pInstance != nullptr)
    {
        SAFE_DELETE(m_pInstance);
    }
}

RoomManager::RoomManager() : m_roomCount(0)
{
}

RoomManager::~RoomManager()
{
}

Room* RoomManager::FindRoom(std::wstring& titleName)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    {
        auto find = m_roomMap.find(titleName);
        if (find != m_roomMap.end())
            return find->second;
    }
    return nullptr;
}

bool RoomManager::DeleteRoom(std::wstring& titleName)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    {
        auto find = m_roomMap.find(titleName);
        if (find != m_roomMap.end())
        {
            uInt32 index = find->second->GetRoomIndex();
            for (auto iter : m_roomMap)
            {
                Room* pRoom = iter.second;
                uInt32 desIndex = pRoom->GetRoomIndex();
                if (pRoom->GetRoomIndex() > index)
                {
                    if (desIndex > index)
                        pRoom->SetRoomIndex(desIndex - 1);
                }
            }
            SAFE_DELETE(find->second);
            m_roomMap.erase(find);
            --m_roomCount;
            return true;
        }
    }
    return false;
}

void RoomManager::InsertRoom(std::wstring& title, Room* room)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    {
        m_roomMap.insert(std::make_pair(title, room));
    }
}

bool RoomManager::EntryRoom(std::wstring& title, std::wstring& enterId)
{
    Room* pRoom = FindRoom(title);
    if (pRoom == nullptr)
        return false;

    std::lock_guard<std::mutex> lock(m_mutex);
    {
        if (pRoom->IsFull())
            return false;

        Player* pPlayer = PlayerManager::GetInstance()->FindPlayer(enterId);
        if (pPlayer == nullptr)
            return false;

        //pRoom->SetEnterPlayer(PlayerManager::GetInstance()->FindPlayer(pPlayer->GetPlayerInfo()->m_id));
    }


    return true;
}

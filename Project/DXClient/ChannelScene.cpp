#include "ChannelScene.h"
#include "Sound.h"
#include "QuickStartUI.h"
#include "RoomMakeUI.h"
#include "RoomUI.h"
#include "ObjectManager.h"
#include "DXWrite.h"
#include "Player.h"
ChannelScene::ChannelScene() : m_pBackgroundImage(nullptr),m_pBackgroundMusic(nullptr), m_pQuickStartUI(nullptr),
m_pRoomMakeUI(nullptr), m_curRoomPage(1),m_pPlayer(nullptr),m_pPlayerImage(nullptr)

{
    

    
}

ChannelScene::~ChannelScene()
{
}

bool ChannelScene::Init()
{
    Scene::Init();
    if (!CreateBackgroundImage())
        return false;
    if (!CreateRoomMakeUI())
        return false;
    if (!CreateQuickStartUI())
        return false;
    if (!CreatePlayerImage())
        return false;
    CreateInfo();
    return true;
}

bool ChannelScene::Update()
{
    Scene::Update();
    return false;
}

bool ChannelScene::Render()
{
    Scene::Render();
    m_pRoomMakeUI->Render();
    //RenderRoomUIs();
    
    RenderPlayerInfo();
    
    return false;
}

bool ChannelScene::Release()
{
    Scene::Release();
    SceneObjRelease();

    if (m_pBackgroundMusic != nullptr)
    {
        m_pBackgroundMusic->Release();
        SAFE_DELETE(m_pBackgroundMusic);
    }
    for (auto iter : m_roomMap)
    {
        iter.second->Release();
        SAFE_DELETE(iter.second);
    }
    m_roomMap.clear();
    //SAFE_DELETE(m_pPlayer);
    if(m_pRoomMakeUI != nullptr)
        m_pRoomMakeUI->Release();
    SAFE_DELETE(m_pRoomMakeUI);
    return true;
}

bool ChannelScene::CreateBackgroundImage()
{
    m_pBackgroundImage = new Object2D();
    m_pBackgroundImage->Init();
    m_pBackgroundImage->SetRectDraw({ 0,0,g_clientRect.right,g_clientRect.bottom });
    m_pBackgroundImage->SetPosition(Vector2(g_clientRect.right / 2.0f, g_clientRect.bottom / 2.0f));
    m_pBackgroundImage->SetCollision(CT_IGNORE, ST_IGNORE, ST_CHANNEL);
    if (!m_pBackgroundImage->Create(g_pd3dDevice, g_pContext,
        L"../../data/Shader/NoBlendVS.txt",
        L"../../data/Shader/NoBlendPS.txt",
        L"../../data/omok/Channel/Loby_Back.jpg"
    ))
    {
        return false;
    }
    m_backObjList.insert(std::make_pair(GetObjCount(), m_pBackgroundImage));
    return true;
}

bool ChannelScene::CreateRoomMakeUI()
{
    m_pRoomMakeUI = new RoomMakeUI(this);
    m_pRoomMakeUI->Init();
    m_pRoomMakeUI->SetRectSource({ 0,0,156,63 });
    m_pRoomMakeUI->SetRectDraw({ 0,0,240,80 }); 
    m_pRoomMakeUI->SetPosition(Vector2(g_clientRect.right / 2.0f  + 140.0f, g_clientRect.bottom / 2.0f + 280.0f)); // 800 450
    m_pRoomMakeUI->SetCollision(CT_IGNORE, ST_OVERLAP, ST_CHANNEL);
    if (!m_pRoomMakeUI->Create(g_pd3dDevice, g_pContext,
        L"../../data/Shader/VertexShader.txt",
        L"../../data/Shader/PixelShader.txt",
        L"../../data/omok/Channel/Loby_CreateRoom.bmp",
        L"../../data/omok/Channel/Loby_CreateRoom-mask.bmp"
    ))
    {
        return false;
    }
    //m_UIObjList.insert(std::make_pair(GetObjCount(), m_pRoomMakeUI));
    return true;
}

bool ChannelScene::CreateQuickStartUI()
{
    m_pQuickStartUI = new QuickStartUI();
    m_pQuickStartUI->Init();
    m_pQuickStartUI->SetRectSource({ 0,0,156,63 });
    m_pQuickStartUI->SetRectDraw({ 0,0,240,80 });
    m_pQuickStartUI->SetPosition(Vector2(g_clientRect.right / 2.0f + 140.0f, g_clientRect.bottom / 2.0f + 380.0f)); // 800 450
    m_pQuickStartUI->SetCollision(CT_IGNORE, ST_OVERLAP, ST_CHANNEL);
    if (!m_pQuickStartUI->Create(g_pd3dDevice, g_pContext,
        L"../../data/Shader/VertexShader.txt",
        L"../../data/Shader/PixelShader.txt",
        L"../../data/omok/Channel/Loby_QuickStart.bmp",
        L"../../data/omok/Channel/Loby_CreateRoom-mask.bmp"
    ))
    {
        return false;
    }
    m_UIObjList.insert(std::make_pair(GetObjCount(), m_pQuickStartUI));
    return true;
}

bool ChannelScene::CreatePlayerImage()
{
    m_pPlayerImage = new Object2D();
    m_pPlayerImage->Init();
    m_pPlayerImage->SetRectSource({ 0,0,180,176 });
    m_pPlayerImage->SetRectDraw({ 0,0,180,176 });
    m_pPlayerImage->SetPosition(Vector2(g_clientRect.right / 2.0f + 390.0f , g_clientRect.bottom / 2.0f + 330.0f));
    m_pPlayerImage->SetCollision(CT_IGNORE, ST_IGNORE, ST_CHANNEL);
    if (!m_pPlayerImage->Create(g_pd3dDevice, g_pContext,
        L"../../data/Shader/VertexShader.txt",
        L"../../data/Shader/PixelShader.txt",
        L"../../data/omok/Channel/[크기변환]Player.bmp",
        L"../../data/omok/Channel/[크기변환]Player-mask.bmp"
    ))
    {
        return false;
    }
    m_UIObjList.insert(std::make_pair(GetObjCount(), m_pPlayerImage));
    return true;
}

bool ChannelScene::CreateRoom(std::wstring& hostName, std::wstring& titleName, uInt32 roomIndex, uInt32 playerCountInRoom, bool isPlayGame)
{
    //DWORD roomIndex = m_pUI->GetRoomCountAndInc();
    float roomLocation = 210.0f + (roomIndex * 40.0f);
    Vector2 pos = Vector2(538.0f, roomLocation);

    RoomUI* pRoom;
    
    pRoom = new RoomUI(hostName,titleName, roomIndex, playerCountInRoom,isPlayGame);
    pRoom->SetPosition(pos);
    pRoom->SetRectSource({ 0,0,996,35 });
    pRoom->SetRectDraw({ 0,0,996,35 });
    pRoom->Init();
    // 초기위치. 538.0 210.0 증가 40
    pRoom->SetCollision(CT_IGNORE, ST_OVERLAP, ST_CHANNEL);
    if (!pRoom->Create(g_pd3dDevice, g_pContext,
        L"../../data/Shader/NoBlendVS.txt",
        L"../../data/Shader/NoBlendPS.txt",
        L"../../data/omok/Channel/RoomOk.bmp"
    ))
    {
        return false;
    }
    //m_UIObjList.insert(std::make_pair(findRoomNum, pRoom))
    pRoom->SetWaitTexture(pRoom->GetTexture());
    if (isPlayGame)
    {
        pRoom->SetTexture(pRoom->GetPlayTexture());
    }
    uInt32 num = GetObjCount();
    pRoom->SetObjNum(num);
    m_UIObjList.insert(std::make_pair(num, pRoom));
    m_roomMap.insert(std::make_pair(titleName, pRoom));
    return true;
}

void ChannelScene::DeleteRoom(std::wstring& titleName)
{
    auto iter = m_roomMap.find(titleName);
    RoomUI* pRoom = iter->second;
    
    uInt32 myRoomIndex = pRoom->GetRoomIndex();
    for (auto iter : m_roomMap)
    {
        RoomUI* pFindRoom = iter.second;
        DWORD index = pFindRoom->GetRoomIndex();
        if (index > myRoomIndex)
            pFindRoom->SetRoomIndex(index - 1);
    }
    m_roomMap.erase(titleName);
    ObjectManager::GetInstance().DeleteCollisionExecute(pRoom, ST_CHANNEL);
    ObjectManager::GetInstance().DeleteSelectExecute(pRoom, ST_CHANNEL);
    pRoom->Release();
    SAFE_DELETE(pRoom);
    
}

//밑 함수 삭제예정.
void ChannelScene::CreateRoomUIs(uInt32 roomCount,std::vector<std::wstring>& hosts, std::vector<std::wstring>& titles, std::vector<uInt32>& roomIndices, std::vector<uInt32>& playerCountInRoom, std::vector<uInt32>& roomState)
{
    for (uInt32 create = 0; create < roomCount; ++create)
    {

        CreateRoom(hosts[create], titles[create], roomIndices[create], playerCountInRoom[create], roomState[create]);
    }
}

void ChannelScene::RenderRoomUIs()
{
    for (auto roomUI : m_roomMap)
        roomUI.second->Render();
}

void ChannelScene::SetInfo(Player* pPlayer)
{
    m_playerInfoBuffer[PI_TOTAL].clear();
    m_playerInfoBuffer[PI_WIN].clear();
    m_playerInfoBuffer[PI_LOSE].clear();
    m_playerInfoBuffer[PI_RATE].clear();

    m_playerInfoBuffer[PI_TOTAL] += (L"총 플레이 : " + std::to_wstring(pPlayer->GetPlayerInfo().m_totalPlayGameCount));
    m_playerInfoBuffer[PI_WIN] += (L"승리 : " +  std::to_wstring(pPlayer->GetPlayerInfo().m_winGameCount));
    m_playerInfoBuffer[PI_LOSE] += (L"패배 : " +  std::to_wstring(pPlayer->GetPlayerInfo().m_loseGameCount));
    m_playerInfoBuffer[PI_RATE] += (L"승리 비율 : " + std::to_wstring(pPlayer->GetPlayerInfo().m_winRate));
}

void ChannelScene::CreateInfo()
{

    m_playerInfoBuffer[PI_ID].reserve(30);
    UserInfo info = m_pPlayer->GetPlayerInfo();
    m_playerInfoBuffer[PI_ID] += (L"닉네임 : " + info.m_name);
    for (uInt32 buf = 1; buf < MAX_INFO; ++buf)
    {
        m_playerInfoBuffer[buf].reserve(20);
    }
    m_playerInfoBuffer[PI_TOTAL] += (L"총 플레이 : "  + std::to_wstring(info.m_totalPlayGameCount));
    m_playerInfoBuffer[PI_WIN]   += (L"승리 : "      + std::to_wstring(info.m_winGameCount));
    m_playerInfoBuffer[PI_LOSE]  += (L"패배 : "      + std::to_wstring(info.m_loseGameCount));
    m_playerInfoBuffer[PI_RATE]  += (L"승리 비율 : "  +  std::to_wstring(info.m_winRate));


    Vector2 pos = m_pPlayerImage->m_pos;
    LONG posX = static_cast<LONG>(pos.x);
    LONG posY = static_cast<LONG>(pos.y);
    m_playerInfoRect[PI_ID] = { posX + 120,posY - 55,g_clientRect.right,g_clientRect.bottom };
    m_playerInfoRect[PI_TOTAL] = { posX + 120,posY - 25,g_clientRect.right,g_clientRect.bottom };
    m_playerInfoRect[PI_WIN] = { posX + 120,posY + 5,g_clientRect.right,g_clientRect.bottom };
    m_playerInfoRect[PI_LOSE] = { posX + 120,posY + 35,g_clientRect.right,g_clientRect.bottom };
    m_playerInfoRect[PI_RATE] = { posX + 120,posY + 65,g_clientRect.right,g_clientRect.bottom };
}

void ChannelScene::RenderPlayerInfo()
{

    g_wt->Draw(m_playerInfoBuffer[PI_ID]     , m_playerInfoRect[PI_ID], D2D1::ColorF(0, 0, 0, 1));
    g_wt->Draw(m_playerInfoBuffer[PI_TOTAL]  , m_playerInfoRect[PI_TOTAL], D2D1::ColorF(0, 0, 0, 1));
    g_wt->Draw(m_playerInfoBuffer[PI_WIN]    , m_playerInfoRect[PI_WIN], D2D1::ColorF(0, 0, 0, 1));
    g_wt->Draw(m_playerInfoBuffer[PI_LOSE]   , m_playerInfoRect[PI_LOSE], D2D1::ColorF(0, 0, 0, 1));
    g_wt->Draw(m_playerInfoBuffer[PI_RATE]   , m_playerInfoRect[PI_RATE], D2D1::ColorF(0, 0, 0, 1));
}

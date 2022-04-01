#include "GameScene.h"
#include "GameDlg.h"
#include "OmokGround.h"
#include "Sound.h"
#include "Player.h"
#include "StartBtn.h"
#include "ReadyBtn.h"
#include "ExitBtn.h"
#include "ChannelScene.h"
#include "ClientServer.h"
GameScene::GameScene() : m_pOmokGround(nullptr),m_pGamedlg(nullptr),m_pBackGroundMusic(nullptr),
    m_pHostPlayer(nullptr),m_pEnterPlayer(nullptr), m_isHost(false),m_pStartBtn(nullptr),m_pReadyBtn(nullptr),m_pExitBtn(nullptr),
    m_isGameStart(false), m_pHostPlayerImage(nullptr),m_pEnterPlayerImage(nullptr),m_pReadyImage(nullptr), m_isReady(false), m_isMyTurn(false),
    m_pBlackStoneImage(nullptr), m_pWhiteStoneImage(nullptr), m_isChangeStone(false)
{
    m_titleName.reserve(20);
}
GameScene::~GameScene()
{
}
bool GameScene::Load(std::wstring file)
{
    return true;
}

bool GameScene::Init()
{
    Scene::Init();
    CreateOmokDistance();
    if (!CreateGameDlg())
        return false;
    if (!CreateOmokGround())
        return false;
    if (!CreateBtn())
        return false;
    if (!CreatePlayerImage())
        return false;
    if (!CreatePlayerStoneImage())
        return false;
    if (m_isHost)
        m_isMyTurn = true;
    
    return true;
}

bool GameScene::Update()
{
    Scene::Update();
    return true;
}

bool GameScene::Render()
{
    Scene::Render();
    if(m_isHost)
        m_pStartBtn->Render();
    else
        m_pReadyBtn->Render();
    m_pExitBtn->Render();

    RenderPlayerImage();

    if(m_isReady)
        m_pReadyImage->Render();

    
    for (uInt32 stone = 0; stone < m_pOmokGround->m_stones.size(); ++stone)
    {
        m_pOmokGround->m_stones[stone]->Render();
    }

    return true;
}

bool GameScene::Release()
{
    Scene::Release();
    SceneObjRelease();
    if (m_pBackGroundMusic != nullptr)
    {
        m_pBackGroundMusic->Release();
        SAFE_DELETE(m_pBackGroundMusic);
    }
    if(m_pStartBtn != nullptr)
        m_pStartBtn->Release();
    SAFE_DELETE(m_pStartBtn);
    if (m_pReadyBtn != nullptr)
        m_pReadyBtn->Release();
    SAFE_DELETE(m_pReadyBtn);
    if (m_pExitBtn != nullptr)
        m_pExitBtn->Release();
    SAFE_DELETE(m_pExitBtn);
    if(m_pHostPlayerImage != nullptr)
        m_pHostPlayerImage->Release();
    SAFE_DELETE(m_pHostPlayerImage);
    if(m_pEnterPlayerImage != nullptr)
        m_pEnterPlayerImage->Release();
    SAFE_DELETE(m_pEnterPlayerImage);
    if (m_pReadyImage != nullptr)
        m_pReadyImage->Release();
    SAFE_DELETE(m_pReadyImage);
    if (m_pBlackStoneImage != nullptr)
        m_pBlackStoneImage->Release();
    SAFE_DELETE(m_pBlackStoneImage);
    if (m_pWhiteStoneImage != nullptr)
        m_pWhiteStoneImage->Release();
    SAFE_DELETE(m_pWhiteStoneImage);



    return true;
}
void GameScene::ReSetCollision()
{
    
    m_pOmokGround->SetCollision(CT_IGNORE, ST_OVERLAP, ST_GAME);
    m_pStartBtn->SetCollision(CT_IGNORE, ST_OVERLAP, ST_GAME);
    m_pReadyBtn->SetCollision(CT_IGNORE, ST_OVERLAP, ST_GAME);
    m_pExitBtn->SetCollision(CT_IGNORE, ST_OVERLAP, ST_GAME);
}

void GameScene::PutStone(StoneType type,uInt32 posY, uInt32 posX)
{
    m_omokPair[posY][posX].first = true;


    Stone* stone = new Stone();
    if (m_isHost)
        stone = stone->CreateStone(type, m_omokPair[posY][posX].second.x, m_omokPair[posY][posX].second.y);
    else
        stone = stone->CreateStone(type, m_omokPair[posY][posX].second.x, m_omokPair[posY][posX].second.y);

    m_pOmokGround->m_stones.push_back(stone);
}

void GameScene::ClearOmok()
{
    for (uInt32 clearY = 0; clearY < MAX_Y; ++clearY)
    {
        for (uInt32 clearX = 0; clearX < MAX_X; ++clearX)
            m_omokPair[clearY][clearX].first = false;
    }
}


bool GameScene::CreateGameDlg()
{
    m_pGamedlg = new GameDlg();

    m_pGamedlg->Init();
    m_pGamedlg->SetRectDraw({ 0,0,g_clientRect.right,g_clientRect.bottom }); // 720 720
    m_pGamedlg->SetPosition(Vector2(g_clientRect.right / 2.0f, g_clientRect.bottom / 2.0f));
    //m_pGamedlg->SetCollision(CT_IGNORE, ST_IGNORE, ST_GAME);
    if (!m_pGamedlg->Create
    (
        g_pd3dDevice, g_pContext,
        L"../../data/Shader/NoBlendVS.txt",
        L"../../data/Shader/NoBlendPS.txt",
        L"../../data/omok/OmokGround/GameDlg.bmp"
    ))
    {
        return false;
    }
    m_UIObjList.insert(std::make_pair(GetObjCount(), m_pGamedlg));
    return true;
}

bool GameScene::CreateOmokGround()
{
    m_pOmokGround = new OmokGround(this);
    m_pOmokGround->Init();
    m_pOmokGround->SetRectDraw({ 0,0,1100,824 }); // 740 660
    m_pOmokGround->SetPosition(Vector2(550.0f, 412.0f)); // 370 330
    //m_pOmokGround->SetCollision(CT_IGNORE, ST_OVERLAP, ST_GAME);
    if (!m_pOmokGround->Create(g_pd3dDevice, g_pContext,
        L"../../data/Shader/NoBlendVS.txt",
        L"../../data/Shader/NoBlendPS.txt",
        L"../../data/omok/OmokGround/OmokGround2.bmp"
    ))
    {
        return false;
    }

    m_UIObjList.insert(std::make_pair(GetObjCount(), m_pOmokGround));
    return true;
}

bool GameScene::CreateBtn()
{
    m_pStartBtn = new StartBtn();
    m_pStartBtn->Init();
    m_pStartBtn->SetRectDraw({ 0,0,90,94 }); // 740 660
    m_pStartBtn->SetPosition(Vector2(g_clientRect.right / 2.0f + 450.0f, g_clientRect.bottom / 2.0f)); // 370 330
    //m_pStartBtn->SetCollision(CT_IGNORE, ST_OVERLAP, ST_GAME);
    if (!m_pStartBtn->Create(g_pd3dDevice, g_pContext,
        L"../../data/Shader/NoBlendVS.txt",
        L"../../data/Shader/NoBlendPS.txt",
        L"../../data/omok/OmokGround/Btn/StartBtn.bmp"
    ))
    {
        return false;
    }
    
    m_pReadyBtn = new ReadyBtn();
    m_pReadyBtn->Init();
    m_pReadyBtn->SetRectDraw({ 0,0,90,94 }); // 740 660
    m_pReadyBtn->SetPosition(Vector2(g_clientRect.right / 2.0f + 450.0f, g_clientRect.bottom / 2.0f)); // 370 330
    //m_pReadyBtn->SetCollision(CT_IGNORE, ST_OVERLAP, ST_GAME);
    if (!m_pReadyBtn->Create(g_pd3dDevice, g_pContext,
        L"../../data/Shader/NoBlendVS.txt",
        L"../../data/Shader/NoBlendPS.txt",
        L"../../data/omok/OmokGround/Btn/ReadyBtn.bmp"
    ))
    {
        return false;
    }


    m_pExitBtn = new ExitBtn();
    m_pExitBtn->Init();
    m_pExitBtn->SetRectDraw({ 0,0,90,94 }); // 740 660
    m_pExitBtn->SetPosition(Vector2(g_clientRect.right / 2.0f + 660.0f, g_clientRect.bottom / 2.0f)); // 370 330
    //m_pExitBtn->SetCollision(CT_IGNORE, ST_OVERLAP, ST_GAME);
    if (!m_pExitBtn->Create(g_pd3dDevice, g_pContext,
        L"../../data/Shader/NoBlendVS.txt",
        L"../../data/Shader/NoBlendPS.txt",
        L"../../data/omok/OmokGround/Btn/ExitBtn.bmp"
    ))
    {
        return false;
    }

    return true;
}

bool GameScene::CreatePlayerImage()
{
    m_pHostPlayerImage = new Object2D();
    m_pHostPlayerImage->Init();
    m_pHostPlayerImage->SetRectDraw({ 0,0,194,190 }); // 740 660
    m_pHostPlayerImage->SetPosition(Vector2(g_clientRect.right / 2.0f + 432.8f, g_clientRect.bottom / 2.0f - 267.3f)); // 370 330
    //m_pHostPlayerImage->SetCollision(CT_IGNORE, ST_IGNORE, ST_GAME);
    if (!m_pHostPlayerImage->Create(g_pd3dDevice, g_pContext,
        L"../../data/Shader/NoBlendVS.txt",
        L"../../data/Shader/NoBlendPS.txt",
        L"../../data/omok/OmokGround/Player.bmp"
    ))
    {
        return false;
    }
    m_pEnterPlayerImage = new Object2D();
    m_pEnterPlayerImage->Init();
    m_pEnterPlayerImage->SetRectDraw({ 0,0,194,190 }); // 740 660
    m_pEnterPlayerImage->SetPosition(Vector2(g_clientRect.right / 2.0f + 668.0f, g_clientRect.bottom / 2.0f - 267.3f)); // 370 330
    //m_pEnterPlayerImage->SetCollision(CT_IGNORE, ST_IGNORE, ST_GAME);
    if (!m_pEnterPlayerImage->Create(g_pd3dDevice, g_pContext,
        L"../../data/Shader/NoBlendVS.txt",
        L"../../data/Shader/NoBlendPS.txt",
        L"../../data/omok/OmokGround/Player.bmp"
    ))
    {
        return false;
    }
    
    m_pReadyImage = new Object2D();
    m_pReadyImage->Init();
    m_pReadyImage->SetRectSource({ 0,0,375,204 });
    m_pReadyImage->SetRectDraw({ 0,0,340,170 }); // 740 660
    m_pReadyImage->SetPosition(Vector2(g_clientRect.right / 2.0f + 670.0f, g_clientRect.bottom / 2.0f - 120.0f)); // 370 330
    //m_pReadyImage->SetCollision(CT_IGNORE, ST_IGNORE, ST_GAME);
    if (!m_pReadyImage->Create(g_pd3dDevice, g_pContext,
        L"../../data/Shader/VertexShader.txt",
        L"../../data/Shader/PixelShader.txt",
        L"../../data/omok/OmokGround/ReadyImage.bmp",
        L"../../data/omok/OmokGround/ReadyImage-mask.bmp"
    ))
    {
        return false;
    }
    return true;
}



void GameScene::CreateOmokDistance()
{
    float startX = 63.0f;
    float startY = 49.0f;
    float conX, conY;
    for (int y = 0; y < MAX_Y; ++y)
    {
        conX = startX;
        conY = startY + (40.4f * y);
        for (int x = 0; x < MAX_Y; ++x)
        {
            m_omokPair[y][x] = std::make_pair(false, Vector2(conX, conY));
            conX += 54.0f;
        }

    }
}

bool GameScene::CreatePlayerStoneImage()
{
    m_pBlackStoneImage = new Object2D();

    m_pBlackStoneImage->Init();
    m_pBlackStoneImage->SetRectSource({ 0,0,50,37 });
    m_pBlackStoneImage->SetRectDraw({ 0,0,50,37 });
    m_pBlackStoneImage->SetPosition(Vector2(m_pReadyImage->m_pos.x - 143.0F, m_pReadyImage->m_pos.y - 45.0f)); // 143 45
    //m_pBlackStoneImage->SetCollision(CT_IGNORE, ST_IGNORE, ST_GAME);
    if (!m_pBlackStoneImage->Create
    (
        g_pd3dDevice, g_pContext,
        L"../../data/Shader/VertexShader.txt",
        L"../../data/Shader/PixelShader.txt",
        L"../../data/omok/Stone/blackStone.bmp",
        L"../../data/omok/Stone/mask2.bmp"
    ))
    {
        return false;
    }
    

    m_pWhiteStoneImage = new Object2D();

    m_pWhiteStoneImage->Init();
    m_pWhiteStoneImage->SetRectSource({ 0,0,50,37 });
    m_pWhiteStoneImage->SetRectDraw({ 0,0,50,37 });
    m_pWhiteStoneImage->SetPosition(Vector2(m_pReadyImage->m_pos.x + 90.0F, m_pReadyImage->m_pos.y - 45.0f));
    //m_pWhiteStoneImage->SetCollision(CT_IGNORE, ST_IGNORE, ST_GAME);
    if (!m_pWhiteStoneImage->Create
    (
        g_pd3dDevice, g_pContext,
        L"../../data/Shader/VertexShader.txt",
        L"../../data/Shader/PixelShader.txt",
        L"../../data/omok/Stone/WhiteStone.bmp",
        L"../../data/omok/Stone/mask2.bmp"
    ))
    {
        return false;
    }
    return true;
}

void GameScene::RenderPlayerImage()
{
    if (m_pHostPlayer != nullptr)
    {
        m_pHostPlayerImage->Render();
        if (!m_isChangeStone)
            m_pBlackStoneImage->Render();
        else
            m_pWhiteStoneImage->Render();
    }
    if (m_pEnterPlayer != nullptr)
    {
        m_pEnterPlayerImage->Render();

        if (!m_isChangeStone)
            m_pWhiteStoneImage->Render();
        else
            m_pBlackStoneImage->Render();
    }

    
}

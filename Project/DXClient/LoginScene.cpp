#include "LoginScene.h"
#include "Object2D.h"
#include "LoginUI.h"
#include "Sound.h"
#include "Input.h"
LoginScene::LoginScene() : m_pBackgroundMusic(nullptr), m_pBackgroundImage(nullptr),m_pLoginUI(nullptr)
{
}
LoginScene::~LoginScene()
{
    int test = 10;
}
bool LoginScene::Load(std::wstring file)
{
    return true;
}

bool LoginScene::Init()
{
    Scene::Init();
   //Object2D* backObj = new Object2D();
    if (!CreateBackgroundImage())
        return false;
    if (!CreateLoginUI())
        return false;


    return true;
}

bool LoginScene::Update()
{
    Scene::Update();
    return true;
}

bool LoginScene::Render()
{
    Scene::Render();

    return true;
}

bool LoginScene::Release()
{
    Scene::Release();
    SceneObjRelease();
    
    if (m_pBackgroundMusic != nullptr)
    {
        m_pBackgroundMusic->Release();
        SAFE_DELETE(m_pBackgroundMusic);
    }

    return true;
}

bool LoginScene::CreateBackgroundImage()
{
    m_pBackgroundImage = new Object2D();
    m_pBackgroundImage->Init();
    m_pBackgroundImage->SetRectDraw({ 0,0,g_clientRect.right,g_clientRect.bottom });
    m_pBackgroundImage->SetPosition(Vector2(g_clientRect.right / 2.0f, g_clientRect.bottom / 2.0f));
    m_pBackgroundImage->SetCollision(CT_IGNORE, ST_IGNORE, ST_LOGIN);
    if (!m_pBackgroundImage->Create(g_pd3dDevice, g_pContext,
        L"../../data/Shader/NoBlendVS.txt",
        L"../../data/Shader/NoBlendPS.txt",
        L"../../data/omok/Login/LoginBackgroundImage.bmp"
    ))
    {
        return false;
    }
    m_backObjList.insert(std::make_pair(GetObjCount(), m_pBackgroundImage));
    return true;
}

bool LoginScene::CreateLoginUI()
{
    //357 90
    m_pLoginUI = new LoginUI();
    m_pLoginUI->SetPosition(Vector2(g_clientRect.right / 2.0f, g_clientRect.bottom / 2.0f + 100.0f));
    m_pLoginUI->Init();
    m_pLoginUI->SetRectSource({ 357,90,495,469 });
    m_pLoginUI->SetRectDraw({ 0,0,400,380 });
    m_pLoginUI->SetCollision(CT_IGNORE, ST_OVERLAP, ST_LOGIN);
    if (!m_pLoginUI->Create(g_pd3dDevice, g_pContext,
        L"../../data/Shader/VertexShader.txt",
        L"../../data/Shader/PixelShader.txt",
        L"../../data/omok/Login/loginUI2.bmp",
        L"../../data/omok/Login/loginUI-mask.bmp"
    ))
    {
        return false;
    }
    m_UIObjList.insert(std::make_pair(GetObjCount(), m_pLoginUI));
    return true;
}

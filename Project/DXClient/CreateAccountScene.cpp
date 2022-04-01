#include "CreateAccountScene.h"
#include "Sound.h"
#include "SignUpUI.h"
CreateAccountScene::CreateAccountScene() :m_pBackgroundImage(nullptr),m_pBackGroundMusic(nullptr), m_pSignupUI(nullptr)
{
}
CreateAccountScene::~CreateAccountScene()
{
}
bool CreateAccountScene::Load(std::wstring file)
{
	return true;
}

bool CreateAccountScene::Init()
{
    Scene::Init();
    if (!CreateBackgroundImage())
        return false;
    if (!CreateSignUpUI())
        return false;
	return true;
}

bool CreateAccountScene::Update()
{
    Scene::Update();
	return true;
}

bool CreateAccountScene::Render()
{
    Scene::Render();
	return true;
}

bool CreateAccountScene::Release()
{
    Scene::Release();
	SceneObjRelease();
	if (m_pBackGroundMusic != nullptr)
	{
		m_pBackGroundMusic->Release();
		SAFE_DELETE(m_pBackGroundMusic);
	}
	return false;
}

bool CreateAccountScene::CreateBackgroundImage()
{
    m_pBackgroundImage = new Object2D();
    m_pBackgroundImage->Init();
    m_pBackgroundImage->SetRectDraw({ 0,0,g_clientRect.right,g_clientRect.bottom });
    m_pBackgroundImage->SetPosition(Vector2(g_clientRect.right / 2.0f, g_clientRect.bottom / 2.0f));
    m_pBackgroundImage->SetCollision(CT_IGNORE, ST_IGNORE, ST_ACCOUNT);
    if (!m_pBackgroundImage->Create(g_pd3dDevice, g_pContext,
        L"../../data/Shader/NoBlendVS.txt",
        L"../../data/Shader/NoBlendPS.txt",
        L"../../data/omok/Login/LoginBackgroundImage.bmp"
    ))
    {
        return false;
    }
    DWORD index = GetObjCount();
    m_backObjList.insert(std::make_pair(index, m_pBackgroundImage));
    return true;
}

bool CreateAccountScene::CreateSignUpUI()
{
    m_pSignupUI = new SignUpUI();
    m_pSignupUI->SetPosition(Vector2(g_clientRect.right / 2.0f, g_clientRect.bottom / 2.0f + 100));
    m_pSignupUI->Init();
    // 641 908
    m_pSignupUI->SetRectSource({ 0,0,1000,1000 });
    m_pSignupUI->SetRectDraw({ 0,0,1000,1000 });
    m_pSignupUI->SetCollision(CT_IGNORE, ST_OVERLAP, ST_ACCOUNT);
    if (!m_pSignupUI->Create(g_pd3dDevice, g_pContext,
        L"../../data/Shader/VertexShader.txt",
        L"../../data/Shader/PixelShader.txt",
        L"../../data/omok/account/signup.bmp",
        L"../../data/omok/account/signup-mask.bmp"
    ))
    {
        return false;
    }
    DWORD index = GetObjCount();
    m_UIObjList.insert(std::make_pair(index, m_pSignupUI));
    return true;
}

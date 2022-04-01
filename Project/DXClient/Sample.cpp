#include "Sample.h"
#include "SoundManager.h"
#include "Sound.h"
#include "ChannelScene.h"
#include "CreateAccountScene.h"
#include "LoginScene.h"
#include "GameScene.h"
#include <time.h>
#include "ClientServer.h"
#include "Input.h"
#include "PlayerManager.h"
#include "AsyncIO.h"
#include <crtdbg.h>
#include <stdlib.h>
#define _CRTDB_MAP_ALLOC

Sample::Sample()
{
}
Sample::~Sample()
{
	
}

BOOL Sample::Init()
{

	/*
	DWORD style = WS_CHILD | WS_VISIBLE | ES_MULTILINE;
	m_hEdit = CreateWindow(L"edit", NULL, style,
		0, g_clientRect.bottom - 50, 300, 50,
		m_hWindowHandle, (HMENU)100, m_hInst, NULL);
	style = WS_CHILD | WS_VISIBLE;
	m_hButton = CreateWindow(L"button", L"傈价", style,
		310, g_clientRect.bottom - 50, 50, 50,
		m_hWindowHandle, (HMENU)200, m_hInst, NULL);
	m_hListBox = CreateWindow(L"listbox", NULL, style,
		0, 0, 300, g_clientRect.bottom - 70,
		m_hWindowHandle, (HMENU)300, m_hInst, NULL);*/

	ClientServer::GetInstance()->Init();


	//m_pChannelScene = new ChannelScene();
	//Scene::m_pScene = m_pChannelScene;
	m_pLoginScene = new LoginScene();
	Scene::m_pScene = m_pLoginScene;
	//SAFE_DELETE(Scene::m_pScene);
	//m_pGameScene = new GameScene();
	//Scene::m_pScene = m_pGameScene;
	Scene::m_pScene->Init();

	//m_zone.Init();
	//m_zone.Load(L"aa");
	return TRUE;
}
BOOL Sample::Update() 
{
	
	Scene::m_pScene->Update();
	ClientServer::GetInstance()->Update();

	return TRUE; 
}

BOOL Sample::Render() 
{
	Scene::m_pScene->Render();



	std::wstring msg = L"FPS:";
	msg += std::to_wstring(m_timer.GetFPS());
	msg += L"  GT:";
	msg += std::to_wstring(g_gameTime);
	
	m_write.Draw(msg, g_clientRect, D2D1::ColorF(0, 0, 1, 1));


	
	return TRUE; 
}
BOOL Sample::Release() 
{

	PlayerManager::GetInstance()->DeleteScene();
	PlayerManager::GetInstance()->ReleaseInstance();
	//Scene::m_pScene->Release();
	//if (Scene::m_pScene != Scene::m_pPrevScene)
	//	SAFE_DELETE(Scene::m_pPrevScene);
	//SAFE_DELETE(Scene::m_pScene);

	ReleaseScene();
	ClientServer::GetInstance()->Release();
	AsyncIO::GetInstance()->DeleteInstance();
	_CrtDumpMemoryLeaks();
	return TRUE;
}

LRESULT Sample::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) // 目盖靛贸府.
{
	return AsyncIO::GetInstance()->MsgProc(hWnd,msg,wParam,lParam);
}

void Sample::ReleaseScene()
{
	if (!ClientServer::GetInstance()->GetIsLogin())
	{
		Scene::m_pScene->Release();
		SAFE_DELETE(Scene::m_pScene);
		return;
	}


	for (uInt32 del = 0; del < Scene::m_delScene.size(); ++del)
	{
		Scene::m_delScene[del]->Release();
		Scene::m_delScene[del]->SceneObjRelease();
		SAFE_DELETE(Scene::m_delScene[del]);
	}
	if (Scene::m_pScene != Scene::m_pPrevScene && Scene::m_pPrevScene != nullptr)
	{
		Scene::m_pPrevScene->Release();
		SAFE_DELETE(Scene::m_pPrevScene);
	}
	if (ClientServer::GetInstance()->GetIsLogin() && Scene::m_pScene != nullptr)
	{
		Scene::m_pScene->Release();
		delete Scene::m_pScene;
	}
	
	
}

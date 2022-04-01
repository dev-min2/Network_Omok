#pragma once
#include "Core.h"
#include "DXObject.h"
class Sound;
class LoginScene;
class LobbyScene;
class ChannelScene;
class GameScene;
class CreateAccountScene;
#define MAX_BUCKET 1024



class Sample : public Core
{
public:
	Sample();
	virtual ~Sample() override;
private:

	Sound*				m_pBackMusic;
	ChannelScene*		m_pChannelScene;
	CreateAccountScene* m_pAccountScene;
	LoginScene*			m_pLoginScene;
	GameScene*			m_pGameScene;
public:
	virtual BOOL		Init();
	virtual BOOL		Update();
	virtual BOOL		Render();
	virtual BOOL		Release();
	virtual LRESULT		MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	void				ReleaseScene();
};



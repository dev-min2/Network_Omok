#pragma once
#include "std.h"
#define MAX_KEY 256
#define MAX_MOUSE 3
enum KeyState
{
	KS_FREE =0,
	KS_UP,
	KS_DOWN,
	KS_HOLD
};

class Input
{
public:
	static Input* GetInstance();
public:
	bool		  Init();
	bool		  Update();
	bool		  Render();
	bool		  Release();
	DWORD		  GetKey(DWORD key);
	bool		  GetPressedKey(DWORD key);
	bool		  GetHoldKey(DWORD key);
	void		  RefreshKey(DWORD key);
	DWORD		  GetMouseLButtonState() { return m_mouseState[0]; }
	bool		  PressKey();
private:
	DWORD				m_getKey;
	DWORD				m_keyState[MAX_KEY];
	DWORD				m_mouseState[MAX_MOUSE];
	static Input*		m_instance;
	POINT				m_mouse;
	bool				m_pressed;
private:
	Input();
	static void	  ReleaseInstance(); // Release에서 호출하게끔.
public:
	~Input() = default;


};


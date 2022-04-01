#pragma once
#include "DXDevice.h"
class Window : public DXDevice
{
protected:
	Window();
	virtual ~Window();
public:
	virtual BOOL InitWindow(HINSTANCE hInstance, _In_ int nCmdShow, int width, int height);
			BOOL WindowRun();
	virtual LRESULT  MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
protected:
	virtual BOOL GameInit()		= 0;
	virtual BOOL GameRun()		= 0;
	virtual BOOL GameRelease()  = 0;
	
private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
protected:
	BOOL		m_windowInit;
	HINSTANCE   m_hInst;
	HWND		m_hWindowHandle;
	BOOL		m_bGameRun;
	RECT		m_clientRect;
	LONG		m_clientWidth;
	LONG		m_clientHeight;
	const wchar_t*	m_windowClassName = nullptr;
};


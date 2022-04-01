#pragma once
#include "Window.h"
#include "Timer.h"
#include "DXWrite.h"
class Core : public Window
{
	class Window;
public:
	Core();
	virtual ~Core();
private:
	BOOL	GameInit()		override;
	BOOL	GameRun()		override;
	BOOL    GameRelease()	override;
	// Run에 frame,release를 호출.
	BOOL    GameUpdate();
	BOOL	GameRender();
	
protected:
	virtual BOOL	Init();
	virtual BOOL	Update();
	virtual BOOL	PreRender();
	virtual BOOL	Render();
	virtual BOOL	PostRender();
	virtual BOOL	Release();
protected:
	Timer				m_timer;
	ID3D11SamplerState* m_pSamplerState;
	DXWrite				m_write;
};

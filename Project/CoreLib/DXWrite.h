#pragma once
#include "std.h"
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
class DXWrite
{
public:
	bool				Init();
	bool				Update();
	bool				Render();
	bool				Release();
public:
	bool				SetRenderTarget(IDXGISurface1* pSurface);
	void				Draw(std::wstring msg, RECT rt, D2D1::ColorF color, IDWriteTextFormat* tf = nullptr);
	void				DrawUserFormat(std::wstring msg, RECT rt, D2D1::ColorF color, IDWriteTextFormat* tf = nullptr);
	IDWriteTextFormat*	GetDefaultFormat() { return m_pd2dMTShadowTF; }
private:
	ID2D1Factory*			m_pd2dFactory = nullptr;
	IDWriteFactory*			m_pWriteFactory = nullptr;
	IDWriteFactory*			m_pUserWriteFactory = nullptr;

	ID2D1RenderTarget*		m_pd2dRT = nullptr;
	IDWriteTextFormat*		m_pd2dTextFormat = nullptr;
	IDWriteTextFormat*		m_pd2dUserTextFormat = nullptr;
	IDWriteTextFormat*		m_pd2dMTShadowTF = nullptr;
	ID2D1SolidColorBrush*	m_pd2dColorBrush = nullptr;
};


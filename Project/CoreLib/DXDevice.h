#pragma once
#include "std.h"
class DXDevice 
{
public:
	DXDevice() = default;
	virtual ~DXDevice();
protected:
	BOOL InitDX(HWND windHwnd, UINT width, UINT height);
	BOOL CleanupDevice();
private:
	BOOL CreateRenderTargetView();
	BOOL SetViewport();
protected:
	ID3D11Device*				m_pd3dDevice = nullptr;		// 디바이스 객체
	ID3D11DeviceContext*		m_pImmediateContext = nullptr;// 다비이스 컨텍스트 객체
	IDXGISwapChain*				m_pSwapChain = nullptr;		// 스왑체인 객체
	ID3D11RenderTargetView*		m_pRenderTargetView = nullptr;// 메인 랜더타켓 뷰
	D3D_FEATURE_LEVEL			m_featureLevel;		// Direct3D 특성레벨 속성값
	D3D_DRIVER_TYPE				m_driverType;		// 디바이스 타입 속성값
	DXGI_SWAP_CHAIN_DESC		m_swapChainDesc;	// 스왑체인 속성값
	D3D11_VIEWPORT				m_viewPort;			// 뷰 포트 속성값
};


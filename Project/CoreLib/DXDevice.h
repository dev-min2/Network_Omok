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
	ID3D11Device*				m_pd3dDevice = nullptr;		// ����̽� ��ü
	ID3D11DeviceContext*		m_pImmediateContext = nullptr;// �ٺ��̽� ���ؽ�Ʈ ��ü
	IDXGISwapChain*				m_pSwapChain = nullptr;		// ����ü�� ��ü
	ID3D11RenderTargetView*		m_pRenderTargetView = nullptr;// ���� ����Ÿ�� ��
	D3D_FEATURE_LEVEL			m_featureLevel;		// Direct3D Ư������ �Ӽ���
	D3D_DRIVER_TYPE				m_driverType;		// ����̽� Ÿ�� �Ӽ���
	DXGI_SWAP_CHAIN_DESC		m_swapChainDesc;	// ����ü�� �Ӽ���
	D3D11_VIEWPORT				m_viewPort;			// �� ��Ʈ �Ӽ���
};


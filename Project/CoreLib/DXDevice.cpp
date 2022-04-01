#include "DXDevice.h"
ID3D11Device* g_pd3dDevice;	// 디바이스 객체
ID3D11DeviceContext* g_pContext;// 다비이스 컨텍스트 객체
DXDevice::~DXDevice()
{

}

BOOL DXDevice::InitDX(HWND windHwnd, UINT width, UINT height)
{
	DXGI_SWAP_CHAIN_DESC scd;
	::ZeroMemory(&scd, sizeof(scd));
	scd.BufferCount = 1; // 백버퍼 수 1개
	scd.OutputWindow = windHwnd; // 출력할 윈도우 핸들 지정
	scd.Windowed = TRUE; // 창모드 TRUE
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//백버퍼 세팅
	scd.BufferDesc.Width = width;
	scd.BufferDesc.Height = height;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 멀티 샘플링 (최대 여러번 그리기)
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;

	UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	D3D_FEATURE_LEVEL retFeatureLevel;
	HRESULT check = ::D3D11CreateDeviceAndSwapChain
	(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		&featureLevel,
		1,
		D3D11_SDK_VERSION,
		&scd,
		&m_pSwapChain,
		&m_pd3dDevice,
		&retFeatureLevel,
		&m_pImmediateContext
	);
	g_pd3dDevice = m_pd3dDevice;
	g_pContext = m_pImmediateContext;

	// ------ ALT + ENTER 막기
	IDXGIFactory1* pFactory = NULL;
	if (SUCCEEDED(m_pSwapChain->GetParent(__uuidof(IDXGIFactory1), (void**)&pFactory)))
	{
		pFactory->MakeWindowAssociation(g_WindowHandle, DXGI_MWA_NO_ALT_ENTER);
		pFactory->Release();
	}

	if (FAILED(check))
	{
		return FALSE;
	}
	CreateRenderTargetView();
	SetViewport();

	return TRUE;
}

BOOL DXDevice::CleanupDevice()
{
	SAFE_DX_RELEASE(m_pd3dDevice);
	SAFE_DX_RELEASE(m_pImmediateContext);
	SAFE_DX_RELEASE(m_pRenderTargetView);
	SAFE_DX_RELEASE(m_pSwapChain);
	return TRUE;
}

BOOL DXDevice::CreateRenderTargetView()
{
	// 렌더타겟뷰 생성.
	D3D11_RENDER_TARGET_VIEW_DESC tvd;
	ID3D11Texture2D* backBuffer = nullptr; // 백버퍼 텍스쳐를 사용해 위의 DESC속성을 사용하여 RenderTargetView를 생성.
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer); // 0번 버퍼의(우린 한개니 0번), Texture2D를 통해 얻어옴.

	m_pd3dDevice->CreateRenderTargetView // 렌더타겟뷰 생성.
	(
		backBuffer,
		NULL,
		&m_pRenderTargetView
	);
	if (backBuffer) backBuffer->Release(); // DX에서 Get을 해서 포인터를 얻어오면 Ref값이 증가되므로 -1 감소시켜야한다.

	// OM에 RenderTarget 셋팅 렌더 타겟 뷰에 뿌려진다는 얘기는 백버퍼에 뿌린다는 말.(현재 스왑체인의 0번 백버퍼)
	// 한번 세팅되면 바꾸기전엔 계속 유지 된다.
	m_pImmediateContext->OMSetRenderTargets
	(1,
		&m_pRenderTargetView,
		NULL
	);
	return TRUE;
}

BOOL DXDevice::SetViewport()
{
	// 뷰포트 세팅
	DXGI_SWAP_CHAIN_DESC getSwapDesc;
	m_pSwapChain->GetDesc(OUT & getSwapDesc);

	m_viewPort.TopLeftX = 0;
	m_viewPort.TopLeftY = 0;
	m_viewPort.Width = getSwapDesc.BufferDesc.Width;
	m_viewPort.Height = getSwapDesc.BufferDesc.Height;
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;
	m_pImmediateContext->RSSetViewports(1, &m_viewPort);
	return TRUE;
}

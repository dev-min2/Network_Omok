#include "DXDevice.h"
ID3D11Device* g_pd3dDevice;	// ����̽� ��ü
ID3D11DeviceContext* g_pContext;// �ٺ��̽� ���ؽ�Ʈ ��ü
DXDevice::~DXDevice()
{

}

BOOL DXDevice::InitDX(HWND windHwnd, UINT width, UINT height)
{
	DXGI_SWAP_CHAIN_DESC scd;
	::ZeroMemory(&scd, sizeof(scd));
	scd.BufferCount = 1; // ����� �� 1��
	scd.OutputWindow = windHwnd; // ����� ������ �ڵ� ����
	scd.Windowed = TRUE; // â��� TRUE
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//����� ����
	scd.BufferDesc.Width = width;
	scd.BufferDesc.Height = height;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// ��Ƽ ���ø� (�ִ� ������ �׸���)
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

	// ------ ALT + ENTER ����
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
	// ����Ÿ�ٺ� ����.
	D3D11_RENDER_TARGET_VIEW_DESC tvd;
	ID3D11Texture2D* backBuffer = nullptr; // ����� �ؽ��ĸ� ����� ���� DESC�Ӽ��� ����Ͽ� RenderTargetView�� ����.
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer); // 0�� ������(�츰 �Ѱ��� 0��), Texture2D�� ���� ����.

	m_pd3dDevice->CreateRenderTargetView // ����Ÿ�ٺ� ����.
	(
		backBuffer,
		NULL,
		&m_pRenderTargetView
	);
	if (backBuffer) backBuffer->Release(); // DX���� Get�� �ؼ� �����͸� ������ Ref���� �����ǹǷ� -1 ���ҽ��Ѿ��Ѵ�.

	// OM�� RenderTarget ���� ���� Ÿ�� �信 �ѷ����ٴ� ���� ����ۿ� �Ѹ��ٴ� ��.(���� ����ü���� 0�� �����)
	// �ѹ� ���õǸ� �ٲٱ����� ��� ���� �ȴ�.
	m_pImmediateContext->OMSetRenderTargets
	(1,
		&m_pRenderTargetView,
		NULL
	);
	return TRUE;
}

BOOL DXDevice::SetViewport()
{
	// ����Ʈ ����
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

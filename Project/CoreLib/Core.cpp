#include "Core.h"
#include "Window.h"
#include "Input.h"
#include "ObjectManager.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "ShaderManager.h"
//DXWrite g_write;
DXWrite* g_wt;
Core::Core()
{
}

Core::~Core()
{
}

BOOL Core::GameInit()
{
    Input::GetInstance()->Init();
    m_timer.Init();
    DXDevice::InitDX(this->m_hWindowHandle,this->m_clientWidth, this->m_clientHeight);
    if (m_write.Init())
    {
        IDXGISurface1* pSurface = nullptr;
        HRESULT hr = m_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface1), (void**)&pSurface);
        if (SUCCEEDED(hr))
            m_write.SetRenderTarget(pSurface);
        SAFE_DX_RELEASE(pSurface);
    }
    g_wt = &m_write;
    ObjectManager::GetInstance().Init();
    SoundManager::GetInstance()->Init();
    Init();
    
    D3D11_SAMPLER_DESC sd;
    ::ZeroMemory(&sd, sizeof(D3D11_SAMPLER_DESC));
    sd.Filter   = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.MinLOD   = FLT_MAX;
    sd.MaxLOD   = FLT_MIN;
    HRESULT hr = m_pd3dDevice->CreateSamplerState(&sd,&m_pSamplerState);
    
    return TRUE;
}

BOOL Core::GameRun()
{
    if (!GameUpdate()) return false; // Update
    if (!GameRender()) return false; // Render
    return true;
}

BOOL Core::GameUpdate()
{
    m_timer.Update();
    Input::GetInstance()->Update();
    m_write.Update();
    Update();
    ObjectManager::GetInstance().Update();
    return true;
}

BOOL Core::GameRender()
{
    // 지우고 뿌리고 더블버퍼링.
    PreRender();
    Render();
    PostRender();
    return true;
}


BOOL Core::GameRelease()
{
    DXDevice::CleanupDevice();
    m_write.Release();
    SAFE_DX_RELEASE(m_pSamplerState);
    m_timer.Release();
    SoundManager::GetInstance()->Release();
    ObjectManager::GetInstance().Release();
    Input::GetInstance()->Release();
    TextureManager::GetInstance()->Release();
    ShaderManager::GetInstance()->Release();
    Release();
    return false;
}

BOOL Core::Init()
{
    return true;
}

BOOL Core::Update()
{
    return true;
}

BOOL Core::PreRender()
{
    float color[4] = { 0.70f,0.30f,0.4f,1.0f };
    m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, color); // 렌더타겟(백버퍼)뷰를 채운다.(이전거는 Clear하는거고)
    m_pImmediateContext->PSSetSamplers(0, 1, &m_pSamplerState);
    // 백버퍼에 렌더링 한다.
    return TRUE;
}

BOOL Core::Render()
{

    m_timer.Render();
    Input::GetInstance()->Render();
    m_write.Render();
    
    return true;
}
BOOL Core::PostRender()
{
    
    // 다 뿌렸으면 더블 버퍼링 (프론트 버퍼, 백버퍼 스왑)
    m_pSwapChain->Present(0, 0); // 최종으로 뿌린다.(프론트 버퍼,백버퍼 스왑된다 여기서)
    return TRUE;
}
BOOL Core::Release()
{
    
    return TRUE;
}
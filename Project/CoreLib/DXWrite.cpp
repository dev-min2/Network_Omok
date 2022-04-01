#include "DXWrite.h"

bool DXWrite::Init()
{
    HRESULT hr = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pd2dFactory);
    if (SUCCEEDED(hr))
    {
        hr = ::DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            (IUnknown**)(&m_pWriteFactory));
        hr = ::DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            (IUnknown**)(&m_pUserWriteFactory));

        if (SUCCEEDED(hr))
        {
            hr = m_pWriteFactory->CreateTextFormat(
                L"±Ã¼­",
                NULL,
                DWRITE_FONT_WEIGHT_NORMAL,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                20,
                L"ko-kr",
                &m_pd2dTextFormat
            );
            hr = m_pWriteFactory->CreateTextFormat(
                L"Consolas",//L"Consolas",
                NULL,
                DWRITE_FONT_WEIGHT_NORMAL,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                12,
                L"ko-kr",
                &m_pd2dUserTextFormat
            );
            /*hr = m_pWriteFactory->CreateTextFormat(
                L"Imprint MT Shadow",
                NULL,
                DWRITE_FONT_WEIGHT_NORMAL,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                50,
                L"en-us",
                &m_pd2dMTShadowTF
            );*/
        }
    }
    if (SUCCEEDED(hr))
        return true;
    return false;
}

bool DXWrite::Update()
{
    return true;
}

bool DXWrite::Render()
{
    m_pd2dRT->BeginDraw();
    m_pd2dRT->EndDraw();
    return true;
}

bool DXWrite::Release()
{
    SAFE_DX_RELEASE(m_pd2dMTShadowTF);
    SAFE_DX_RELEASE(m_pd2dTextFormat);
    SAFE_DX_RELEASE(m_pd2dColorBrush);
    SAFE_DX_RELEASE(m_pUserWriteFactory);
    SAFE_DX_RELEASE(m_pd2dUserTextFormat);
    SAFE_DX_RELEASE(m_pd2dRT);
    SAFE_DX_RELEASE(m_pWriteFactory);
    SAFE_DX_RELEASE(m_pd2dFactory);
    return true;
}

bool DXWrite::SetRenderTarget(IDXGISurface1* pSurface)
{
    UINT dpi = ::GetDpiForWindow(g_WindowHandle);

    D2D1_RENDER_TARGET_PROPERTIES rtp;
    ZeroMemory(&rtp, sizeof(D2D1_RENDER_TARGET_PROPERTIES));
    rtp.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
    rtp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN,D2D1_ALPHA_MODE_PREMULTIPLIED);
    rtp.dpiX = dpi;
    rtp.dpiY = dpi;
    rtp.usage = D2D1_RENDER_TARGET_USAGE_NONE;
    rtp.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
    HRESULT hr = m_pd2dFactory->CreateDxgiSurfaceRenderTarget(
        pSurface,
        &rtp,
        &m_pd2dRT);
    if (FAILED(hr))
    {
        return false;
    }

    if (FAILED(m_pd2dRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black),
        &m_pd2dColorBrush)))
    {
        return false;
    }
    return true;
}

void DXWrite::Draw(std::wstring msg, RECT rt, D2D1::ColorF color, IDWriteTextFormat* tf)
{
    m_pd2dRT->BeginDraw();
    D2D_RECT_F fRT;
    fRT.top     = rt.top;
    fRT.left    = rt.left;
    fRT.right   = rt.right;
    fRT.bottom  = rt.bottom;
    
    m_pd2dColorBrush->SetColor(color);
    if (tf == nullptr)
        m_pd2dRT->DrawText(msg.c_str(), msg.size(), m_pd2dTextFormat,
            &fRT, m_pd2dColorBrush);
    else
        m_pd2dRT->DrawText(msg.c_str(), msg.size(), tf,
            &fRT, m_pd2dColorBrush);
    
    m_pd2dRT->EndDraw();
}

void DXWrite::DrawUserFormat(std::wstring msg, RECT rt, D2D1::ColorF color, IDWriteTextFormat* tf)
{
    m_pd2dRT->BeginDraw();
    D2D_RECT_F fRT;
    fRT.top = rt.top;
    fRT.left = rt.left;
    fRT.right = rt.right;
    fRT.bottom = rt.bottom;

    m_pd2dColorBrush->SetColor(color);
    if (tf == nullptr)
        m_pd2dRT->DrawText(msg.c_str(), msg.size(), m_pd2dUserTextFormat,
            &fRT, m_pd2dColorBrush);
    else
        m_pd2dRT->DrawText(msg.c_str(), msg.size(), tf,
            &fRT, m_pd2dColorBrush);

    m_pd2dRT->EndDraw();
}

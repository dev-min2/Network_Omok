#include "Window.h"
HWND g_WindowHandle;
RECT g_clientRect;
Window* g_pWindow = nullptr;
Window::Window() : m_windowInit(FALSE),m_hInst(NULL),m_hWindowHandle(NULL),m_bGameRun(FALSE), m_clientRect()
,m_clientWidth(0),m_clientHeight(0)
{
    g_pWindow = this;
}

Window::~Window()
{
}


BOOL Window::InitWindow(HINSTANCE hInstance, _In_ int nCmdShow, int width, int height)
{
    m_hInst = hInstance;
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    m_windowClassName = L"MyWindowClass";
    wcex.style = CS_HREDRAW | CS_VREDRAW;//WS_CAPTION | WS_SYSMENU
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = m_hInst;
    wcex.hIcon = LoadIcon(m_hInst, NULL);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = m_windowClassName;
    wcex.hIconSm = LoadIcon(wcex.hInstance, NULL);

    RegisterClassExW(&wcex);


    // 윈도우 생성
    RECT rt = { 0,0,width,height };
    AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, FALSE); // 클라이언트 영역이 우리가 원하는 크기가 되게끔 만들어준다.(RT의 크기조정)

   /* m_hWindowHandle = CreateWindow(wcex.lpszClassName, L"오목", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 
        rt.right - rt.left, rt.bottom - rt.top, 
        nullptr, nullptr, m_hInst, nullptr);
    */
    m_hWindowHandle = CreateWindow(
        wcex.lpszClassName, 
        L"오목", 
       WS_CAPTION | WS_SYSMENU, //
        CW_USEDEFAULT, CW_USEDEFAULT,
        rt.right - rt.left, 
        rt.bottom - rt.top,
        nullptr, 
        nullptr, 
        m_hInst, 
        nullptr);

    if (!m_hWindowHandle)
    {
        return FALSE;
    }
    g_WindowHandle = m_hWindowHandle;


    ShowWindow(m_hWindowHandle, SW_SHOW);
    UpdateWindow(m_hWindowHandle);
    m_bGameRun = TRUE;
    m_windowInit = TRUE;


    
    ::GetClientRect(m_hWindowHandle, &m_clientRect);
    g_clientRect = m_clientRect;

    
    return TRUE;
}

BOOL Window::WindowRun()
{
    if (!m_windowInit)
        return false;

    MSG msg;
    GameInit();

    // 기본 메시지 루프입니다:
    while (m_bGameRun)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            m_bGameRun = GameRun();
        }

    }
    GameRelease();
    return TRUE;
}

LRESULT Window::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return LRESULT();
}

LRESULT Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR strTemp[256];
    POINTS point;
    g_pWindow->MsgProc(hWnd, message, wParam, lParam);

    switch (message)
    {
    case WM_DESTROY:
        ::PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

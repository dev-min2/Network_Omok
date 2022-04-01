#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCKAPI_
#include <WinSock2.h>
#include <ws2tcpip.h> // inet_pton
#include <Windows.h>
#include <stdlib.h>
#include <string>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <conio.h>
#include <unordered_set>
#include <functional>
#include <d3d11.h>
#include <d3dcompiler.h>

#include <atlconv.h> 
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"ws2_32.lib")
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#ifdef _DEBUG
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#endif
class DXWrite;
#define GAMERUN int APIENTRY wWinMain(_In_ HINSTANCE hInstance,\
    _In_opt_ HINSTANCE hPrevInstance, \
    _In_ LPWSTR    lpCmdLine, \
    _In_ int       nCmdShow) \
{ \
    Sample window; \
    window.InitWindow(hInstance, nCmdShow, 1024, 768); \
    window.WindowRun(); \
\
    return 0; \
}

#define GAMERUNWH(x,y) int APIENTRY wWinMain(_In_ HINSTANCE hInstance,\
    _In_opt_ HINSTANCE hPrevInstance, \
    _In_ LPWSTR    lpCmdLine, \
    _In_ int       nCmdShow) \
{ \
    _CrtDumpMemoryLeaks(); \
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); \
    Sample window; \
    window.InitWindow(hInstance, nCmdShow, x, y); \
    window.WindowRun(); \
\
    return 0; \
}
extern HWND		g_WindowHandle;
extern RECT		g_clientRect;
extern float    g_deltaTime;
extern float    g_gameTime;
extern POINT    g_mouse;
extern ID3D11Device* g_pd3dDevice;	// 디바이스 객체
extern ID3D11DeviceContext* g_pContext;// 다비이스 컨텍스트 객체
extern DXWrite*              g_wt;

#define SERVER_IP "127.0.0.1"
#define PORT_NUM 9123
#define BUF_SIZE 1024
#define SAFE_DX_RELEASE(dx) if(dx != nullptr) dx->Release(); dx = nullptr;
#define SAFE_DELETE(obj) if(obj != nullptr) delete obj; obj = nullptr;
#define MAX_BUFFER 256
static std::wstring Multi2Uni(const std::string & _src)
{
    USES_CONVERSION;
    return std::wstring(A2W(_src.c_str()));
};

static std::string Uni2Multi(const std::wstring & _src)
{
    USES_CONVERSION;
    return std::string(W2A(_src.c_str()));
};

template <typename T>
class Singleton
{
public:
    static T& GetInstace()
    {
        static  T instance;
        return instance;
    }
};
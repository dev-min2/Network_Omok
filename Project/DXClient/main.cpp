#include "Sample.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, 
    _In_opt_ HINSTANCE hPrevInstance, 
    _In_ LPWSTR    lpCmdLine, 
    _In_ int       nCmdShow) 
{ 
    _CrtDumpMemoryLeaks(); 
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); 
    Sample window; 
    //window.InitWindow(hInstance, nCmdShow, 1600, 900); 
    window.InitWindow(hInstance, nCmdShow, 1600, 900);
    window.WindowRun(); 
    
    return 0; 
}
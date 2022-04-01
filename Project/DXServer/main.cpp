// Packet.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include "Acceptor.h"
//#include "DefUserClass.h"
#include "Session.h"
#include "Packet.h"
#include <chrono>
#include "ThreadManager.h"
#include "RingBuffer.h"
#include "User.h"
#include "RemoveSession.h"
#include <crtdbg.h>
#include "OdbcContents.h"
#include "RoomManager.h"
#include "PlayerManager.h"

int main()
{
#if _DEBUG
    //#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    ::setlocale(LC_ALL, "korean");
    RoomManager::GetInstance();
    PlayerManager::GetInstance();
    SQLWCHAR dir[MAX_PATH] = { 0, };
    ::GetCurrentDirectory(MAX_PATH, dir);
    std::wstring dbPath = dir;
    //dbPath += L"\\REALDBSA.dsn"; // DB2.dsn(파일)
    dbPath += L"\\omokdb.dsn"; // DB2.dsn(파일)

    { 
        Acceptor<User> ac;
        ac.Init("127.0.0.1", 9199);
        OdbcContents::GetInstance()->Init();
        OdbcContents::GetInstance()->Connect(dbPath.c_str());
        //OdbcContents::GetInstance()->ConnectUserDsn(dbPath.c_str());
        //OdbcContents::GetInstance()->ConnectDirect();
        OdbcContents::GetInstance()->OdbcContentsInit();
        CompletionHandler::GetInstance()->Init(&ac);
        
        DWORD waitTick = 1000;
        RemoveSession::GetInstance()->InitRemoveSessionThread(waitTick);
        ThreadManager::GetInstance()->Join(); // 모든 쓰레드 종료 대기.
        OdbcContents::GetInstance()->ReleaseInstance();
        RoomManager::ReleaseInstance();
        PlayerManager::ReleaseInstance();
    }
   
   
    return 0;
}
#include "OdbcContents.h"
#include "Player.h"
#include "PlayerManager.h"
OdbcContents* OdbcContents::m_pInstance = nullptr;
OdbcContents* OdbcContents::GetInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new OdbcContents();
    return m_pInstance;
}

void OdbcContents::ReleaseInstance()
{
    m_pInstance->Release();
    for (uInt32 del = 0; del < MAX_CONTENTS_SQL; ++del)
        SQLFreeHandle(SQL_HANDLE_STMT, m_pInstance->m_hContentsStmts[del]);

    if(m_pInstance != nullptr)
        SAFE_DELETE(m_pInstance);
}

bool OdbcContents::OdbcContentsInit()
{
    m_hContentsStmts.reserve(20);
    m_prepareList.reserve(20);

    uInt32 in;
    for (in = 0; in < MAX_CONTENTS_SQL; ++in)
        CreateContentsSQL();
    for (in = 0; in < MAX_CONTENTS_SQL; ++in)
        m_prepareList.push_back(false);
    return true;
}

uInt32 OdbcContents::CreateAccount(std::wstring& id, std::wstring& ps, std::wstring& age)
{
    // 락?
    // 동시에 똑같은 아이디로 회원가입할 시 겹칠 수 있음.
    SQLRETURN   retCode;
    SWORD       retValue = 0; //리턴값
    SQLLEN      retParam = SQL_NTS;
    int32 ageInt = ::_wtoi(age.c_str());
    { // 세팅부분
        retCode = SQLBindParameter(m_hContentsStmts[CQ_CreateAccount], 1, SQL_PARAM_OUTPUT,
            SQL_C_SSHORT, SQL_INTEGER, 0, 0, &retValue, 0, &retParam);


        retCode = ::SQLBindParameter
        (
            m_hContentsStmts[CQ_CreateAccount], 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
            (id.size() + 2) * sizeof(WCHAR), 0, (void*)id.c_str(),
            (id.size() + 2) * sizeof(WCHAR), NULL
        );
        retCode = ::SQLBindParameter
        (
            
            m_hContentsStmts[CQ_CreateAccount], 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
            (ps.size() + 2) * sizeof(WCHAR), 0, (void*)ps.c_str(),
            (ps.size() + 2) * sizeof(WCHAR), NULL
        );
        retCode = ::SQLBindParameter
        (
            m_hContentsStmts[CQ_CreateAccount], 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,
            sizeof(ageInt), 0, (void*)&ageInt,
            sizeof(ageInt), NULL
        );
    }
    if (!m_prepareList[PS_Account])
    {
        TCHAR callSp[] = L"{? = call OmokDB.dbo.CreateAccount(?,?,?)}";
        retCode = ::SQLPrepare(m_hContentsStmts[CQ_CreateAccount], callSp, SQL_NTS);
        m_prepareList[PS_Account] = true;
    }
    retCode = ::SQLExecute(m_hContentsStmts[CQ_CreateAccount]);
    if (retCode != SQL_SUCCESS && retCode != SQL_SUCCESS_WITH_INFO)
    {
        ErrorCheck();
        return false;
    }

    EndCommand(m_hContentsStmts[CQ_CreateAccount]);

    if (retValue == 0) // 0 이외의 값은 실패.
        return 0;
    else if (retValue == 1)
        return 1;
    else
        return 2;
}

uInt32 OdbcContents::RequestLogin(std::wstring& id, std::wstring& ps)
{
    // 락?
    // 동시에 똑같은 아이디로 로그인할 시 겹칠 수 있음.
    SQLRETURN   retCode;
    SWORD       retValue = 0; //리턴값
    SQLLEN      retParam = SQL_NTS;
    { // 세팅부분
        retCode = SQLBindParameter(m_hContentsStmts[CQ_LoginRequest], 1, SQL_PARAM_OUTPUT,
            SQL_C_SSHORT, SQL_INTEGER, 0, 0, &retValue, 0, &retParam);

        retCode = ::SQLBindParameter
        (
            m_hContentsStmts[CQ_LoginRequest], 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
            (id.size() + 2) * sizeof(WCHAR), 0, (void*)id.c_str(),
            (id.size() + 2) * sizeof(WCHAR), NULL
        );
        retCode = ::SQLBindParameter
        (

            m_hContentsStmts[CQ_LoginRequest], 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
            (ps.size() + 2) * sizeof(WCHAR), 0, (void*)ps.c_str(),
            (ps.size() + 2) * sizeof(WCHAR), NULL
        );
    }
    if (!m_prepareList[PS_Login])
    {
        TCHAR callSp[] = L"{? = call OmokDB.dbo.RequestLogin(?,?)}";
        retCode = ::SQLPrepare(m_hContentsStmts[CQ_LoginRequest], callSp, SQL_NTS);
        m_prepareList[PS_Login] = true;
    }
    retCode = ::SQLExecute(m_hContentsStmts[CQ_LoginRequest]);
    if (retCode != SQL_SUCCESS && retCode != SQL_SUCCESS_WITH_INFO)
    {
        //ErrorCheck();
        //std::wcout << "[Server] " << id << " " << std::endl;
        return 2;
    }

    EndCommand(m_hContentsStmts[CQ_LoginRequest]);

    if (retValue == 0) // 0 이외의 값은 실패.
    {
        if (PlayerManager::GetInstance()->FindPlayer(id) != nullptr)
        {
            std::wcout << L"[Server] " << id << L"(은)는 이미 로그인 중" << std::endl;
            return 999;
        }
        return 0;
    }
    else if (retValue == 1)
        return 1;
    else
        return 2;
}

void OdbcContents::UpdateLogout(std::wstring& id)
{
    SQLRETURN retCode;

    SWORD retValue = 0; // 리턴값
    SQLLEN retParam = SQL_NTS;
    // 세팅부분
    retCode = ::SQLBindParameter(m_hContentsStmts[CQ_LogoutRequest], 1, SQL_PARAM_OUTPUT,
        SQL_C_SSHORT, SQL_INTEGER, 0, 0, &retValue, 0, &retParam);

    retCode = ::SQLBindParameter
    (
        m_hContentsStmts[CQ_LogoutRequest], 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
        (id.size() + 2) * sizeof(WCHAR), 0, (void*)id.c_str(),
        (id.size() + 2) * sizeof(WCHAR), NULL
    );

    if (!m_prepareList[PS_Logout])
    {
        TCHAR callSp[] = L"{? = call OmokDB.dbo.UpdateLogout(?)}";

        retCode = ::SQLPrepare(m_hContentsStmts[CQ_LogoutRequest], callSp, SQL_NTS);
        m_prepareList[PS_Logout] = true;
    }

    retCode = ::SQLExecute(m_hContentsStmts[CQ_LogoutRequest]);
    if (retCode != SQL_SUCCESS && retCode != SQL_SUCCESS_WITH_INFO)
    {
        ErrorCheck();
        return;
    }

    std::cout << "[Server] User : "; //<< name.c_str() << "로그아웃.." << std::endl;
    std::wcout << id.c_str();
    std::cout << " 로그아웃 업데이트." << std::endl;
    EndCommand(m_hContentsStmts[CQ_LogoutRequest]);

    return;
}

std::pair<bool, UserInfoDB> OdbcContents::SetPlayerInfo(Player* player)
{
    SQLRETURN   retCode;
    SWORD       retValue = 0; //리턴값
    SQLLEN      retParam = SQL_NTS;

    std::pair<bool, UserInfoDB> playerInfo;
    playerInfo.first = false;
    int32 total;
    int32 winCount;
    int32 loseCount;
    float winRate;

    { 
        // 세팅부분
        retCode = SQLBindParameter(m_hContentsStmts[CQ_GetPlayerInfo], 1, SQL_PARAM_OUTPUT,
            SQL_C_SSHORT, SQL_INTEGER, 0, 0, &retValue, 0, &retParam);


        retCode = ::SQLBindParameter
        (
            m_hContentsStmts[CQ_GetPlayerInfo], 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
            (player->GetPlayerInfo()->m_id.size() + 2) * sizeof(WCHAR), 0, (void*)player->GetPlayerInfo()->m_id.c_str(),
            (player->GetPlayerInfo()->m_id.size() + 2) * sizeof(WCHAR), NULL
        );
        retCode = ::SQLBindParameter
        (
            m_hContentsStmts[CQ_GetPlayerInfo], 3, SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER,
            sizeof(total), 0, (void*)&total,
            sizeof(total), NULL
        );
        retCode = ::SQLBindParameter
        (
            m_hContentsStmts[CQ_GetPlayerInfo], 4, SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER,
            sizeof(winCount), 0, (void*)&winCount,
            sizeof(winCount), NULL
        );
        retCode = ::SQLBindParameter
        (
            m_hContentsStmts[CQ_GetPlayerInfo], 5, SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER,
            sizeof(loseCount), 0, (void*)&loseCount,
            sizeof(loseCount), NULL
        );
        retCode = ::SQLBindParameter
        (
            m_hContentsStmts[CQ_GetPlayerInfo], 6, SQL_PARAM_OUTPUT, SQL_C_FLOAT, SQL_FLOAT,
            sizeof(winRate), 0, (void*)&winRate,
            sizeof(winRate), NULL
        );
    }
    if (!m_prepareList[PS_Info])
    {
        TCHAR callSp[] = L"{? = call OmokDB.dbo.GetPlayerInfo(?,?,?,?,?)}";
        retCode = ::SQLPrepare(m_hContentsStmts[CQ_GetPlayerInfo], callSp, SQL_NTS);
        m_prepareList[PS_Info] = true;
    }
    retCode = ::SQLExecute(m_hContentsStmts[CQ_GetPlayerInfo]);
    if (retCode != SQL_SUCCESS && retCode != SQL_SUCCESS_WITH_INFO)
    {
        ErrorCheck();
        return playerInfo;
    }

    EndCommand(m_hContentsStmts[CQ_GetPlayerInfo]);

    if (retValue == 0) 
    {
        playerInfo.first = true;

        playerInfo.second.m_totalPlayCount = total;
        playerInfo.second.m_winGameCount = winCount;
        playerInfo.second.m_loseGameCount = loseCount;
        playerInfo.second.m_winRate = winRate;
        player->SetPlayerInfo(playerInfo.second);
        return playerInfo;
    }
    else if (retValue == 1)
    {
        return playerInfo;
    }
    else
        return playerInfo;
}

void OdbcContents::UpdateUserInfo(std::wstring& id, uInt32 total, uInt32 winCount, uInt32 loseCount, float rate)
{
    SQLRETURN   retCode;
    SWORD       retValue = 0; //리턴값
    SQLLEN      retParam = SQL_NTS;
    
    {
        // 세팅부분
        retCode = SQLBindParameter(m_hContentsStmts[CQ_UpdatePlayerInfo], 1, SQL_PARAM_OUTPUT,
            SQL_C_SSHORT, SQL_INTEGER, 0, 0, &retValue, 0, &retParam);


        retCode = ::SQLBindParameter
        (
            m_hContentsStmts[CQ_UpdatePlayerInfo], 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
            (id.size() + 2) * sizeof(WCHAR), 0, (void*)id.c_str(),
            (id.size() + 2) * sizeof(WCHAR), NULL
        );
        retCode = ::SQLBindParameter
        (
            m_hContentsStmts[CQ_UpdatePlayerInfo], 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,
            sizeof(total), 0, (void*)&total,
            sizeof(total), NULL
        );
        retCode = ::SQLBindParameter
        (
            m_hContentsStmts[CQ_UpdatePlayerInfo], 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,
            sizeof(winCount), 0, (void*)&winCount,
            sizeof(winCount), NULL
        );
        retCode = ::SQLBindParameter
        (
            m_hContentsStmts[CQ_UpdatePlayerInfo], 5, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,
            sizeof(loseCount), 0, (void*)&loseCount,
            sizeof(loseCount), NULL
        );
        retCode = ::SQLBindParameter
        (
            m_hContentsStmts[CQ_UpdatePlayerInfo], 6, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_FLOAT,
            sizeof(rate), 0, (void*)&rate,
            sizeof(rate), NULL
        );
    }
    if (!m_prepareList[PS_UpdateInfo])
    {
        TCHAR callSp[] = L"{? = call OmokDB.dbo.UserInfoUpdate(?,?,?,?,?)}";
        retCode = ::SQLPrepare(m_hContentsStmts[CQ_UpdatePlayerInfo], callSp, SQL_NTS);
        m_prepareList[PS_UpdateInfo] = true;
    }
    retCode = ::SQLExecute(m_hContentsStmts[CQ_UpdatePlayerInfo]);
    if (retCode != SQL_SUCCESS && retCode != SQL_SUCCESS_WITH_INFO)
    {
        ErrorCheck();
        return;
    }

    EndCommand(m_hContentsStmts[CQ_UpdatePlayerInfo]);

    if (retValue == 0)
    {
        std::wcout << L"[Server] " << id << L" 유저 정보 업데이트" << std::endl;
        return;
    }
}

void OdbcContents::EndCommand(SQLHSTMT hStmt)
{
    while (::SQLMoreResults(hStmt) != SQL_NO_DATA);
    //::SQLFreeStmt(hStmt, SQL_UNBIND); // 
    ::SQLFreeStmt(hStmt, SQL_RESET_PARAMS); // bindParameter에 설정된 매개변수 모두 해제.
    //::SQLFreeStmt(hStmt, SQL_CLOSE); // 
    ::SQLCloseCursor(hStmt);
}

void OdbcContents::CreateContentsSQL()
{
    SQLRETURN retCode;
    SQLHSTMT hstmt = SQL_NULL_HSTMT;
    retCode = ::SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &hstmt);
    if (retCode != SQL_SUCCESS && retCode != SQL_SUCCESS_WITH_INFO)
    {
        ErrorCheck();
    }
    m_hContentsStmts.push_back(hstmt);
}
OdbcContents::OdbcContents()
{

}
OdbcContents::~OdbcContents()
{
}

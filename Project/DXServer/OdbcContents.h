#pragma once
#include "OdbcDB.h"
class Player;
#define MAX_CONTENTS_SQL 5
#define MAX_PREPARE 5
enum ContentSQL
{
	CQ_CreateAccount = 0,
	CQ_LoginRequest,
	CQ_LogoutRequest,
	CQ_GetPlayerInfo,
	CQ_UpdatePlayerInfo
};

enum PrepareSQL
{
	//PS
	PS_Account = 0,
	PS_Login,
	PS_Logout,
	PS_Info,
	PS_UpdateInfo
};

class UserInfoDB
{
public:
	UserInfoDB() : m_totalPlayCount(0),m_winGameCount(0),m_loseGameCount(0),m_winRate(0.0f) { m_id.reserve(20); }
	std::wstring	m_id;
	int32			m_totalPlayCount;
	int32			m_winGameCount;
	int32			m_loseGameCount;
	float			m_winRate;
};

class OdbcContents : public OdbcDB
{
public:
	static	OdbcContents*			GetInstance();
	static	void					ReleaseInstance();
public:
	bool							OdbcContentsInit();
public:
	uInt32							CreateAccount(std::wstring& id, std::wstring& ps, std::wstring& age);
	uInt32							RequestLogin(std::wstring& id, std::wstring& ps);
	void							UpdateLogout(std::wstring& id);
	std::pair<bool,UserInfoDB>		SetPlayerInfo(Player* player);
	void							UpdateUserInfo(std::wstring& id, uInt32 total, uInt32 winCount, uInt32 loseCount, float rate);

	// wstring을 반환하는 건 User에서 들고있기 위함.


private:
	void							EndCommand(SQLHSTMT hStmt);
	void							CreateContentsSQL();
private:		
	OdbcContents();
	~OdbcContents() override;
private:
	static OdbcContents*			m_pInstance;
private:
	std::vector<SQLHSTMT>			m_hContentsStmts;
	std::vector<bool>				m_prepareList; // SP
};


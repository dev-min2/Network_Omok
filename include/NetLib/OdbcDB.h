#pragma once
#include "std.h"
#include <sql.h>
#include <sqlext.h>
#define MAX_SIZE 255

// 상속해서 컨텐츠단에서 사용
struct ColumnInfo
{
	SQLUSMALLINT        m_col;
	SQLWCHAR			m_colName[20];
	SQLSMALLINT         m_bufferLength;
	SQLSMALLINT			m_nameLengthPtr;
	SQLSMALLINT			m_sqlType;
	SQLULEN				m_columnSizePtr;
	SQLSMALLINT			m_decimalDigitsPtr;
	SQLSMALLINT			m_nullable;
};
struct TableInfo
{
	std::vector<ColumnInfo>	    m_ColumnList; // 열 정보.
	SQLSMALLINT 				m_numCol; // 열 숫자.
	std::wstring				m_tableName; // 열 이름.
};
struct Field
{
	std::wstring				m_data;
	int							m_dataType;
};
struct Record
{
	std::vector<Field>  fields;
};
class OdbcDB
{
protected:
	OdbcDB() = default;
	virtual ~OdbcDB() = default;
public:
	bool						Init();
	bool						Connect(const TCHAR* dsn);
	bool						ConnectUserDsn(const TCHAR* dsnName);
	bool						Release();
	bool						ExecTableInfo(const TCHAR* tableName);
	void						ErrorCheck();
protected:
	SQLHENV						m_hEnv; // 환경핸들
	SQLHDBC						m_hDbc; // 연결핸들
	SQLHSTMT					m_hStmt; // 명령핸들
	std::vector<TableInfo>		m_tableList;
	std::vector<Record>         m_stringRecordData;
};


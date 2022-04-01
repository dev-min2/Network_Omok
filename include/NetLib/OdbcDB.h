#pragma once
#include "std.h"
#include <sql.h>
#include <sqlext.h>
#define MAX_SIZE 255

// ����ؼ� �������ܿ��� ���
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
	std::vector<ColumnInfo>	    m_ColumnList; // �� ����.
	SQLSMALLINT 				m_numCol; // �� ����.
	std::wstring				m_tableName; // �� �̸�.
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
	SQLHENV						m_hEnv; // ȯ���ڵ�
	SQLHDBC						m_hDbc; // �����ڵ�
	SQLHSTMT					m_hStmt; // ����ڵ�
	std::vector<TableInfo>		m_tableList;
	std::vector<Record>         m_stringRecordData;
};


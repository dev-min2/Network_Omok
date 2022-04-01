#include "OdbcDB.h"
#include <stdio.h>
#include <string>


#include <tchar.h>

bool OdbcDB::Init()
{
	// 초기화
	if (::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv) != SQL_SUCCESS)
	{
		return false;
	}
	if (::SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION,
		(SQLPOINTER)SQL_OV_ODBC3_80, SQL_IS_INTEGER) != SQL_SUCCESS)
	{
		//MSSQL -> ODBC3.8로 연결하려면 Driver가 설치되어있어야한다.
		return false;
	}
	if (::SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hDbc) != SQL_SUCCESS)
	{
		return false;
	}
	return true;
}

bool OdbcDB::Connect(const TCHAR* dsn)
{
	SQLTCHAR	outConnect[MAX_SIZE] = { 0, };
	//SQLSMALLINT outConn;
	TCHAR       inConn[MAX_SIZE] = { 0, };

	int size = sizeof(outConnect);

	SQLSMALLINT outLen;
	SQLRETURN ret;

	_stprintf(inConn, _T("FileDsn=%s"), dsn);
	ret = ::SQLDriverConnect(m_hDbc, NULL,
		inConn, _countof(inConn),
		outConnect, _countof(outConnect),
		&outLen, SQL_DRIVER_NOPROMPT);
	


	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		ErrorCheck();
		return false;
	}
	if (::SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmt) != SQL_SUCCESS)
	{
		return false;
	}

	WCHAR verCheck[20] = { 0, };
	SQLSMALLINT ch = 0;
	ret = ::SQLGetInfo(m_hDbc, SQL_DRIVER_ODBC_VER, verCheck, _countof(verCheck), &ch);

	std::wcout << L"[Server] ODBC Driver VERSION : " << verCheck << std::endl;
	

	return true;
}

bool OdbcDB::ConnectUserDsn(const TCHAR* dsnName)
{
	SQLTCHAR	outConnect[MAX_SIZE] = { 0, };
	//SQLSMALLINT outConn;
	TCHAR       inConn[MAX_SIZE] = { 0, };

	int size = sizeof(outConnect);

	//SQLSMALLINT outLen;
	SQLRETURN ret;
	_stprintf(inConn, _T("Dsn=%s"), dsnName);
	ret = SQLConnect(m_hDbc,
		(SQLTCHAR*)dsnName, SQL_NTS,
		(SQLTCHAR*)L"sa", SQL_NTS,
		(SQLTCHAR*)L"1q2w3e4r!", SQL_NTS);

	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		ErrorCheck();
		return false;
	}
	if (::SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmt) != SQL_SUCCESS)
	{
		return false;
	}

	WCHAR verCheck[20] = { 0, };
	SQLSMALLINT ch = 0;
	ret = ::SQLGetInfo(m_hDbc, SQL_DRIVER_ODBC_VER, verCheck, _countof(verCheck), &ch);

	std::wcout << L"[Server] ODBC Driver VERSION : " << verCheck << std::endl;
	return true;
}

bool OdbcDB::Release()
{
	::SQLCloseCursor(m_hStmt);
	::SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt);
	::SQLDisconnect(m_hDbc);
	::SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc);
	::SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
	return true;
}

bool OdbcDB::ExecTableInfo(const TCHAR* tableName)
{
	TableInfo table;
	table.m_tableName = tableName;
	std::wstring sql = L"select * from ";
	sql += tableName; // SQL

	// 지정된 SQL 질의문을 직접 실행 (여기서는 위의 sql스트링)
	SQLRETURN ret = ::SQLExecDirect(m_hStmt, (SQLTCHAR*)sql.c_str(), SQL_NTS);
	if (ret != SQL_SUCCESS)
	{
		ErrorCheck();
		return false;
	}

	//SQLSMALLINT iNumCols;
	::SQLNumResultCols(m_hStmt, &table.m_numCol); // 해당 테이블의 컬럼 갯수 가져온다.

	// 테이블의 컬럼 정보가져오기.
	for (int cols = 1; cols <= table.m_numCol; cols++)
	{
		ColumnInfo column;
		column.m_col = cols;
		int iSize = _countof(column.m_colName);
		SQLDescribeCol(m_hStmt,
			cols,
			column.m_colName,
			iSize,
			&column.m_nameLengthPtr,
			&column.m_sqlType,// 데이터형
			&column.m_columnSizePtr,
			&column.m_decimalDigitsPtr, // 10진수 자리수
			&column.m_nullable);// NULL 허용여부
		table.m_ColumnList.push_back(column);
	}

	SQLLEN temp;
	TCHAR strData[100][21] = { 0, };
	int   intData[100];
	Record recordData;
	for (int bindCol = 0; bindCol < table.m_ColumnList.size(); bindCol++)
	{
		switch (table.m_ColumnList[bindCol].m_sqlType)
		{
		case SQL_TYPE_TIMESTAMP:
		{
			Field field;
			field.m_dataType = SQL_UNICODE;
			ret = ::SQLBindCol(m_hStmt, bindCol + 1,
				SQL_TYPE_TIMESTAMP,
				&strData[bindCol],
				0,
				&temp);
			if (ret != SQL_SUCCESS)
			{
				ErrorCheck();
				return false;
			}
			recordData.fields.push_back(field);
		}break;
		case SQL_WCHAR:
		case SQL_WVARCHAR: {
			Field field;
			field.m_dataType = SQL_UNICODE;
			ret = ::SQLBindCol(m_hStmt, bindCol + 1,
				SQL_UNICODE,
				strData[bindCol],
				sizeof(strData[bindCol]),
				&temp);
			if (ret != SQL_SUCCESS)
			{
				ErrorCheck();
				return false;
			}
			recordData.fields.push_back(field);
		}break;
		case SQL_INTEGER: {
			Field field;
			field.m_dataType = SQL_INTEGER;
			ret = ::SQLBindCol(m_hStmt, bindCol + 1,
				SQL_INTEGER,
				&intData[bindCol],
				0,
				&temp);
			if (ret != SQL_SUCCESS)
			{
				ErrorCheck();
				return false;
			}
			recordData.fields.push_back(field);
		}break;
		case -7: {
			Field field;
			field.m_dataType = SQL_C_ULONG;
			ret = ::SQLBindCol(m_hStmt, bindCol + 1,
				SQL_C_ULONG,
				&intData[bindCol],
				0,
				&temp);
			if (ret != SQL_SUCCESS)
			{
				ErrorCheck();
				return false;
			}
			recordData.fields.push_back(field);
		};
		}
	}

	// 바인딩 된 열에 데이터를 가져옴.
	while (SQLFetch(m_hStmt) != SQL_NO_DATA)
	{
		for (int iCol = 0; iCol < table.m_ColumnList.size(); iCol++)
		{
			recordData.fields[iCol].m_dataType = recordData.fields[iCol].m_dataType;
			if (recordData.fields[iCol].m_dataType == SQL_UNICODE)
			{
				recordData.fields[iCol].m_data = strData[iCol];
			}
			else
			{
				recordData.fields[iCol].m_data = std::to_wstring(intData[iCol]);
			}
		}
		m_stringRecordData.push_back(recordData);
	}
	SQLCloseCursor(m_hStmt);
	m_tableList.push_back(table);
	return true;
}

void OdbcDB::ErrorCheck()
{
	SQLTCHAR	sqlState[SQL_SQLSTATE_SIZE + 1];
	SQLTCHAR	errorBuffer[SQL_MAX_MESSAGE_LENGTH + 1];
	SQLINTEGER	sqlCode;
	SQLSMALLINT len;
	::SQLError(m_hEnv, m_hDbc,
		m_hStmt,
		sqlState,
		&sqlCode,
		errorBuffer,
		sizeof(errorBuffer),
		&len);
	::MessageBox(NULL, errorBuffer, sqlState, MB_OK);
}



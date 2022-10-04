#include "StdAfx.h"
//#include <strstream>
#include "COdbcManager.h"
#include "strsafe.h" // Safe string function

COdbcManager* COdbcManager::SelfInstance = NULL;

COdbcManager::COdbcManager(void)
{
	hEnvSerMan = NULL;  // Handle of sql (void*)	
	hOdbcSerMan = NULL;; // Handle of ODBC (void*)	
}

COdbcManager* COdbcManager::GetInstance()
{
	if (SelfInstance == NULL)
		SelfInstance = new COdbcManager();

	return SelfInstance;
}

void COdbcManager::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		delete SelfInstance;
		SelfInstance = NULL;
	}
}

COdbcManager::~COdbcManager(void)
{
	if (hOdbcSerMan != NULL)
	{
		::SQLDisconnect(hOdbcSerMan);
		::SQLFreeHandle(SQL_HANDLE_DBC, hOdbcSerMan);
		hOdbcSerMan = NULL;
	}

	if (hEnvSerMan != NULL)
	{
		::SQLFreeHandle(SQL_HANDLE_ENV, hEnvSerMan);
		hEnvSerMan = NULL;
	}
}

void COdbcManager::SetSerManOdbcName(const char* szName)
{
	::StringCchCopy(m_szSerManOdbcName, DB_SVR_NAME_LENGTH, szName);
}

void COdbcManager::SetSerManUserID(const char* szUsrID)
{
	::StringCchCopy(m_szSerManUsrID, USER_ID_LENGTH, szUsrID);
}

void COdbcManager::SetSerManUserPass(const char* szUsrPasswd)
{
	::StringCchCopy(m_szSerManUsrPass, USER_PASS_LENGTH, szUsrPasswd);
}

void COdbcManager::SetSerManDBName(const char* szDBName)
{
	::StringCchCopy(m_szSerManDBName, DB_NAME_LENGTH, szDBName);
}

void COdbcManager::SetSerManDBServer(const char* szName,
						const char* szUsrID,
						const char* szUsrPass,
						const char* szDBName)
{
	SetSerManOdbcName	(szName);
	SetSerManUserID	(szUsrID);
	SetSerManUserPass	(szUsrPass);
	SetSerManDBName	(szDBName);	
}

int	COdbcManager::OpenSerManDB()
{
	SQLRETURN sReturn = 0; // SQL return value (short)

	// ODBC allocate environment memory
	sReturn = ::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnvSerMan);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{		
		return DB_ERROR;
	}

	// ODBC set environment
	// version
	sReturn = ::SQLSetEnvAttr(hEnvSerMan, SQL_ATTR_ODBC_VERSION, (void*) SQL_OV_ODBC3, 0);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{		
		::SQLFreeHandle(SQL_HANDLE_ENV, hEnvSerMan);
		hEnvSerMan = NULL;
		return DB_ERROR;
	}
    
	// ODBC allocate connection memory
	sReturn = ::SQLAllocHandle(SQL_HANDLE_DBC, hEnvSerMan, &hOdbcSerMan);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		::SQLFreeHandle(SQL_HANDLE_DBC, hOdbcSerMan);
		hOdbcSerMan = NULL;
		::SQLFreeHandle(SQL_HANDLE_ENV, hEnvSerMan);
		hEnvSerMan = NULL;
		return DB_ERROR;
	}

	// ODBC connect
	// Login time out 5 sec
	sReturn = ::SQLSetConnectAttr(hOdbcSerMan, 5, (void*) SQL_LOGIN_TIMEOUT, 0);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{	
		::SQLFreeHandle(SQL_HANDLE_ENV, hEnvSerMan);
		hEnvSerMan = NULL;
		return DB_ERROR;
	}

	sReturn = ::SQLConnect(hOdbcSerMan, 
						(SQLCHAR*) m_szSerManOdbcName,
						SQL_NTS,
						(SQLCHAR*) m_szSerManUsrID,
						SQL_NTS,
						(SQLCHAR*) m_szSerManUsrPass,
						SQL_NTS);

	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
	{	
		return DB_OK;
	}
	else
	{
		::SQLFreeHandle(SQL_HANDLE_ENV, hEnvSerMan);
		hEnvSerMan = NULL;
		return DB_ERROR;
	}
}

int	COdbcManager::OpenSerManDB(char* szOdbcName, 
			char* szUsrID, 
			char* szUsrPasswd,
			char* szDbName)
{
	SetSerManDB(szOdbcName, szUsrID, szUsrPasswd, szDbName);
	return OpenSerManDB();
}

void COdbcManager::CloseSerManDB()
{
	if (hOdbcSerMan != NULL)
	{
		::SQLDisconnect(hOdbcSerMan);
		::SQLFreeHandle(SQL_HANDLE_DBC, hOdbcSerMan);
		hOdbcSerMan = NULL;
	}

	if (hEnvSerMan != NULL)
	{
		::SQLFreeHandle(SQL_HANDLE_ENV, hEnvSerMan);
		hEnvSerMan = NULL;
	}
}

void COdbcManager::SetSerManDB(const char* szOdbcName,
						const char* szUsrID,
						const char* szUsrPass,
						const char* szDBName)
{
	SetSerManOdbcName(szOdbcName);
	SetSerManUserID(szUsrID);
	SetSerManUserPass(szUsrPass);
}

int COdbcManager::Insert(CString strSQL)
{
	return ExecuteSQL(strSQL);
}

int	COdbcManager::ExecuteSQL(CString strSQL)
{
	SQLRETURN	sReturn = 0; // SQL return value (short)	
	SQLHSTMT	hStmt; // Handle of statement (void*)
	
	// Prepare statement handle
	sReturn = ::SQLAllocHandle(SQL_HANDLE_STMT, hOdbcSerMan, &hStmt);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		return DB_ERROR;
	}
	
	sReturn = ::SQLExecDirect(hStmt, 
							(SQLCHAR*) strSQL.GetString(), 
							SQL_NTS);
	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO || sReturn == SQL_NO_DATA)
	{
		::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);	
		return DB_OK;
	}
	else
	{
		::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);	
		return DB_ERROR;
	}
}
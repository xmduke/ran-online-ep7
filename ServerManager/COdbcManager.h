#pragma once

#ifndef COdbcManager_H_
#define COdbcManager_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Windows.h>
#include <sqlext.h> // ODBC library
#include <stdio.h>

#include <odbcinst.h>

// Auto Link Library Files
// To use the installer DLL functions, you must: 
// Use #include < ODBCINST.H>.
// Link with Odbcinst.lib for 16-bit applications.
// Link with Odbccp32.lib for 32-bit applications. 
// #pragma comment (lib,"Odbccp32.lib")
#pragma comment (lib,"odbc32.lib")


// Seconds of database response time
#define DB_RESPONSE_TIME	30
#define DB_CONNECTION_COUNT 10

#define DB_ERROR			-1
#define DB_OK				0

#define DB_USE				1
#define DB_NOT_USE			0

#define DB_CHA_MAX			-2

// string data size
#define USER_ID_LENGTH				20
#define	USER_PASS_LENGTH			20

//#define CHAT_MSG_SIZE				100
#define DB_SVR_NAME_LENGTH			50
#define DB_NAME_LENGTH				50
#define SERVER_NAME_LENGTH			50
//#define MAP_NAME_LENGTH				50

//#define MAX_ONESERVERCHAR_NUM		16 // 한 서버에서 만들수 있는 캐릭터 갯수

class COdbcManager
{
public:
	static COdbcManager* GetInstance();	
	static void	ReleaseInstance();

private:
	COdbcManager(void);
	~COdbcManager(void);

	static COdbcManager* SelfInstance;

protected:
   	SQLHENV hEnvSerMan;  // Handle of sql (void*)	
	SQLHDBC hOdbcSerMan; // Handle of ODBC (void*)	

	char		m_szSerManOdbcName[DB_SVR_NAME_LENGTH+1];	
	char		m_szSerManUsrID[USER_ID_LENGTH+1];	
	char		m_szSerManUsrPass[USER_PASS_LENGTH+1];
	char		m_szSerManDBName[DB_NAME_LENGTH+1];

	int			m_nSerManDBTimeOut;

public:	
	void		SetSerManDB(const char* szOdbcName,
						const char* szUsrID,
						const char* szUsrPass,
						const char* szDBName);
	void		SetSerManOdbcName	(const char* szName);
	void		SetSerManUserID		(const char* szUsrID);
	void		SetSerManUserPass	(const char* szUsrPasswd);
	void		SetSerManDBName		(const char* szDBName);
	void		SetSerManDBServer	(const char* szName,
									const char* szUsrID,
									const char* szUsrPass,
									const char* szDBName);
	int			OpenSerManDB();	
	
	int			OpenSerManDB(char* szOdbcName, 
						char* szUsrID, 
						char* szUsrPasswd,
						char* szDbName);	
	void		CloseSerManDB();

	int			ExecuteSQL	(CString strSQL);
	int			Insert		(CString strSQL);
};

#endif COdbcManager_H_
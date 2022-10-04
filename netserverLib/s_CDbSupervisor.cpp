///////////////////////////////////////////////////////////////////////////////
// s_CDbmanager.cpp
//
// class CDbList
// class CDbmanager
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note :
// 
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "s_CDbmanager.h"
#include "GLogicData.h"
#include "s_CCfg.h"
#include <strstream>
#include "GLCharAG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// Class CDbSupervisor
// This class for MS-SQL Server.
// This is rapper class of DB Library for C
// Present, this class only work for MS-SQL Server.
//
///////////////////////////////////////////////////////////////////////////////
CDbSupervisor::CDbSupervisor()
{	
	m_nDBPoolSize	= 5;
	m_nDBTimeOut	= 10;
	m_pUse			= NULL;
	m_pUnUse		= NULL;
	m_pLogin		= NULL;

	memset(m_szServerName, 0, DB_SVR_NAME_LENGTH);
	memset(m_szUsrID, 0, USR_ID_LENGTH);
	memset(m_szUsrPass, 0, USR_PASS_LENGTH);
	memset(m_szDBName, 0, DB_NAME_LENGTH);	
}

CDbSupervisor::~CDbSupervisor()
{
	SAFE_DELETE(m_pUse);
	SAFE_DELETE(m_pUnUse);
}

void CDbSupervisor::SetServerName(const char* szName)
{
	// strcpy(m_szServerName, szName);
	::StringCchCopy(m_szServerName, DB_SVR_NAME_LENGTH, szName);
	
}

void CDbSupervisor::SetUserID(const char* szUsrID)
{
	// strcpy(m_szUsrID, szUsrID);
	::StringCchCopy(m_szUsrID, USR_ID_LENGTH, szUsrID);	
}

void CDbSupervisor::SetUserPass(const char* szUsrPasswd)
{
	::StringCchCopy(m_szUsrPass, USR_PASS_LENGTH, szUsrPasswd);
}

void CDbSupervisor::SetDBName(const char* szDBName)
{
	::StringCchCopy(m_szDBName, DB_NAME_LENGTH, szDBName);
}

void CDbSupervisor::SetDBServer(const char* szName,
								const char* szUsrID,
								const char* szUsrPass,
								const char* szDBName,
								int nPoolSize,
								int nTimeOut)
{
	SetServerName	(szName);
	SetUserID		(szUsrID);
	SetUserPass		(szUsrPass);
	SetDBName		(szDBName);
	SetDBPoolSize	(nPoolSize);
	SetDBTimeOut	(nTimeOut);
}

void CDbSupervisor::SetDBTimeOut(int nTimeSec)
{
	m_nDBTimeOut = nTimeSec;
}

void CDbSupervisor::SetDBPoolSize(int nSize)
{
	if (nSize > 0)
		m_nDBPoolSize = nSize;
}

int CDbSupervisor::Open()
{
	RETCODE nRetCode=0;

	m_pLogin = dblogin();                     // get login record from DB-LIB
	if (m_pLogin == NULL)
		return DB_ERROR;

	DBSETLUSER		(m_pLogin, (char *) m_szUsrID);      // set the username
	DBSETLAPP		(m_pLogin, (char *) "ran_online_server"); // set the application name
	DBSETLPWD		(m_pLogin, (char *) m_szUsrPass);         // set the SQL Server password
	DBSETLVERSION	(m_pLogin, DBVER60);
	if (DBSETLPACKET(m_pLogin, DB_PACKET_SIZE) == FAIL)
	{
		return DB_ERROR;
	}
	// To use secure, or trusted, connection, uncomment the following line.
	// DBSETLSECURE (login);

	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dblibc/dbc_pdc04e_1b8z.asp
	// A DB-Library application has a maximum number of simultaneously open DBPROCESS structures. 
	// Although the default number is 25, the application can change it by calling dbsetmaxprocs.
	// Microsoft¢ç Windows NT¢ç 4.0 Limited by amount of memory available. 
	// Microsoft Windows¢ç 95 or Windows 98 Limited by amount of memory available. 
	// Windows  45 for all Microsoft¢ç SQL Server¢â 2000-running applications. 
	// Microsoft MS-DOS¢ç 15. 
	
	// if (nDBPoolSize > 25)
    //    dbsetmaxprocs(nDBPoolSize);
	dbsetmaxprocs(100);
	
	// Old version
	// dberrhandle((DBERRHANDLE_PROC)DB_err_handler);
	// dbmsghandle((DBMSGHANDLE_PROC)DB_msg_handler);
	// New version
	dbprocerrhandle((PDBHANDLE) m_pLogin, (DBERRHANDLE_PROC) DB_err_handler);
	dbprocmsghandle((PDBHANDLE) m_pLogin, (DBMSGHANDLE_PROC) DB_err_handler);

	m_pUse   = new CDbList(m_pLogin, m_szServerName, m_szDBName);
	m_pUnUse = new CDbList(m_pLogin, m_szServerName, m_szDBName, m_nDBPoolSize);
	
	dbsettime(DB_RESPONSE_TIME);
	
	//LockOff();
	return DB_OK;
}

int CDbSupervisor::Open(char* szSvrName, 
					char* szUsrID, 
					char* szUsrPasswd,
					char* szDbName,
					int nPoolSize,
					int nTimeOut)
{
	SetDBServer(szSvrName, szUsrID, szUsrPasswd, szDbName, nPoolSize, nTimeOut);
	return Open();

}

DB_LIST* CDbSupervisor::GetFreeConnection(void)
{
	DB_LIST* temp = m_pUnUse->GetRef();
	
	assert(temp);
	if ( temp == NULL ) return NULL;
	
	m_pUse->AddTail(temp);
	return temp;
}

void CDbSupervisor::ReleaseConnection(DB_LIST* pDB)
{
	m_pUse->RemoveAt ((DB_LIST*) pDB);
	m_pUnUse->AddTail((DB_LIST*) pDB);
}

///////////////////////////////////////////////////////////////////////////////
// s_CServerManagerCfg.h
//
// class CServerManagerCfg
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2005 (c) Mincoms. All rights reserved.                 
// 
// * Note
// Config file load class head file
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CServerManagerCfg_H_
#define CServerManagerCfg_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRSAFE_LIB
#define STRSAFE_NO_DEPRECATE

// Must Install Platform SDK
// Visit and install http://www.microsoft.com/msdownload/platformsdk/sdkupdate/
#include "strsafe.h" // Safe string function

// Auto Link Library Files
#pragma comment (lib,"strsafe.lib")

#define USER_ID_LENGTH				20
#define	USER_PASS_LENGTH				20

#define DB_SVR_NAME_LENGTH			50
#define DB_NAME_LENGTH				50

#define SM_MAX_PHONE_LIST 10

class CServerManagerCfg
{
protected:
	
public:
	static CServerManagerCfg* GetInstance();
	static void	ReleaseInstance();

	void		SetDefault(void);
	int			Process(char* szLine);
	int			Load();
	CString		GetAppPath();

	int         GetSessionPort();

	CString     GetSessionIP();

	char		m_strOdbcName[DB_SVR_NAME_LENGTH+1];
	char		m_strOdbcDBName[DB_NAME_LENGTH+1];
	char		m_strOdbcUserName[USER_ID_LENGTH+1];
	char		m_strOdbcPassword[USER_PASS_LENGTH+1];
	
	
	CString     GetSmsPhoneList(int nList);
	int         GetSmsPhoneListCount();

private:
	CServerManagerCfg();
	virtual ~CServerManagerCfg();
	static CServerManagerCfg* SelfInstance;

protected:
	CString m_strSessionIP;	
	CString m_SmsPhoneList[SM_MAX_PHONE_LIST];	
	int  m_nSessionPort;
	int  m_nSmsPhoneCount;
};

#endif // CServerManagerCfg_H_
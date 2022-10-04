///////////////////////////////////////////////////////////////////////////////
// CServerManagerCfg.cpp
// 
// class CServerManagerCfg
//
// * History
// 2003.09.09 jgkim Create
//
// Copyright 2002-2005 (c) Mincoms. All rights reserved.                 
// 
// * Note 
// Config file load class
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
// #include <stdio.h>
#include <string.h>
#include <time.h>
#include "CManagerCfg.h"
#include "s_CClientConsoleMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CServerManagerCfg* CServerManagerCfg::SelfInstance = NULL;

CServerManagerCfg::CServerManagerCfg()
{
	SetDefault();
}

CServerManagerCfg* CServerManagerCfg::GetInstance()
{
	if (SelfInstance == NULL)
		SelfInstance = new CServerManagerCfg();

	return SelfInstance;
}

void CServerManagerCfg::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		delete SelfInstance;
		SelfInstance = NULL;
	}
}

void CServerManagerCfg::SetDefault(void)
{
	m_nSessionPort   = 0;	
	m_nSmsPhoneCount = 0;
}

CServerManagerCfg::~CServerManagerCfg()
{
}

CString	CServerManagerCfg::GetAppPath ()
{
	CString strFullPath;
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if ( !strCommandLine.IsEmpty() )
	{
		DWORD dwFind = strCommandLine.ReverseFind ( '\\' );
		if ( dwFind != -1 )
		{
			strFullPath = strCommandLine.Left ( dwFind );
			
			if ( !strFullPath.IsEmpty() )
			if ( strFullPath.GetAt(0) == '"' )
				strFullPath = strFullPath.Right ( strFullPath.GetLength() - 1 );
		}
	}

	return strFullPath;
}

int CServerManagerCfg::Load()
{
	// Data type problem
	CString strFileName = GetAppPath() + "\\ServerManager.cfg";
	
	FILE *oFile = NULL;
	char szLine[1024+1];

	// Open for read 
	if ((oFile = fopen(strFileName.GetString(), "r")) == NULL)
	{
		return 0;
	}	

	// Read a line and process
	while (::fgets(szLine, 1024, oFile))
	{
		Process(szLine);
	}
	
	// Close file
	fclose(oFile);
	return 1;
}

int CServerManagerCfg::Process(char* szLine)
{
	// 토큰으로 분리할 분리자...
	// space, comma, tab, new line
	char seps[]   = " ,\t\n";
	char *token;

	// Establish string and get the first token
	token = strtok(szLine, seps);
	while (token != NULL)
	{	
		// 주석일때는 무시...
		if (strcmp(token, "//") == 0) 
		{
			return 0;
		} 
		else if (strcmp(token, "session_ip") == 0) 
		{
			token = strtok(NULL, seps);
			if (token)
			{
				m_strSessionIP = token;
				m_strSessionIP.Trim(_T(" "));
				CClientConsoleMessage::GetInstance()->WriteWithTime(_T("Session IP:%s"), m_strSessionIP);
			}
			else
			{
				return 0;
			}
		}
		else if (strcmp(token, "session_port") == 0)
		{
			token = strtok(NULL, seps);
			if (token) 
			{
				m_nSessionPort = atoi(token);
				CClientConsoleMessage::GetInstance()->WriteWithTime(_T("Session Port:%d"), m_nSessionPort);
			}
			else
			{
				return 0;
			}
		}
		else if (::strcmp(token, "odbc_name") == 0) 
		{
			token = ::strtok(NULL, seps );
			if (token)
				::StringCchCopy(m_strOdbcName, DB_SVR_NAME_LENGTH, token);
			else
				return 0;
		}
		else if (::strcmp(token, "odbc_db_name") == 0) 
		{
			token = ::strtok(NULL, seps );
			if (token)
				::StringCchCopy(m_strOdbcDBName, DB_NAME_LENGTH, token);
			else
				return 0;
		}
		else if (::strcmp(token, "odbc_user_name") == 0) 
		{
			token = ::strtok(NULL, seps );
			if (token)
				::StringCchCopy(m_strOdbcUserName, USER_ID_LENGTH, token);
			else
				return 0;
		}
		else if (::strcmp(token, "odbc_password") == 0) 
		{
			token = ::strtok(NULL, seps );
			if (token)
				::StringCchCopy(m_strOdbcPassword, USER_PASS_LENGTH, token);
			else
				return 0;
		}
		else if (strcmp(token, "sms_phone") == 0)
		{			
			while (token = strtok(NULL, seps))
			{
                m_SmsPhoneList[m_nSmsPhoneCount] = token;
				m_SmsPhoneList[m_nSmsPhoneCount].Trim(_T(" "));				
				m_SmsPhoneList[m_nSmsPhoneCount].Replace("-", "");

				CClientConsoleMessage::GetInstance()->WriteWithTime(_T("SMS Phone %d:%s"), 
					                                                m_nSmsPhoneCount, 
																	m_SmsPhoneList[m_nSmsPhoneCount]);
				m_nSmsPhoneCount++;
			}
			return 0;
		}
		else
		{			
		}
		// Get next token
		token = ::strtok( NULL, seps );
	}
	return 0;
}

int CServerManagerCfg::GetSessionPort()
{ 
	return m_nSessionPort;
}

CString CServerManagerCfg::GetSessionIP()
{
	return m_strSessionIP;
}

CString CServerManagerCfg::GetSmsPhoneList(int nList)
{
	if (nList < 0 || nList >= SM_MAX_PHONE_LIST)
		return NULL;

	return m_SmsPhoneList[nList];
}

int CServerManagerCfg::GetSmsPhoneListCount()
{
	return m_nSmsPhoneCount;
}
///////////////////////////////////////////////////////////////////////////////
// CCfg.cpp
// 
// class CCfg
//
// * History
// 2003.09.09 jgkim Create
//
// Copyright 2002-2004 (c) Mincoms. All rights reserved.                 
// 
// * Note 
// Config file load class
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>
#include <time.h>
#include "CCfg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCfg* CCfg::SelfInstance = NULL;

CCfg::CCfg()
{
	SetDefault();
}

CCfg::CCfg(const char* filename)
{
	SetDefault();
	Load(filename);
}

CCfg* CCfg::GetInstance()
{
	if (SelfInstance == NULL)
		SelfInstance = new CCfg();

	return SelfInstance;
}

void CCfg::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		delete SelfInstance;
		SelfInstance = NULL;
	}
}

void CCfg::SetDefault(void)
{

}

CCfg::~CCfg()
{
}

CString	CCfg::GetAppPath ()
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

int CCfg::Load(const char* filename)
{
	// Data type problem
	// program directory + cfg + filename	
	CString strAppPath;
	char szAppPath[MAX_PATH] = {0};

	strAppPath = GetAppPath ();
	strcpy ( szAppPath, strAppPath.GetString () );
	strcat(szAppPath, "\\");
	strcat(szAppPath, filename);

	FILE *oFile;
	char line[300];

	// Open for read 
	if ((oFile = ::fopen(szAppPath, "r" )) == NULL)
	{
		return -1;
	}	

	// Read a line and process
	while (::fgets(line, 200, oFile))
	{
		Process(line);
	}
	
	// Close file
	if (::fclose( oFile ))
	{
		return -1;
	}	
	return 0;
}

int CCfg::Process(char* sLine)
{
	// 토큰으로 분리할 분리자...
	// space, comma, tab, new line
	char seps[]   = " ,\t\n";
	char *token;

	// Establish string and get the first token
	token = ::strtok(sLine, seps);
	while (token != NULL)
	{	
		// 주석일때는 무시...
		if (::strcmp(token, "//") == 0) 
		{
			return 0;
		} 
		//////////////////////////////////////////////////////////////////////
		// database setting value 
		else if (::strcmp(token, "odbc_name") == 0) 
		{
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szOdbcName, token);
			else
				return 0;
		}
		else if (::strcmp(token, "odbc_db_name") == 0) 
		{
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szOdbcDbName, token);
			else
				return 0;
		}
		else if (::strcmp(token, "odbc_user_name") == 0) 
		{
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szOdbcUserName, token);
			else
				return 0;
		}
		else if (::strcmp(token, "odbc_password") == 0) 
		{
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szOdbcPassword, token);
			else
				return 0;
		}
		//////////////////////////////////////////////////////////////////////////////
		// FTP setting
		else if (::strcmp(token, "ftp_ip") == 0) 
		{
			token = ::strtok(NULL, seps );
			while (token != NULL)
			{
				m_vFtpIP.push_back(token);
				token = ::strtok(NULL, seps );
			}
		}
		else if (::strcmp(token, "ftp_port") == 0) 
		{
			token = ::strtok(NULL, seps);
			if (token)
				m_nFtpPort = atoi(token);
			else
				return 0;
		}
		else if (::strcmp(token, "ftp_user_name") == 0) 
		{
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szFtpUserName, token);
			else
				return 0;
		}
		else if (::strcmp(token, "ftp_password") == 0) 
		{
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szFtpPassword, token);
			else
				return 0;
		}
		// ////////////////////////////////////////////////////////////////////////////
		// Target path
		else if (::strcmp(token, "target_path") == 0) 
		{			
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szTargetPath, token);
			else
				return 0;
		}
		//
		else
		{
			
		}
		// Get next token
		token = ::strtok( NULL, seps );
	}
	return 0;
}


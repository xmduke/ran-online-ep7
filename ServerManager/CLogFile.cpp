///////////////////////////////////////////////////////////////////////////////
// s_CLogFile.cpp
//
// class CLogFile
//
// * History
// 2002.05.30 jgkim Create
// 2003.02.04 jgkim Error 처리 추가
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note 
// 
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdarg.h>     // WIN32 and ANSI 
// #include <varargs.h> // UNIX
#include <stdio.h>
#include <string>
#include "CLogFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLogFile::CLogFile(CString strFileName)
{	
	m_pFile = NULL;
	CString strAppPath;	
	CString strFullFileName;
	char fileName[MAX_FILE_NAME_LENGTH];
	

	strAppPath = GetAppPath ();
	strFullFileName.Format("%s\\%s", strAppPath.GetString(), strFileName.GetString());
	
    struct tm *today;
	int nRetCode = 0;
	time_t ltime;

	// Use time structure to build a customized time string.
	// Use strftime to build a customized time string.
	time(&ltime);
	today = localtime(&ltime);
	strftime(fileName, MAX_PATH, "%Y%m%d%H%M.csv", today);	

	strFullFileName = strFullFileName + fileName;

	if ((m_pFile = fopen(strFullFileName.GetString(), "w")) != NULL)
	{
		m_strFullFileName = strFullFileName;
		fclose(m_pFile);
		m_pFile = NULL;
	}
	else // error
	{
		// fclose(m_pFile);
	}

}

CLogFile::~CLogFile()
{
	if (m_pFile)	fclose(m_pFile);
}

void CLogFile::Write(char *szStr, ...)
{
	char buf[C_BUFFER_SIZE];
	// char sbuf[C_BUFFER_SIZE];
	
	if (szStr == NULL) return;

	va_list ap;
	va_start(ap, szStr);	
	StringCbVPrintf(buf, C_BUFFER_SIZE, szStr, ap);
	va_end(ap);
	
	if (m_pFile == NULL)
		m_pFile = fopen(m_strFullFileName, "at");

	if (m_pFile)
	{		
		// 로그 파일로 쓰기
		fprintf(m_pFile, "%s\n", buf); 
		fclose(m_pFile);
		m_pFile = NULL;
	}
	else
	{
		return; // 에러 발생시 리턴
	}
}

void CLogFile::WriteWithTime(char *szStr, ...)
{
	char buf[C_BUFFER_SIZE];
	char sbuf[C_BUFFER_SIZE];
	
	if (szStr == NULL) return;

	va_list ap;
	va_start(ap, szStr);	
	StringCbVPrintf(buf, C_BUFFER_SIZE, szStr, ap);
	va_end(ap);
	
	if (m_pFile == NULL)
		m_pFile = fopen(m_strFullFileName, "at");

	if (m_pFile)
	{
		CString strTemp(sbuf);
		CTime cTime = CTime::GetCurrentTime();
		CString strTime = cTime.Format("%H:%M:%S");
		CString strWrite = strTime + "," + buf;

		// 로그 파일로 쓰기
		fprintf(m_pFile, "%s\n", strWrite.GetString()); 
		fclose(m_pFile);
		m_pFile = NULL;
	}
	else
	{
		return; // 에러 발생시 리턴
	}
}
CString CLogFile::GetAppPath ()
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

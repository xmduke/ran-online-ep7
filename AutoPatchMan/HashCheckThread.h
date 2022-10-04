#pragma once

#include "./HttpThread.h"

class CHttpPatch;

class CHashCheckThread : public CHttpThread
{
	DECLARE_DYNCREATE(CHashCheckThread)

protected:
	FILEVECTOR	m_vecHashFile;
	FILEVECTOR	m_vecHashFailFile;
	FILEVECTOR	m_vecHashDownFile;
	FILEVECTOR	m_vectorClientFile;

public:
	CHashCheckThread( LPVOID pParam, DWORD nThreadID );

protected:
	virtual void ThreadMain();

protected:
	BOOL CreateHashFile();
	BOOL CheckHashFile();
	BOOL DownloadHashFailFiles( CHttpPatch* pHttpPatch );
	BOOL InstallingHashFailFiles();
	BOOL DeleteDownFiles();
	BOOL LoadListHash();

public:
	CHashCheckThread(){}
	virtual ~CHashCheckThread();

protected:
	DECLARE_MESSAGE_MAP()
};

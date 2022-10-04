#pragma once

#include "LPatchThread.h"

class CHttpPatch;

class CHttpPatchThread : public CLPatchThread
{
	DECLARE_DYNCREATE(CHttpPatchThread)

public:
	CHttpPatchThread( CHttpPatch* pHttpPatch,  const TCHAR * DownloadURL, const TCHAR* SavePath, DWORD nThreadID );

protected:
	virtual void DownloadStart();
	BOOL GETFILE_USEHTTP( CHttpPatch* pHttpPatch, CString strRemoteSubPath, CString strFilePath );

public:
	CHttpPatchThread(){}
	virtual ~CHttpPatchThread(){}

protected:
	DECLARE_MESSAGE_MAP()
};

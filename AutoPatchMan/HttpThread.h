#pragma once

#include "./LPatchThread.h"
#include "./PatchSet.h"
#include "./PatchVariable.h"
#include "./GlobalVariable.h"

class CHttpPatch;
struct S_PATCH_THREAD_PARAM;

class CHttpThread : public CLPatchThread
{
	DECLARE_DYNCREATE(CHttpThread)

protected:
	S_PATCH_THREAD_PARAM * m_pPatchThreadParam;
	DWORD m_nDlgThreadID;

protected:
	FILEVECTOR	m_vectorServerFile;
	CPatchSet	m_ClientFileTree;

	int m_sFailGameVer;

protected:
	BOOL GETFILE_USEHTTP( CHttpPatch* pHttpPatch, std::string strRemoteSubPath, std::string strFileName, CString strTempDir = NS_GLOBAL_VAR::strDownloadTemp );

protected:
	BOOL Initialize()		{ return TRUE; }
	BOOL LoadList();
	BOOL LoadListServer();
	BOOL LoadListClient();

public:
	CHttpThread(){}
	virtual ~CHttpThread(){}

protected:
	DECLARE_MESSAGE_MAP()
};

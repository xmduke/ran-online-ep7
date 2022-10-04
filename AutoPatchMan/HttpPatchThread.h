#pragma once

#include "LPatchThread.h"
#include "GlobalVariable.h"
#include "PatchSet.h"
#include "PatchVariable.h"

class CHttpPatch;
struct	S_PATCH_THREAD_PARAM;

class CHttpPatchThread : public CLPatchThread
{
	DECLARE_DYNCREATE(CHttpPatchThread)

protected:
	S_PATCH_THREAD_PARAM * m_pPatchThreadParam;
	DWORD m_nDlgThreadID;

protected:
	FILEVECTOR	m_vectorServerFile;
	CPatchSet	m_ClientFileTree;
	FILEVECTOR	m_vectorNewFile;
	FILEVECTOR	m_vectorDeleteFile;

	FILEVECTOR	m_vectorDownFile;
	FILEMAP		m_mapDownedFile;
	FILEVECTOR	m_vectorCopyFile;
	FILEMAP		m_mapCopiedFile;

	int m_sFailGameVer;

public:
	CHttpPatchThread( LPVOID pParam, DWORD nThreadID );

protected:
	virtual void ThreadMain();
	BOOL GETFILE_USEHTTP( CHttpPatch* pHttpPatch, std::string strRemoteSubPath, std::string strFileName, CString strTempDir = NS_GLOBAL_VAR::strDownloadTemp );

protected:
	BOOL Initialize(){return TRUE;}
	BOOL LoadList();
	BOOL MakeNewList( const int cPatchVer, const int sPatchVer, const int cGameVer, const int sGameVer );
	BOOL DownloadFilesByHttp( CHttpPatch* pHttpPatch );
	BOOL Installing();
	BOOL DeleteNotFoundFile();
	BOOL DeleteDownFiles();
	BOOL Destroy();
	BOOL VersionUp( int sGameVer );

protected:
	//	Note : Success List 구조
	//		버전 + 파일 개수 + 리스트
	//		모두 SFILENODE 구조체를 이용한다.
	BOOL	SaveDownList( int sGameVer );
	BOOL	LoadDownList();

	BOOL	SaveCopyList( int sGameVer );
	BOOL	LoadCopyList();

public:
	CHttpPatchThread(){}
	virtual ~CHttpPatchThread(){}

protected:
	DECLARE_MESSAGE_MAP()
};

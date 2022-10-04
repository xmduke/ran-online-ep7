// LPatchThread.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PatchPrimeMan.h"
#include "LPatchThread.h"

#include "GlobalVariable.h"
#include "../Common/SUBPATH.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CLPatchThread, CWinThread)

CLPatchThread::CLPatchThread( LPVOID pParam, const TCHAR * szAppPath, DWORD nThreadID) :
	m_bFail(FALSE),
	m_bExtractError(FALSE),
	m_nDlgThreadID(nThreadID)
{
	m_bAutoDelete = FALSE;

	VERIFY( pParam != NULL);
	m_pPatchThreadParam = (S_PATCH_THREAD_PARAM*)pParam;

	StringCchCopy ( m_szAppPath, MAX_PATH, szAppPath );

	m_hEventKill = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_hEventDead = CreateEvent( NULL, TRUE, FALSE, NULL );
}

CLPatchThread::~CLPatchThread()
{
	CloseHandle( m_hEventKill );
	CloseHandle( m_hEventDead );
}

BOOL CLPatchThread::InitInstance()
{
	LauncherPatch();

	return FALSE; // 메세지 루프를 종료하기 위해서 TRUE가 아닌 FALSE를 준다.
}

void CLPatchThread::Delete() // delete 연산자 재정의
{
	CWinThread::Delete();

	VERIFY( SetEvent( m_hEventDead ) );
}

void CLPatchThread::KillThread()
{
	VERIFY( SetEvent( m_hEventKill ));

	SetThreadPriority( THREAD_PRIORITY_ABOVE_NORMAL );
	WaitForSingleObject( m_hEventDead, INFINITE );
	WaitForSingleObject( m_hThread, INFINITE );

	delete this;
}

BOOL CLPatchThread::VersionUp( int sPatchProgramVer )
{
	CString str;

	str.Format ( "%s%s", m_szAppPath, g_szClientVerFile );
	SetFileAttributes ( str.GetString(), FILE_ATTRIBUTE_NORMAL );	

	FILE* cfp = fopen ( str.GetString(), "rb+" );	
	if ( !cfp ) return FALSE;

	if ( 1 != fwrite ( &sPatchProgramVer, sizeof ( int ), 1, cfp ) )
	{
		fclose ( cfp );
		return FALSE;
	}
	
	fclose ( cfp );
	return TRUE;
}

BOOL CLPatchThread::DELETEFILE( BOOL bCab )
{
	CString strTemp;
	strTemp.Format ( "%s%s%s", NS_GLOBAL_VAR::strProFile, SUBPATH::SAVE_ROOT, NS_GLOBAL_VAR::g_szLauncherCabFile );

	if( !bCab ) 
	{
		strTemp = strTemp.Left ( strTemp.ReverseFind ( '.' ) );
	}

	SetFileAttributes ( strTemp.GetString(), FILE_ATTRIBUTE_NORMAL );

	return DeleteFile ( strTemp.GetString() );
}

BOOL CLPatchThread::DeleteLauncherFile()
{
	CString strTemp;
	strTemp.Format ( "%s%s", m_szAppPath, NS_GLOBAL_VAR::g_szLauncherCabFile );
	strTemp = strTemp.Left ( strTemp.ReverseFind ( '.' ) );

	SetFileAttributes ( strTemp.GetString(), FILE_ATTRIBUTE_NORMAL );

	return DeleteFile ( strTemp.GetString() );
}

BOOL CLPatchThread::IsRunning ()
{
	return WaitForSingleObject( m_hEventDead, 0 ) == WAIT_TIMEOUT;
}

BOOL CLPatchThread::IsForceTerminate ()
{
	return !(WaitForSingleObject( m_hEventKill, 0 ) == WAIT_TIMEOUT);
}

BEGIN_MESSAGE_MAP(CLPatchThread, CWinThread)
END_MESSAGE_MAP()


// CLPatchThread 메시지 처리기입니다.

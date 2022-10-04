// LPatchThread.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LPatchThread.h"
#include <strsafe.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CLPatchThread, CWinThread)

CLPatchThread::CLPatchThread( CHttpPatch* pHttpPatch, const TCHAR* DownloadURL, const TCHAR* SavePath, DWORD nThreadID )
	: 	m_bFail(FALSE)
	,	m_bExtractError(FALSE)
	,	m_nDlgThreadID(nThreadID)
{
	m_pHttpPatch = pHttpPatch;
	m_bAutoDelete = FALSE;

	StringCchCopy ( m_DownloadURL, MAX_PATH, DownloadURL );
	StringCchCopy ( m_SavePath, MAX_PATH, SavePath );

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
	// TODO: 여기에서 각 스레드에 대한 초기화를 수행합니다.
	DownloadStart();

	return FALSE;
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

#include "StdAfx.h"

#include "httppatchthread.h"
#include "Resource.h"
#include <strsafe.h>
#include "LogControl.h"
#include "s_CHttpPatch.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const int MAX_HTTP = 24;

IMPLEMENT_DYNCREATE(CHttpPatchThread, CLPatchThread)

CHttpPatchThread::CHttpPatchThread( CHttpPatch* pHttpPatch,  const TCHAR * DownloadURL, const TCHAR* SavePath, DWORD nThreadID )
	:	CLPatchThread( pHttpPatch, DownloadURL, SavePath, nThreadID )
{
}

void CHttpPatchThread::DownloadStart()
{

	CString str;

	if ( IsForceTerminate() ) return;

	// 스레드 종료 코드
	//

	if ( !GETFILE_USEHTTP ( m_pHttpPatch, m_DownloadURL, m_SavePath  ) )
	{
		if ( !IsForceTerminate() )
		{
			SetFail (); // MEMO : 처음부터 다시 다운받아야 한다.
		}
		
		return;
	}

	if ( IsForceTerminate() ) return;

	return;

}

BOOL CHttpPatchThread::GETFILE_USEHTTP( CHttpPatch* pHttpPatch, CString strRemoteSubPath, CString strFilePath )
{
	if ( !pHttpPatch )
	{
		return FALSE;
	}

	if ( !strFilePath.GetLength () )
	{
		return FALSE;
	}

	//	'\\'문자를 '/'로 변경한다.
	CString strRealAddress = strRemoteSubPath;
	strRealAddress.Replace( '\\','/' );
	int index = strRealAddress.GetLength() - (strRealAddress.Find( "//" ) + 2) ;
	strRealAddress = strRealAddress.Right( index );
	strRealAddress = strRealAddress.Left( strRealAddress.ReverseFind( '/' ) );

	index = strRemoteSubPath.ReverseFind ( '/' );
	index = strRemoteSubPath.GetLength() - index - 1 ;

	CString m_strFileName = "/";
	m_strFileName += strRemoteSubPath.Right( index );

	static int nTRY = 0;
	int nTRY_FILESIZE_CHECK = 0;
	int nERROR_RETRY = 0;
	int nADDRESS_NULL_COUNT = 0;

	while ( nTRY_FILESIZE_CHECK < 3 )
	{
		if( IsForceTerminate() ) return FALSE;

		//	NOTE
		//		최대 시도 회수 초과시
		if ( nADDRESS_NULL_COUNT == MAX_HTTP ) return FALSE;

		if ( MAX_HTTP == nTRY ) nTRY = 0;
		if ( nERROR_RETRY == 3 ) return FALSE;		

		static const CString strHTTP = "http://";

		if ( !strRealAddress.GetLength () )
		{
			nADDRESS_NULL_COUNT++;
			nTRY++;
			continue;
		}

		//	널 체크를 통과했다는 것은 nADDRESS_NULL_COUNT을 초기화해야함을 의미한다.
		nADDRESS_NULL_COUNT = 0;

		CString strHttpAddress = strHTTP + strRealAddress;

		if ( NET_ERROR == pHttpPatch->SetBaseURL ( strHttpAddress ) )
		{
			//::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_020, 0 );
			nTRY++;
			nERROR_RETRY++;
			continue;
		}

		if ( NET_ERROR == pHttpPatch->GetFile ( m_strFileName.GetString(), strFilePath.GetString() ) )
		{
			//::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_020, 0 );
			nTRY ++;
			nERROR_RETRY++;
			continue;
		}

		if( IsForceTerminate() ) return FALSE;

		ULONGLONG ulRECEIVED, ulTOTALSIZE;
		NS_LOG_CONTROL::GetProcessCurPosition ( &ulRECEIVED, &ulTOTALSIZE );

		if ( ulRECEIVED != ulTOTALSIZE )
		{
			nTRY_FILESIZE_CHECK++;
			continue;
		}
		else
		{
			return TRUE; // NET_OK, 성공한 것
		}

		Sleep( 0 );
	}

	return FALSE;
}

BEGIN_MESSAGE_MAP(CHttpPatchThread, CLPatchThread)
END_MESSAGE_MAP()
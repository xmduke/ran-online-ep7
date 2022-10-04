#include "StdAfx.h"

#include <algorithm>

#include "httppatchthread.h"
#include "Resource.h"

#include "s_CHttpPatch.h"
#include "GlobalVariable.h"
#include "LogControl.h"
#include "CompactFdi.h"
#include "RANPARAM.h"
#include "../Common/SUBPATH.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CHttpPatchThread, CLPatchThread)

CHttpPatchThread::CHttpPatchThread( LPVOID pParam, const TCHAR * szAppPath, DWORD nThreadID ) :
	CLPatchThread( pParam, szAppPath, nThreadID )
{
}

void CHttpPatchThread::LauncherPatch()
{
	CHttpPatch* pHttpPatch	= m_pPatchThreadParam->pHttpPatch;
	const int sPatchVer		= m_pPatchThreadParam->sPatchVer;

	CString str;

	if ( IsForceTerminate() ) return;

	// 스레드 종료 코드
	//
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_011, 0 );
	
	if ( !GETFILE_USEHTTP ( pHttpPatch, "\\", NS_GLOBAL_VAR::g_szLauncherCabFile, "" ) )
	{
		if ( !IsForceTerminate() )
		{
			CHAR * szListMessage = new CHAR[MAX_PATH];
			StringCchCopy( szListMessage, MAX_PATH, pHttpPatch->GetErrMsg() );
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_020, 0 );
			SetFail (); // MEMO : 처음부터 다시 다운받아야 한다.
		}
		goto LFail;
	}
	else ::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_013, 0 );

	//	구버전 Ran Auto Patch 삭제 ( *.exe )
	//	
	DELETEFILE(FALSE); // cab 확장자가 없을경우 false

	if ( IsForceTerminate() ) goto LFail;

	// 구버전 런쳐 파일 삭제
	DeleteLauncherFile();

	//	새 파일 설치
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_014, 0 );

	str.Format ( "%s%s%s", NS_GLOBAL_VAR::strProFile,SUBPATH::SAVE_ROOT, NS_GLOBAL_VAR::g_szLauncherCabFile );
	if ( !Extract ( m_szAppPath, str.GetString() ) )
	{
		SetExtractError(); // Cab 파일 풀기 실패

		if ( !IsForceTerminate() )
		{
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_012, 0 );
			SetFail ();
		}
		goto LFail;
	}
	else ::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_015, 0 );

	if ( IsForceTerminate() ) goto LFail;

	//	Ran Auto Patch 버전 올리기
	//
	if ( !VersionUp( sPatchVer ) )
	{		
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_016, 0 );
		goto LFail;
	}
	else ::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_017, 0 );

	DELETEFILE(); // 다운로드 한 Ran Auto Patch 삭제 ( *.exe.cab )

	return;

LFail:
	DELETEFILE(); // 다운로드 한 Ran Auto Patch 삭제 ( *.exe.cab )
}

BOOL CHttpPatchThread::GETFILE_USEHTTP( CHttpPatch* pHttpPatch, std::string strRemoteSubPath, std::string strFileName, CString strTempDir )
{
	if ( !pHttpPatch )
	{
		GASSERT ( 0 && "잘못된 연산을 수행할 수 있습니다." );
		return FALSE;
	}

	if ( !strFileName.size () )
	{
		GASSERT ( 0 && "파일이 지정되지 않았습니다." );
		return FALSE;
	}

	//	'\\'문자를 '/'로 변경한다.
	std::replace ( strRemoteSubPath.begin(), strRemoteSubPath.end(), '\\','/' );	

	// MEMO : 패스 설정
	#if !defined(_DEBUG) && !defined(DAUMPARAM) &&!defined(GS_PARAM)
	{
		if( m_pPatchThreadParam->sGameVer != -1 )
		{
			CString strFolder;
			strFolder.Format( "/%04d", m_pPatchThreadParam->sGameVer );
			strFolder += strRemoteSubPath.c_str();
			strRemoteSubPath = strFolder;
		}
	}
	#endif

	strRemoteSubPath += strFileName;

	std::string strLocalFullPath;
	strLocalFullPath += NS_GLOBAL_VAR::strProFile.GetString();	
	strLocalFullPath += SUBPATH::SAVE_ROOT;
	strLocalFullPath += strTempDir.GetString();
	strLocalFullPath += strFileName;

	static int nTRY = 0;
	int nTRY_FILESIZE_CHECK = 0;
	int nERROR_RETRY = 0;
	int nADDRESS_NULL_COUNT = 0;

	while ( nTRY_FILESIZE_CHECK < 3 )
	{
		if( IsForceTerminate() ) return FALSE;

		//	NOTE
		//		최대 시도 회수 초과시
		if ( nADDRESS_NULL_COUNT == RANPARAM::MAX_HTTP ) return FALSE;

		if ( RANPARAM::MAX_HTTP == nTRY ) nTRY = 0;
		if ( nERROR_RETRY == 3 ) return FALSE;		

		static const CString strHTTP = "http://";

		CString strRealAddress = RANPARAM::HttpAddressTable[nTRY];
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

		if ( NET_ERROR == pHttpPatch->GetFile ( strRemoteSubPath.c_str(), strLocalFullPath.c_str() ) )
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
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_018, 0 );
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
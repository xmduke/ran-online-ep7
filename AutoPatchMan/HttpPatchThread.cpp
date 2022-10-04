#include "StdAfx.h"
#include <algorithm>
#include "httppatchthread.h"
#include "Resource.h"

#include "s_CHttpPatch.h"
#include "RANPARAM.h"
#include "LogControl.h"
#include "CompactFdi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CHttpPatchThread, CLPatchThread)

CHttpPatchThread::CHttpPatchThread( LPVOID pParam, DWORD nThreadID ) :
	m_nDlgThreadID(nThreadID),
	m_sFailGameVer(0)
{
	VERIFY( pParam != NULL);
	m_pPatchThreadParam = (S_PATCH_THREAD_PARAM *)pParam;
}

void CHttpPatchThread::ThreadMain()
{
	CHttpPatch* pHttpPatch	= m_pPatchThreadParam->pHttpPatch;
	const bool	bUseHttp	= m_pPatchThreadParam->bUseHttp;
	const int	cPatchVer	= m_pPatchThreadParam->cPatchVer;
	const int	sPatchVer	= m_pPatchThreadParam->sPatchVer;
	const int	cGameVer	= m_pPatchThreadParam->cGameVer;
	const int	sGameVer	= m_pPatchThreadParam->sGameVer;	

	CString cFileList, sFileList, str, strTemp;
	BOOL bInstalling(FALSE);

	NS_LOG_CONTROL::SetProcessAllPosition ( 0, 100 );

	if ( IsForceTerminate() ) return ;

	// 리스트 전송중
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_049, 0 );
	// 파일 리스트 다운로드
	if ( !GETFILE_USEHTTP ( pHttpPatch, "\\", NS_GLOBAL_VAR::strServerCabFileList.GetString(), "" ) )
	{	
		if ( !IsForceTerminate() )
		{
			// 리스트 전송중 오류
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_050, 0 );
		}

		goto LFail;
	}

	if ( IsForceTerminate() ) goto LFail;

	//	압축 풀기위한 전체 경로 설정 및 압축 풀기&설치
	str = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strServerCabFileList;
	// 리스트 압축 풀고 있음
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_051, 0 );
	if ( !Extract ( NS_GLOBAL_VAR::strAppPath.GetString (), str.GetString() ) )
	{
		// 리스트 압축풀기 실패
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_052, 0 );
		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 4, 100 );

	//Initialize(); // MEMO : 함수 구현 안됨.

	if ( IsForceTerminate() ) goto LFail;

	// 파일 리스트 비교 및 새 목록 작성
	// 패치 파일 리스트 로드
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_053, 0 );
	if ( !LoadList() )
	{
		// 리스트 로드에 실패
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_054, 0 );
		goto LFail;
	}

	if ( IsForceTerminate() ) goto LFail;

	// 패치 파일 리스트 비교 진행
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_057, 0 );
	if ( !MakeNewList ( cPatchVer, sPatchVer, cGameVer, sGameVer ) )
	{
		// 리스트 비교에 실패
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_058, 0 );
		goto LFail;
	}    

	NS_LOG_CONTROL::SetProcessAllPosition ( 10, 100 );

	if ( IsForceTerminate() ) goto LFail;

	//	새 목록에 나온 파일 다운 로드
	str = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp;
	CreateDirectory ( str.GetString(), NULL );
	if ( !DownloadFilesByHttp ( pHttpPatch ) )
	{
		if ( !IsForceTerminate() )
		{
			// 패치 파일 전송중 오류
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_055, 0 );
		}

		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 70, 100 );

	if ( IsForceTerminate() ) goto LFail;

	//	파일 복사
	if ( !Installing() )
	{	
		//	강제 종료가 아닌 진짜 오류
		//
		if ( IsExtractError() )
		{
			// 패치 파일중 일부 손상
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_059, 0 );

			//	Note : DS list 삭제
			//
			CString strDownList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownList;
			DeleteFile ( strDownList.GetString() );

			// MEMO : 카피 리스트로 비교후에 설치된 파일은 다운로드 받지 않아도 된다.
			//
			CString strCopyList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strCopyList;
			CopyFile( strCopyList.GetString(), strDownList.GetString(), FALSE );
		}

		bInstalling = TRUE; // 설치중이다.

		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 90, 100 );

	//	<--	클라이언트 목록를 서버목록으로 교체	-->	//
	cFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szClientFileList;
	sFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szServerFileList;

	DeleteFile( cFileList.GetString () );
	MoveFile( sFileList.GetString(), cFileList.GetString() );

	//	Note : DS list 삭제
	//	
	strTemp = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownList;
	DeleteFile( strTemp.GetString() );

	// Note : 카피 리스트 삭제
	//
	strTemp = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strCopyList;
	DeleteFile ( strTemp.GetString() );

	sFileList.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::strServerCabFileList );
	DeleteFile ( sFileList.GetString() );

	DeleteDownFiles();
	DeleteNotFoundFile();

	str = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp;
	RemoveDirectory ( str.GetString() );

	Destroy ();	

	// 클라이언트 버전 갱신
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_060, 0 );
	if ( !VersionUp ( sGameVer ) )
	{
		// 클라이언트 버전 갱신 실패
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_061, 0 );
		goto LFail;
	}

	// 모든 패치 성공
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_062, 0 );
	NS_LOG_CONTROL::SetProcessAllPosition ( 100, 100 );

	return ;

LFail:
	//	다운로드할 파일들이 손상되어 실패한 경우에는
	//	리스트를 처음부터 다시 작성해야하기때문에
	//	리스트를 저장하지 않는다.
	if ( IsExtractError () )
	{
		if ( IsForceTerminate () )
		{
			SaveDownList ( sGameVer );
		}

		SaveCopyList ( sGameVer ); // MEMO :설치한 리스트는 설치중에 취소되었을때는 무조건 저장한다.
	}
	else
	{
		SaveDownList ( sGameVer );

		if( bInstalling ) SaveCopyList ( sGameVer ); // MEMO :설치중에 중단되면 카피 리스트 작성.
	}

	sFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szServerFileList;
	DeleteFile ( sFileList.GetString() );
	sFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strServerCabFileList;
	DeleteFile ( sFileList.GetString() );	

	Destroy ();

	sFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szServerFileList;
	DeleteFile ( sFileList.GetString() );

	SetFail();

	return ;
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

	std::string strSubPath( strRemoteSubPath );

	//	'\\'문자를 '/'로 변경한다.
	std::replace ( strSubPath.begin(), strSubPath.end(), '\\','/' );

	#if !defined(_DEBUG) && !defined(DAUMPARAM)
	{
		CString strFolder;
		strFolder.Format( "/%04d", m_pPatchThreadParam->sGameVer );
		strFolder += strSubPath.c_str();
		strSubPath = strFolder;
	}
	#endif

	strSubPath += strFileName;

	std::string strLocalFullPath;
	strLocalFullPath += NS_GLOBAL_VAR::strAppPath.GetString();	
	strLocalFullPath += strTempDir.GetString();
	strLocalFullPath += strFileName;

	static int nTRY = 0;
	int nTRY_FILESIZE_CHECK = 0;
	int nERROR_RETRY = 0;
	int nADDRESS_NULL_COUNT = 0;
	while ( nTRY_FILESIZE_CHECK < 3 )
	{
		Sleep( 0 );

		//	강제 종료시
		if ( IsForceTerminate () ) return FALSE;		

		//	NOTE
		//		최대 시도 회수 초과시
		if ( nADDRESS_NULL_COUNT == RANPARAM::MAX_HTTP ) return FALSE;

		if ( RANPARAM::MAX_HTTP == nTRY ) nTRY = 0;
		if ( nERROR_RETRY == 5 ) return FALSE;		

		static const CString strHTTP = "http://";

		CString strRealAddress = RANPARAM::HttpAddressTable[nTRY];
		if ( !strRealAddress.GetLength () )
		{
			nADDRESS_NULL_COUNT++;		//	MAX_HTTP가 모두 널인가?
			nTRY++;
			continue;
		}

		//	널 체크를 통과했다는 것은 nADDRESS_NULL_COUNT을 초기화해야함을 의미한다.
		nADDRESS_NULL_COUNT = 0;

		CString strHttpAddress = strHTTP + strRealAddress; // "http://ranp.daumgame.com"

		if ( NET_ERROR == pHttpPatch->SetBaseURL ( strHttpAddress ) )
		{			
			//NS_LOG_CONTROL::Write ( pHttpPatch->GetErrMsg() );
			nTRY++;
			nERROR_RETRY++;
			continue;
		}

		if ( NET_ERROR == pHttpPatch->GetFile ( strSubPath.c_str(), strLocalFullPath.c_str() ) )
		{
			//NS_LOG_CONTROL::Write ( pHttpPatch->GetErrMsg() );

			CString	strMsg;
			strMsg.LoadString( IDS_MESSAGE_047 );
			CHAR * szListMessage = new CHAR[256];
			wsprintf( szListMessage, "%s %s", strMsg.GetString(), strFileName.c_str() );
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );

			nTRY ++;
			nERROR_RETRY++;
			continue;
		}

		//	강제 종료시
		if ( IsForceTerminate () ) return FALSE;	

		ULONGLONG ulRECEIVED, ulTOTALSIZE;
		NS_LOG_CONTROL::GetProcessCurPosition ( &ulRECEIVED, &ulTOTALSIZE );

		if ( ulRECEIVED != ulTOTALSIZE )
		{
			nTRY_FILESIZE_CHECK++;
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_046, 0 ); // 전송 받는 파일 크기가 맞지 않다.
			continue;
		}
		else
		{
			//NOTE	
			//	NET_OK
			//	성공한 것
			return TRUE;
		}        
	}

	return FALSE;
}

BEGIN_MESSAGE_MAP(CHttpPatchThread, CLPatchThread)
END_MESSAGE_MAP()
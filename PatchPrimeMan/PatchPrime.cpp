#include "StdAfx.h"
#include "Resource.h"
#include "patchprime.h"

#include <afxinet.h>
#include "s_NetGlobal.h"
#include "s_NetClient.h"
#include "s_CConsoleMessage.h"
#include "s_CPatch.h"
#include "CompactFdi.h"
#include "RANPARAM.h"
#include "s_CHttpPatch.h"
#include "PatchThread.h"
#include "GlobalVariable.h"
#include "LogControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//char	g_szLauncherCabFile[] = "launcher4d.exe.cab";
extern	char*	g_szClientVerFile;
extern	char	g_szAppPath[1024];

DWORD	PatchByFTP ( S_PATCH_THREAD_PARAM* pParam )
{
	return PatchByHTTP ( pParam );
}

DWORD	PatchByHTTP ( S_PATCH_THREAD_PARAM* pParam )
{
	//CPatch*		pFtpPatch = pParam->pFtpPatch;
	CHttpPatch* pHttpPatch = pParam->pHttpPatch;
	const bool bUseHttp = pParam->bUseHttp;
	//const int cPatchVer = pParam->cPatchVer;
	const int sPatchVer = pParam->sPatchVer;
	//const int cGameVer = pParam->cGameVer;
	//const int sGameVer = pParam->sGameVer;

	CString str;

	//pFtpPatch->SetLocalDir ( g_szAppPath );

	// 스레드 종료 코드
	//
	if ( NS_PATCH_THREAD::IsForceTerminate () ) return 0;

	if ( bUseHttp )
	{
		NS_LOG_CONTROL::Write ( IDS_MESSAGE_011 ); // 파일을 다운로드

		if ( !GETFILE_USEHTTP ( pHttpPatch, "\\", NS_GLOBAL_VAR::g_szLauncherCabFile, "" ) )
		{
			if ( !NS_PATCH_THREAD::IsForceTerminate () )
			{
				NS_LOG_CONTROL::Write ( IDS_MESSAGE_020 ); // 다운로드 실패
				NS_PATCH_THREAD::SetFail (); // MEMO : 처음부터 다시 다운받아야 한다.
			}

			goto LFail;
		}
		else NS_LOG_CONTROL::Write ( IDS_MESSAGE_013 ); // 다운로드 성공
	}
	//else
	//{
	//	//	Ran Auto Patch 가져오기
	//	//	
	//	pFtpPatch->SetCurrentDirectory ( "\\" );	//	루트로 이동
	//	if ( pFtpPatch->GetFile ( NS_GLOBAL_VAR::g_szLauncherCabFile ) == NET_ERROR )
	//	{
	//		NS_LOG_CONTROL::Write ( IDS_MESSAGE_012 );
	//		NS_PATCH_THREAD::SetFail ();
	//		return 0;
	//	}
	//}

	// 스레드 종료 코드
	//
	if ( NS_PATCH_THREAD::IsForceTerminate () ) goto LFail;

	//	구버전 Ran Auto Patch 삭제 ( *.exe )
	//	
	DELETEFILE(FALSE); // cab 확장자가 없을경우 false

	//	새 파일 설치
	NS_LOG_CONTROL::Write ( IDS_MESSAGE_014 ); // 파일 설치

	str.Format ( "\"%s%s\"", g_szAppPath, NS_GLOBAL_VAR::g_szLauncherCabFile );
	if ( !Extract ( g_szAppPath, str.GetString() ) )
	{
		NS_PATCH_THREAD::SetExtractError(); // Cab 파일 풀기 실패

		if ( !NS_PATCH_THREAD::IsForceTerminate () )
		{
			NS_LOG_CONTROL::Write ( IDS_MESSAGE_012 ); // 설치 실패
			NS_PATCH_THREAD::SetFail ();
		}

		goto LFail;
	}
	else NS_LOG_CONTROL::Write ( IDS_MESSAGE_015 ); // 설치 성공

	// 스레드 종료 코드
	//
	if ( NS_PATCH_THREAD::IsForceTerminate () ) goto LFail;

	//	Ran Auto Patch 버전 올리기
	//
	if ( !VersionUp( sPatchVer ) )
	{		
		NS_LOG_CONTROL::Write ( IDS_MESSAGE_016 ); // 정보 갱신 실패
		goto LFail;
	}
	else NS_LOG_CONTROL::Write ( IDS_MESSAGE_017 ); // 패치 성공

	DELETEFILE(); // 다운로드 한 Ran Auto Patch 삭제 ( *.exe.cab )

	return 0;

LFail:
	DELETEFILE(); // 다운로드 한 Ran Auto Patch 삭제 ( *.exe.cab )

	//str.Format ( "\"%s%s\"", g_szAppPath, NS_GLOBAL_VAR::g_szLauncherCabFile );
	//str = str.Left ( str.ReverseFind ( '.' ) );

	//CString	Error;
	//Error.Format ( " PatchError" );
	//str += Error;

	//memset ( &si, 0, sizeof ( STARTUPINFO ) );
	//si.cb = sizeof ( STARTUPINFO );
	//si.dwFlags = 0;	
	//CreateProcess ( NULL, str.GetString(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi );

	return 0;
}

BOOL	DELETEFILE(BOOL bCab)
{
	CString strTemp;
	strTemp.Format ( "%s%s", g_szAppPath, NS_GLOBAL_VAR::g_szLauncherCabFile );
	
	if( !bCab ) 
	{
		strTemp = strTemp.Left ( strTemp.ReverseFind ( '.' ) );
	}
	
	SetFileAttributes ( strTemp.GetString(), FILE_ATTRIBUTE_NORMAL );

	return DeleteFile ( strTemp.GetString() );
}

BOOL	VersionUp ( int sPatchProgramVer )
{	
	CString str;

	str.Format ( "%s%s", g_szAppPath, g_szClientVerFile );
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

BOOL	GETFILE_USEHTTP ( CHttpPatch* pHttpPatch, std::string strRemoteSubPath, std::string strFileName, CString strTempDir )
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
	replace ( strRemoteSubPath.begin(), strRemoteSubPath.end(), '\\','/' );	
	strRemoteSubPath += strFileName;
	
	std::string strLocalFullPath;
	strLocalFullPath += g_szAppPath;	
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
		if ( NS_PATCH_THREAD::IsForceTerminate () ) return FALSE;		
        
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
			NS_LOG_CONTROL::Write ( pHttpPatch->GetErrMsg() );
			nTRY++;
			nERROR_RETRY++;
			continue;
		}
		
		if ( NET_ERROR == pHttpPatch->GetFile ( strRemoteSubPath.c_str(), strLocalFullPath.c_str() ) )
		{
			NS_LOG_CONTROL::Write ( pHttpPatch->GetErrMsg() );
			nTRY ++;
			nERROR_RETRY++;
			continue;
		}

		//	강제 종료시
		if ( NS_PATCH_THREAD::IsForceTerminate () ) return FALSE;	

		ULONGLONG ulRECEIVED, ulTOTALSIZE;
		NS_LOG_CONTROL::GetProcessCurPosition ( &ulRECEIVED, &ulTOTALSIZE );
		
		if ( ulRECEIVED != ulTOTALSIZE )
		{
			nTRY_FILESIZE_CHECK++;
			NS_LOG_CONTROL::Write ( IDS_MESSAGE_018 );			
			continue;
		}
		else
		{
			// NET_OK, 성공한 것
			return TRUE;
		}        
	}

	return FALSE;
}

BOOL MAKE_ALL_FILE_NORMAL_ATTRIBUTE ( CString strPath )
{	
	//	모든 파일에 대해서 검사를 진행함.
	if ( !strPath.GetLength () ) return FALSE;
	if ( strPath[strPath.GetLength()-1] != '\\') strPath += "\\";
	strPath += "*.*";

	CFileFind finder;	
	BOOL bWorking = finder.FindFile ( strPath.GetString () );

	CString strFilePath;
	while (bWorking)
	{
		if ( NS_PATCH_THREAD::IsForceTerminate () )	return FALSE;

		bWorking = finder.FindNextFile();

		// skip . and .. files; otherwise, we'd
		// recur infinitely!
		if (finder.IsDots())
			continue;

		strFilePath = finder.GetFilePath ();

		if ( !SetFileAttributes ( strFilePath.GetString(), FILE_ATTRIBUTE_NORMAL ) )
		{
		}

		if ( finder.IsDirectory () )
		{
			if ( !MAKE_ALL_FILE_NORMAL_ATTRIBUTE ( strFilePath ) )
			{
				finder.Close ();
				return FALSE;
			}
		}
		else
		{
		}
	}
	finder.Close ();

	return TRUE;
}
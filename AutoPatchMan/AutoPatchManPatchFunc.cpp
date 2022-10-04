#include "stdafx.h"
#include "AutoPatchMan.h"
#include "AutoPatchManDlg.h"

#include "s_CPatch.h"
#include "CompactFdi.h"

#include "GlobalVariable.h"
#include "LogControl.h"
#include "RANPARAM.h"
#include "LauncherText.h"
#include "../Common/SUBPATH.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void	CAutoPatchManDlg::BEGIN_PATCH ()
{
	if ( m_emPatchState != E_VERSION_CUR )	
	{		
		//	런처패치 실행
		//		
		switch ( BEGIN_LAUNCHER_PATCH () )
		{
		case E_MSG_DOWNLOAD_FAILED:	ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 10 ) );	break; // 패치 파일 가져오기 중 실패
		case E_MSG_EXTRACT_FAILED:	ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 11 ) );	break; // 패치 파일 설치 중 실패
		case E_MSG_SUCCEED:			PostMessage( WM_QUIT );				break; // 현재 프로그램 종료
		}
        //	패치 후 자신 죽이기, 더 이상의 진행을 막기 위함임
		return;
	}

	if ( m_emGameState != E_VERSION_CUR )
	{
		static S_PATCH_THREAD_PARAM sParam;

		//sParam.pFtpPatch = m_pFtpPatch;
		sParam.pHttpPatch = m_pHttpPatch;
		sParam.bUseHttp = m_bUseHttp;
		sParam.cPatchVer = m_cPatchVer;
		sParam.sPatchVer = m_sPatchVer;
		sParam.cGameVer = m_cGameVer;
		sParam.sGameVer = m_sGameVer;

		BeginThread( E_THREAD_PATCH, m_pPatchThread, &sParam );
		SetTimer ( E_START_PATCH, 30, NULL );
	}
}

void	CAutoPatchManDlg::END_PATCH ()
{
	m_ctrlProgressCur.SetPos ( 100 );
	m_ctrlProgressAll.SetPos ( 100 );

#ifdef CH_PARAM
	::ShowWindow( m_ctrlStart.GetSafeHwnd(), SW_SHOW );
	::ShowWindow( m_ctrlRetry.GetSafeHwnd(), SW_HIDE );
#endif

	m_ctrlStart.EnableWindow( TRUE );
	m_ctrlOption.EnableWindow( TRUE );

	//	다시 한번 더 읽기
	RANPARAM::LOAD( NS_GLOBAL_VAR::strAppPath.GetString() );
	
	TCHAR szFullPath[MAX_PATH] = {0};

	StringCchCopy( szFullPath, MAX_PATH, NS_GLOBAL_VAR::strAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::GUI_FILE_ROOT);
	
	CLauncherTextMan::GetInstance().Clear();
	CLauncherTextMan::GetInstance().SetPath(szFullPath);
	CLauncherTextMan::GetInstance().LoadText(RANPARAM::strLauncherText.GetString(), RANPARAM::bXML_USE );
	
	InitDlgText();

	// 게임 진행에 필요한 준비 완료
	ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 12 ) );
}

int	CAutoPatchManDlg::BEGIN_LAUNCHER_PATCH ()
{	
	if ( m_bUseHttp )
	{
		if ( !GETFILE_USEHTTP ( m_pHttpPatch, "\\", NS_GLOBAL_VAR::g_szLauncherPatchCabFile, "" ) )
		{
			return E_MSG_DOWNLOAD_FAILED;
		}
	}

	//	<--	압축 풀기위한 전체 경로 설정 및 압축 풀기&설치	-->	//

	TCHAR szPROFILE[MAX_PATH] = {0};
	SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );

	CString strTemp;
	strTemp.Format ( "%s%s%s", NS_GLOBAL_VAR::strProFile, SUBPATH::SAVE_ROOT, NS_GLOBAL_VAR::g_szLauncherPatchCabFile );
	
	if ( !Extract ( NS_GLOBAL_VAR::strAppPath.GetString(), strTemp.GetString() ) )
	{
		return E_MSG_EXTRACT_FAILED;
	}

	//	패치 프로그램 실행
	CREATEPROCESS_PATCHPRIME();

	return E_MSG_SUCCEED;
}

void	CAutoPatchManDlg::END_LAUNCHER_PATCH ()
{
	//	PatchPrime 파일 삭제 ( *.exe.cab )
	//

	CString strTemp, strTemp2;
	strTemp.Format ( "%s%s%s", NS_GLOBAL_VAR::strProFile, SUBPATH::SAVE_ROOT, NS_GLOBAL_VAR::g_szLauncherPatchCabFile );
	
	{
		CFileFind finder;
		BOOL bFound = finder.FindFile ( strTemp.GetString() );
		if ( bFound )
		{
			SetFileAttributes ( strTemp.GetString(), FILE_ATTRIBUTE_NORMAL );
			DeleteFile ( strTemp.GetString() );

		}
	}

	// 구버젼 런쳐가 받은 PatchPrime 파일 삭제 ( *.exe.cab )
	strTemp2.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath, NS_GLOBAL_VAR::g_szLauncherPatchCabFile );
	
	{
		CFileFind finder;
		BOOL bFound = finder.FindFile ( strTemp2.GetString() );
		if ( bFound )
		{
			SetFileAttributes ( strTemp2.GetString(), FILE_ATTRIBUTE_NORMAL );
			DeleteFile ( strTemp2.GetString() );

		}
	}

	//	PatchPrime 파일 삭제 ( *.exe )
	//
	strTemp.Format ( "%s%s",  NS_GLOBAL_VAR::strAppPath , NS_GLOBAL_VAR::g_szLauncherPatchCabFile );		
	strTemp = strTemp.Left ( strTemp.ReverseFind ( '.' ) );

	CFileFind finder;
	BOOL bFound = finder.FindFile ( strTemp.GetString() );
	if ( bFound )
	{
		SetFileAttributes ( strTemp.GetString(), FILE_ATTRIBUTE_NORMAL );
		DeleteFile ( strTemp.GetString() );
	}
}
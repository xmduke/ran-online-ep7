#include "stdafx.h"
#include "AutoPatchMan.h"
#include "AutoPatchManDlg.h"

#include <afxinet.h>
#include "s_CPatch.h"
#include "RANPARAM.h"
#include "DebugSet.h"

#include "GlobalVariable.h"
#include "LogControl.h"
#include "LauncherText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//BOOL	CAutoPatchManDlg::ConnectFtpServer ( CString strFtpAddress )
//{
//	int nRetCode;	
//
//	Sleep( 500 );
//	return FALSE;
//	
//    
//#ifdef	__ALPHA__
//	nRetCode = m_pFtpPatch->Connect( strFtpAddress.GetString (),
//						21,
//						g_szAlphaFtpID,
//						g_szAlphaFtpPW,
//						RANPARAM::bUsePassiveDN );
//#else
//	nRetCode = m_pFtpPatch->Connect( strFtpAddress.GetString (),
//						21,
//						NS_GLOBAL_VAR::g_szBetaFtpID,
//						NS_GLOBAL_VAR::g_szBetaFtpPW,
//						RANPARAM::bUsePassiveDN );
//#endif
//    	
//	if (nRetCode == NET_ERROR)
//	{
//		return FALSE;
//	}
//	return TRUE;
//}
//
//BOOL	CAutoPatchManDlg::DisconnectFtpServer ()
//{
//	if ( m_pFtpPatch )
//	{
//		m_pFtpPatch->DisConnect();		
//	}
//
//	return TRUE;
//}

void	CAutoPatchManDlg::InitDlgText()
{
	SetDlgItemText(IDC_TITLE_STATIC,ID2LAUNCHERTEXT("IDC_TITLE_STATIC"));
	SetDlgItemText(IDC_BUTTON_START,ID2LAUNCHERTEXT("IDC_BUTTON_START"));
	SetDlgItemText(IDC_BUTTON_RETRY,ID2LAUNCHERTEXT("IDC_BUTTON_RETRY"));
	SetDlgItemText(IDC_READYTOSTART,ID2LAUNCHERTEXT("IDC_READYTOSTART"));
	SetDlgItemText(IDC_BUTTON_PATCH,ID2LAUNCHERTEXT("IDC_BUTTON_PATCH"));
	SetDlgItemText(IDC_CHECK_USE_HTTP,ID2LAUNCHERTEXT("IDC_CHECK_USE_HTTP"));
	SetDlgItemText(IDC_BUTTON_OPTION,ID2LAUNCHERTEXT("IDC_BUTTON_OPTION"));
	SetDlgItemText(IDC_BUTTON_EXIT,ID2LAUNCHERTEXT("IDC_BUTTON_EXIT"));
}

void	CAutoPatchManDlg::SetAppPath ()
{
	// Note : 실행파일의 경로를 찾아서 저장한다.
	//
	CString strAppPath;
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if ( !strCommandLine.IsEmpty() )
	{
		strAppPath = strCommandLine.Left ( strCommandLine.ReverseFind ( '\\' ) );
		
		if ( !strAppPath.IsEmpty() )
		if ( strAppPath.GetAt(0) == '"' )
			strAppPath = strAppPath.Right ( strAppPath.GetLength() - 1 );

        strAppPath += '\\';
		NS_GLOBAL_VAR::strAppPath = strAppPath.GetString();		
	}
	else 
	{
		MessageBox ( "SetAppPath Error", "Error", MB_OK );
		return;
	}

	TCHAR szPROFILE[MAX_PATH] = {0};
	SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );	
	NS_GLOBAL_VAR::strProFile = szPROFILE;
}

int	CAutoPatchManDlg::CheckVersion ()
{
	m_sPatchVer= m_pNetClient->GetPatchVer ();
	m_sGameVer = m_pNetClient->GetGameVer ();
	
	if (m_sPatchVer == E_CHK_VER_NOT_READY)
	{
		ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 27 ) );
		return E_CHK_VER_NOT_READY; // 버전 전송 실패
	}

	//	NOTE
	//		버전만 확인하고
	//		바로 객체를 삭제함
	//		로그인서버(?)에 계속 붙어있게되는것 방지
	SAFE_DELETE ( m_pNetClient );

	if ( m_sPatchVer == E_CHK_VER_SERVER_UPDATE )
	{
		// 서버 점검중
		ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 8 ) );
		return E_CHK_VER_SERVER_UPDATE;
	}

	//	NOTE
	//		외부 인터페이스에도 버전을 로드하는 부분이 있습니다.
	//		변경될 경우 반드시 체크 해야합니다.
	CString strTemp;
	strTemp.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath, g_szClientVerFile );
	FILE* cfp = fopen ( strTemp.GetString(), "rb" );
	if( !cfp )
	{
		ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 9 ) );
		return E_MSG_FAILED;
	}

	if ( 1 != fread ( &m_cPatchVer, sizeof ( int ), 1, cfp ) )
	{
		fclose ( cfp );	
		
		// 버전 파일 읽기 실패
		ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 9 ) );
		return E_MSG_FAILED;
	}

	if ( 1 != fread ( &m_cGameVer, sizeof ( int ), 1, cfp ) )
	{
		fclose ( cfp );	
		
		ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 9 ) );
		return E_MSG_FAILED;
	}

	fclose ( cfp );

	////////////////////////////////////////////////////////////////////
	//	Launcher 패치
	if ( m_sPatchVer <= m_cPatchVer )	m_emPatchState = E_VERSION_CUR;
	else								m_emPatchState = E_VERSION_UP;
	////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////
	//	게임 데이타 패치
	if ( m_sGameVer <= m_cGameVer )		m_emGameState = E_VERSION_CUR;
	else								m_emGameState = E_VERSION_UP;
	///////////////////////////////////////////////////////////////////

	return E_MSG_SUCCEED;	
}
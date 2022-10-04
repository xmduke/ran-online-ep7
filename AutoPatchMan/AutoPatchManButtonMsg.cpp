#include "stdafx.h"
#include "AutoPatchMan.h"
#include "AutoPatchManDlg.h"
#include "StringUtils.h"

#include "GlobalVariable.h"
#include "LoginThread.h"
#include "FtpThread.h"

#include "GameSetDlg.h"
#include "LogControl.h"

#include "DaumGameParameter.h"
#include "rol_clipboard.h"

#include "RANPARAM.h"
#include "LauncherText.h"
#include "DlgWarningWhql.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CAutoPatchManDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.    
	m_MouseClickPos = point;
	m_bLBDown = TRUE;
	SetCapture();

	CDialog::OnLButtonDown(nFlags, point);
}


void CAutoPatchManDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if ( !m_bLBDown ) return;

	RECT rect;
	GetWindowRect ( &rect );

	CPoint MovePos;
	MovePos.x = m_MouseClickPos.x - point.x;
	MovePos.y = m_MouseClickPos.y - point.y;
	
	rect.left -= MovePos.x;
	rect.top -= MovePos.y;
	rect.right -= MovePos.x;
	rect.bottom -= MovePos.y;

	MoveWindow ( &rect );

	CDialog::OnMouseMove(nFlags, point);
}

void CAutoPatchManDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if ( m_bLBDown )
	{
		ReleaseCapture ();
		m_bLBDown = FALSE;
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CAutoPatchManDlg::OnBnClickedButtonStart()
{
	CWinApp *pApp = AfxGetApp();
	if ( !pApp )	return;

	HWND hTrayWnd = NULL;
	hTrayWnd = ::FindWindow("Shell_TrayWnd", NULL);
	
	if ( hTrayWnd )
	{
		::SendMessage( hTrayWnd, WM_COMMAND, 419, 0 );		
	}

	CString StrCmdLine = pApp->m_lpCmdLine;

	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "/" );
	STRUTIL::RegisterSeparator( " " );

	CStringArray strCmdArray;
	STRUTIL::StringSeparate ( StrCmdLine, strCmdArray );

	CString strCommand = CString("app") + _T("_") + _T("run") + _T(" ");
	CString strNewCmd = _T("/") + strCommand;

	for ( int i=0; i<strCmdArray.GetCount(); i++ )
	{
		CString strCmd = strCmdArray.GetAt(i);
		CString strNew;

		//	96byte 짜리 cmd는 terra의 ActiveX에서 전달되는 로그인 정보.
#if defined (PH_PARAM) || defined ( VN_PARAM)
		if ( strCmd.GetLength() == 96 )		strNew = _T("TID:") + strCmd;
		else								strNew = strCmd;		
#else // 말레이시아
		if ( strCmd.GetLength() == 92 )		strNew = _T("TID:") + strCmd;
		else								strNew = strCmd;
#endif

		strNewCmd += _T("/") + strNew + _T(" ");
	}

	{
		CString str;
		str.Format ( "\"%s%s\"", NS_GLOBAL_VAR::strAppPath, NS_GLOBAL_VAR::g_szRanFileName);	

		#ifndef KR_PARAM
		{
			rol_clipboard::InitClipboard();
			rol_clipboard::PutClipboard(m_hWnd);
		}
		#endif // !KR_PARAM

		int result = (int)ShellExecute( m_hWnd , "open", str.GetString() ,strNewCmd.GetString(), NULL, SW_SHOW);

		if( result < 32  )
		{
			CString strMsg;
			strMsg.Format( "%s Error Number : %d", ID2LAUNCHERTEXT("IDS_MESSAGE", 2 ), result ); 
			MessageBox ( strMsg.GetString(), "ERROR", MB_OK | MB_ICONEXCLAMATION );

			#ifdef KR_PARAM
			{
				CDaumGameParameter cDGP;
				cDGP.Empty();
			}
			#endif	// KR_PARAM
		}
	}

	OnOK(); // 런처 종료
}

void CAutoPatchManDlg::OnBnClickedButtonExit()
{
	//	다음 게임 파라메타의 내용을 지워줍니다.
	#ifdef KR_PARAM
	{
		CDaumGameParameter cDGP;
		cDGP.Empty();
	}
	#endif	// KR_PARAM

	m_bForceTerminate = TRUE;

	EndThread( m_pWebBrowserThread );
	EndThread( m_pLoginThread );
	EndThread( m_pPatchThread );
	EndThread( m_pHashThread );

	// 종료하고 있습니다.
	ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 3 ));
	OnOK();
}

void CAutoPatchManDlg::OnBnClickedButtonOption()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CGameSetDlg dlg;

#ifdef CH_PARAM
	dlg.m_nDownloadArea = RANPARAM::nChinaRegion;
	dlg.m_strAreaName[0] = RANPARAM::China_Region[0].strName;
	dlg.m_strAreaName[1] = RANPARAM::China_Region[1].strName;
	dlg.m_strAreaName[2] = RANPARAM::China_Region[2].strName;
//	dlg.m_strAreaName[3] = RANPARAM::China_Region[3].strName;
//	dlg.m_strAreaName[4] = RANPARAM::China_Region[4].strName;
//	dlg.m_strAreaName[5] = RANPARAM::China_Region[5].strName; // 중국지역 추가
#endif

	dlg.DoModal ();
}

void CAutoPatchManDlg::OnBnClickedButtonRetry()
{
	//	버튼 막기
#ifdef CH_PARAM
	::ShowWindow( m_ctrlRetry.GetSafeHwnd(), SW_SHOW );
	::ShowWindow( m_ctrlStart.GetSafeHwnd(), SW_HIDE );
	m_ctrlRetry.EnableWindow( FALSE );
#endif

	m_ctrlStart.EnableWindow( FALSE );
	m_ctrlOption.EnableWindow( FALSE );

	RANPARAM::LOAD( NS_GLOBAL_VAR::strAppPath.GetString() );

	EndThread( m_pLoginThread );
	EndThread( m_pPatchThread );
	EndThread( m_pHashThread );

	if( !m_pNetClient )
	{
		m_pNetClient = new CNetClient( m_hWnd );
		if( !m_pNetClient )
		{
			EndDialog(IDOK);
			return;
		}
	}

	m_nLoginTry = 0;
	BEGIN_LOGIN_THREAD ();
}
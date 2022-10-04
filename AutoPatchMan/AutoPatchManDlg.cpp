// AutoPatchManDlg.cpp : 구현 파일
//
#include "stdafx.h"
#include "AutoPatchMan.h"
#include "AutoPatchManDlg.h"

#include "GlobalVariable.h"
#include "RANPARAM.h"
#include "CheckSystem.h"

//#include "WebBrowserThread.h"

#include <afxinet.h>
#include <process.h>
#include "s_CPatch.h"

#include "SUBPATH.h"

#include "RegFunction.h"
#include "StringUtils.h"
#include "LogControl.h"
//#include "FtpThread.h"

#include "DlgWarningWhql.h"
#include "CompactFdi.h"

#include "DlgDownloadArea.h"
#include "LauncherText.h"

#ifndef USE_MFC7_HTMLVIEW_FEATURES
	#include "Custsite.h"
	#include ".\autopatchmandlg.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const UINT CAutoPatchManDlg::nLOGIN_CONNECT_TIME_ELAPSE = 500;
const UINT CAutoPatchManDlg::nCHECK_VERSION_TIME_ELAPSE = 500;
const UINT CAutoPatchManDlg::nFTP_CONNECT_TIME_ELAPSE = 500;
const UINT CAutoPatchManDlg::nFORCE_TERMINATE_TIME_ELAPSE = 500;
const UINT CAutoPatchManDlg::n500_MILISEC = 500;
const UINT CAutoPatchManDlg::n1000_MILISEC = 1000;
const UINT CAutoPatchManDlg::nWEB_BROWSER_START = 1000;

const UINT WM_CUSTOM_CONTROLSITE_MSG  = RegisterWindowMessage(_T("LauncherMsg"));

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	virtual BOOL OnInitDialog();

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
protected:
	DECLARE_MESSAGE_MAP()
private:
	void InitDlgText();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitDlgText();
	
	return TRUE;
}
void CAboutDlg::InitDlgText()
{
//	SetDlgItemText(IDD_ABOUT_BOX,ID2LAUNCHERTEXT("IDD_ABOUT_BOX"));
	SetWindowText(ID2LAUNCHERTEXT("IDD_ABOUT_BOX"));
	SetDlgItemText(IDC_VER_STATIC,ID2LAUNCHERTEXT("IDC_VER_STATIC"));
	SetDlgItemText(IDOK,ID2LAUNCHERTEXT("ABOUT_BOX_ID_OK"));	
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAutoPatchManDlg 대화 상자



CAutoPatchManDlg::CAutoPatchManDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoPatchManDlg::IDD, pParent),
	m_pNetClient( NULL ),
	//m_pFtpPatch( NULL ),
	m_pHttpPatch( NULL ),
	m_bForceTerminate( FALSE ),
	m_emPatchState( E_VERSION_CUR ),
	m_emGameState( E_VERSION_CUR ),
	m_bUseHttp( FALSE ),
	m_pWebBrowserThread(NULL),
	m_pLoginThread(NULL),
	m_pPatchThread(NULL),
	m_pHashThread(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAutoPatchManDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_CUR, m_ctrlProgressCur);
	DDX_Control(pDX, IDC_PROGRESS_ALL, m_ctrlProgressAll);
	DDX_Control(pDX, IDC_STATIC_NOTIFY, m_ctrlNotify);
	DDX_Control(pDX, IDC_BUTTON_OPTION, m_ctrlOption);
	DDX_Control(pDX, IDC_BUTTON_START, m_ctrlStart);
#ifdef CH_PARAM
	DDX_Control(pDX, IDC_BUTTON_RETRY, m_ctrlRetry);
#endif
	DDX_Control(pDX, IDC_BUTTON_EXIT, m_ctrlExit);
	DDX_Control(pDX, IDC_LIST_LOG, m_ctrlListBox);
	DDX_Control(pDX, IDC_READYTOSTART, m_ctrlCheckBox);
}

BEGIN_MESSAGE_MAP(CAutoPatchManDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_START, OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_OPTION, OnBnClickedButtonOption)
	ON_BN_CLICKED(IDC_BUTTON_RETRY, OnBnClickedButtonRetry)

	ON_REGISTERED_MESSAGE( WM_CUSTOM_CONTROLSITE_MSG, OnCustomControlSiteMsg)
	ON_BN_CLICKED(IDC_BUTTON_PATCH, OnBnClickedButtonPatch)
	ON_LBN_DBLCLK(IDC_LIST_LOG, OnLbnDblclkListLog)
END_MESSAGE_MAP()


// CAutoPatchManDlg 메시지 처리기

BOOL CAutoPatchManDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu = ID2LAUNCHERTEXT("IDS_ABOUTBOX", 0 );
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	//InitializeCriticalSection ( &NS_PATCH_THREAD::CRITICAL_SECTION_INSTANCE ); //필요가 없을것 같은...

	//	NOTE
	//		초기화
	m_bLBDown = FALSE;	
	SetWindowText ( NS_GLOBAL_VAR::strTitle.GetString() );
	SetAppPath ();	//	디렉토리 경로 설정


	//	버튼 막기
#ifdef CH_PARAM
	::ShowWindow( m_ctrlRetry.GetSafeHwnd(), SW_SHOW );
	::ShowWindow( m_ctrlStart.GetSafeHwnd(), SW_HIDE );
	m_ctrlRetry.EnableWindow( FALSE );
#else
	::ShowWindow( m_ctrlRetry.GetSafeHwnd(), SW_HIDE );
#endif


	m_ctrlStart.EnableWindow( FALSE );
	m_ctrlOption.EnableWindow( FALSE );

	RANPARAM::LOAD( NS_GLOBAL_VAR::strAppPath.GetString() );

	TCHAR szFullPath[MAX_PATH] = {0};

	StringCchCopy( szFullPath, MAX_PATH, NS_GLOBAL_VAR::strAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::GUI_FILE_ROOT);
	
	CLauncherTextMan::GetInstance().SetPath(szFullPath);
	CLauncherTextMan::GetInstance().LoadText(RANPARAM::strLauncherText.GetString(), RANPARAM::bXML_USE );
	
	InitDlgText();

#ifdef CH_PARAM
	// 중국 - 패치 지역을 읽어서 디폴트 값이면 다운로드 설정 다이얼로그를 연다.
	if( RANPARAM::nChinaRegion == RANPARAM::MAX_CHINA_REGION )
	{
		// 다이얼로그 오픈
		CDlgDownloadArea dlg;
		dlg.m_strAreaName[0] = RANPARAM::China_Region[0].strName;
		dlg.m_strAreaName[1] = RANPARAM::China_Region[1].strName;
		dlg.m_strAreaName[2] = RANPARAM::China_Region[2].strName;
//		dlg.m_strAreaName[3] = RANPARAM::China_Region[3].strName;
//		dlg.m_strAreaName[4] = RANPARAM::China_Region[4].strName;
//		dlg.m_strAreaName[5] = RANPARAM::China_Region[5].strName; // 중국지역 추가

		dlg.DoModal();
		
		RANPARAM::nChinaRegion = dlg.m_nDownloadArea;

		StringCchCopy( RANPARAM::LoginAddress, STRING_NUM_128, RANPARAM::China_Region[RANPARAM::nChinaRegion].LoginAddress.GetString() );

		for( int i=0 ; ; ++i )
		{
			if( RANPARAM::China_Region[RANPARAM::nChinaRegion].HttpAddressTable[i].IsEmpty() ) 
				break;

			StringCchCopy( RANPARAM::HttpAddressTable[i], STRING_NUM_128, RANPARAM::China_Region[RANPARAM::nChinaRegion].HttpAddressTable[i].GetString() );
		}

		RANPARAM::strNewsURL = RANPARAM::China_Region[RANPARAM::nChinaRegion].strNewsURL;

		RANPARAM::SAVE();

		dlg.ShowWindow( SW_HIDE );
	}
#endif

	m_strNewsURL = RANPARAM::strNewsURL;

/*	//		Japan 로그인 방식 변경
#ifdef JP_PARAM
	CWinApp *pApp = AfxGetApp();
	if( !pApp )	return FALSE;

	CString StrCmdLine = pApp->m_lpCmdLine;

	STRUTIL::ClearSeparator ();
	STRUTIL::RegisterSeparator ( "/" );
	STRUTIL::RegisterSeparator ( " " );

	CStringArray strCmdArray;
	STRUTIL::StringSeparate ( StrCmdLine, strCmdArray );

	CString strTemp = strCmdArray.GetAt(0);

	CString strVALUE, strKEY;
	int nPos(-1);

	strKEY = "U=";
	nPos = strTemp.Find( strKEY );
	if ( nPos!=-1 )
	{
		strVALUE = strTemp.Right ( strTemp.GetLength()-strKEY.GetLength() );

		// 일본 런쳐에서 커맨드 라인에 U=01101@nm 일 경우
		// NewsURL 뒤에 nm을 붙여서 브라우저를 동작 시킨다.
		// 일본 넷마블에서 로그인 할때 다른 주소를 붙여주기 위함이다.
		m_strNewsURL += "?d=";

		nPos = strVALUE.Find( _T('@') );
		if( nPos != -1 )
		{
			m_strNewsURL += strVALUE.Mid( nPos+1, 2 );
		}
	}
#endif
*/
	//	브라우저 동작
	SetTimer( E_WEB_BROWSER, nWEB_BROWSER_START, NULL );
	//BeginThread( E_THREAD_WEB, m_pWebBrowserThread, &m_ctrlNotify, strNewsURL );

	//	Note : 디버그샛의 초기화.
	//
	TCHAR szPROFILE[MAX_PATH] = {0};
	SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );
	CDebugSet::OneTimeSceneInit ( szPROFILE, true );

	//	주의!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//	프로그램 실행 경로가 필요한 경우 'SetAppPath()'가 호출된 이후에
	//	코드를 작성해야한다.
	END_LAUNCHER_PATCH ();

	//////////////////////////////////////////////////////////////////////////////	
	// Initilaize Winsock2
	int Ret = NET_InitializeSocket();
	if ( Ret == NET_ERROR )
	{
		// 윈속 버전이 낮다.	
		ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 5 ) );
		EndDialog(IDOK);
	}
	//////////////////////////////////////////////////////////////////////////////
	
	// NOTE
	//		기본 데이타 형성
	m_pNetClient = new CNetClient( m_hWnd );
	if( NULL == m_pNetClient ) EndDialog(IDOK);
	m_pHttpPatch = new CHttpPatch;
	if( NULL == m_pHttpPatch ) EndDialog(IDOK);
	//m_pFtpPatch = new CPatch;

	//	강제종료 토글 설정
	//
	//m_pFtpPatch->SetForceTerminateToggle ( &m_bForceTerminate );
	m_pHttpPatch->SetForceTerminateToggle ( &m_bForceTerminate );
	SetCabForceTerminate ( &m_bForceTerminate );

	//	진행상황 설정
	ULONGLONG* pCurPos = NULL;
	ULONGLONG* pCurEnd = NULL;
	NS_LOG_CONTROL::GetProcessCurPositionAddr ( &pCurPos, &pCurEnd );	
	//m_pFtpPatch->SetProgressValue ( pCurPos, pCurEnd );
	m_pHttpPatch->SetProgressValue ( pCurPos, pCurEnd );
	SetCabProgressValue ( pCurPos, pCurEnd );

	//	Note : 로그인 서버 접속 -> 버전검사 -> 패치
	//
	m_nLoginTry = 0;
	BEGIN_LOGIN_THREAD ();

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void	CAutoPatchManDlg::UPDATE_TIME ( const UINT nElapsedTime )
{
	if ( 0.0f <= m_nLIFE_TIME )	m_nLIFE_TIME -= nElapsedTime;
}

void CAutoPatchManDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CAutoPatchManDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CAutoPatchManDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAutoPatchManDlg::PostNcDestroy()
{
	EndThread( m_pWebBrowserThread );
	EndThread( m_pLoginThread );
	EndThread( m_pPatchThread );
	EndThread( m_pHashThread );

	//	FTP 연결 종료
	//m_pFtpPatch->DisConnect ();

	SAFE_DELETE ( m_pNetClient );
	SAFE_DELETE ( m_pHttpPatch );
	//SAFE_DELETE ( m_pFtpPatch );

	NET_CloseSocket();

	CDebugSet::FinalCleanup ();

	//DeleteCriticalSection ( &NS_PATCH_THREAD::CRITICAL_SECTION_INSTANCE );
	
	CDialog::PostNcDestroy();
}

BOOL CAutoPatchManDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	switch ( pMsg->message )
	{
	case WM_KEYDOWN:
		{
			switch ( pMsg->wParam )
			{
			case VK_RETURN:
			case VK_ESCAPE:
				return TRUE;
			}
		}
		break;

	case WM_SYSKEYDOWN:
		{
			if ( pMsg->wParam == VK_F4 )
			{
				OnBnClickedButtonExit ();
				return TRUE;
			}
		}
		break;

	case WM_LISTADDSTRING:
		{
			if( pMsg->wParam == 0)
			{
				ListAddString( (PCHAR)pMsg->lParam );
				delete [] (PCHAR)pMsg->lParam; // 보낸 메세지의 포인터를 삭제해준다.
			}
			else
			{
//				ListAddString( static_cast<UINT>(pMsg->wParam) );
				ListAddString( (LPCTSTR) pMsg->wParam );
			}

			return TRUE;
		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}


LRESULT CAutoPatchManDlg::OnCustomControlSiteMsg(WPARAM wParam, LPARAM lParam)
{
LRESULT hasBeenHandled = FALSE;

	ASSERT((wParam > kCCSN_NoMessage) && (wParam < kCCSN_MessageLimit));

	switch (wParam)
	{
	case kCCSN_CreateSite:

		if (lParam != NULL)
		{
			kCCSN_CreateSiteParams *params = (kCCSN_CreateSiteParams *)lParam;

			if (params->pCtrlCont != NULL)
			{
				params->pSite = new CCustomControlSite(params->pCtrlCont);
				hasBeenHandled = TRUE;
			}
		}

		break;
	}

	return hasBeenHandled;
}

void CAutoPatchManDlg::OnBnClickedButtonPatch()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//	Note : 로그인 서버 접속 -> 버전검사 -> 패치
	//
	m_nLoginTry = 0;
	BEGIN_LOGIN_THREAD ();
}

void CAutoPatchManDlg::OnLbnDblclkListLog() // 리스트에 있는 정보를 에러로그로 출력
{
	TCHAR szDEBUGDIR[MAX_PATH]={0};
	SHGetSpecialFolderPath( NULL, szDEBUGDIR, CSIDL_PERSONAL, FALSE );
	StringCchCat( szDEBUGDIR, MAX_PATH, SUBPATH::DEBUGINFO_ROOT );

	// Use time structure to build a customized time string.
	time_t ltime;
	struct tm *today;

	time(&ltime);
	today = localtime(&ltime);

	char szFileName[MAX_PATH] = {0};
	// Use strftime to build a customized time string.
	strftime ( szFileName, MAX_PATH, "log.%Y%m%d%H%M.txt", today );	
	
	CString strLogFile;
	strLogFile.Format ( "%s\\%s", szDEBUGDIR, szFileName );

	FILEOUT( strLogFile ); // 로그파일 출력
}

BOOL CAutoPatchManDlg::CREATEPROCESS_PATCHPRIME()
{
	CString strTemp;
	CString strCmdLine;
	CString strFile( NS_GLOBAL_VAR::g_szLauncherPatchCabFile );
	strFile = strFile.Left( strFile.ReverseFind( '.' ) );

    // 런쳐 파라메타 전송( 웹런쳐 및 웹 로그인시 사용 ) 
	CWinApp *pApp = AfxGetApp();
	if( !pApp )	return FALSE;
	strTemp.Format( _T("\"%s%s\""), NS_GLOBAL_VAR::strAppPath, strFile);
	strCmdLine.Format( _T(" %d %d %s"), m_sPatchVer, m_sGameVer, pApp->m_lpCmdLine ); 

	// MEMO : Handles in PROCESS_INFORMATION must be closed with CloseHandle when they are no longer needed.
	if( ! ShellExecute( m_hWnd , "open", strTemp.GetString() ,strCmdLine.GetString(), NULL, SW_SHOW) )
	{
		MessageBox ( "CreateProcess Patch Prime", "ERROR", MB_OK|MB_ICONEXCLAMATION );
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
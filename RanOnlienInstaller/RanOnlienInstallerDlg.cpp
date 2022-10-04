// RanOnlienInstallerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "RanOnlienInstaller.h"
#include "RanOnlienInstallerDlg.h"
#include "HttpPatchThread.h"
#include "s_CHttpPatch.h"
#include "LogControl.h"

const CLSID FOLDERID_LocalAppDataLow = { 0xA520A1A4, 0x1780, 0x4FF6 ,{ 0xBD, 0x18, 0x16, 0x73, 0x43, 0xC5, 0xAF, 0x16}};

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRanOnlienInstallerDlg 대화 상자



CRanOnlienInstallerDlg::CRanOnlienInstallerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRanOnlienInstallerDlg::IDD, pParent)
	, m_bAutoExecute(FALSE)
	, m_bForceTerminate ( FALSE )
	, m_pThread ( NULL )
	, m_ULBefore ( 0UL )
	, m_dwBps ( 0 )
	, m_dwBeforeTime ( 0 )
	, m_dwCntTime ( 0 )
	, m_pHttpPatch ( NULL )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRanOnlienInstallerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXECUTE_BUTTON, m_ctrlExecute);
	DDX_Control(pDX, IDC_START_BUTTON, m_ctrlStart);
	DDX_Control(pDX, IDC_PROGRESS_ALL, m_ctrlProgressAll);
	DDX_Check(pDX, IDC_READYTOSTART, m_bAutoExecute);
}

BEGIN_MESSAGE_MAP(CRanOnlienInstallerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_EXECUTE_BUTTON, OnBnClickedExecuteButton)
	ON_BN_CLICKED(IDC_START_BUTTON, OnBnClickedStartButton)
	ON_BN_CLICKED(IDC_CLOSE_BUTTON, OnBnClickedCloseButton)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CRanOnlienInstallerDlg 메시지 처리기

BOOL CRanOnlienInstallerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	CWinApp *pApp = AfxGetApp();
	if( !pApp )	return FALSE;

	m_DownloadURL = pApp->m_lpCmdLine;

	if ( m_DownloadURL.IsEmpty() )
	{
		OnCancel();
		return FALSE;
	}

	m_ctrlExecute.EnableWindow( FALSE );

	TCHAR szPROFILE[MAX_PATH] = {0};

	SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );
	
	m_strFileName = m_DownloadURL;

	int index = m_strFileName.ReverseFind ( '/' );
	index = m_strFileName.GetLength() - index - 1 ;

	m_strFileName = m_strFileName.Right( index );

	m_strSavePath += szPROFILE;
	m_strSavePath += "\\";

	m_pHttpPatch = new CHttpPatch;
	//	강제종료 토글 설정
	m_pHttpPatch->SetForceTerminateToggle ( &m_bForceTerminate );

	//	진행상황 설정
	ULONGLONG* pCurPos = NULL;
	ULONGLONG* pCurEnd = NULL;
	NS_LOG_CONTROL::GetProcessCurPositionAddr ( &pCurPos, &pCurEnd );	
	m_pHttpPatch->SetProgressValue ( pCurPos, pCurEnd );

	SetDlgItemText( IDC_DOWNPLACE_STATIC, m_strSavePath + m_strFileName );
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CRanOnlienInstallerDlg::OnPaint() 
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
HCURSOR CRanOnlienInstallerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRanOnlienInstallerDlg::OnBnClickedExecuteButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EXECUTE_INSTALL();
}

void CRanOnlienInstallerDlg::OnBnClickedStartButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog dlg( FALSE, (LPCTSTR)".*", m_strFileName.GetString(), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		NULL, this );

	dlg.m_ofn.lpstrInitialDir = m_strSavePath;
	if ( dlg.DoModal() != IDOK )
	{
		return;
	}

	m_strSavePath = dlg.GetPathName();

	m_strSavePath = m_strSavePath.Left ( m_strSavePath.ReverseFind ( '\\' ) + 1 );


	m_strFileName = dlg.GetFileName();	

	// 다운로드 시작

	SetDlgItemText( IDC_DOWNPLACE_STATIC, m_strSavePath + m_strFileName );

	m_ULBefore = 0UL;
	m_dwBps = 0;
	m_dwBeforeTime = 0;
	m_dwCntTime = 0;


	BEGIN_DOWNLOAD();

	return;
}

void CRanOnlienInstallerDlg::OnBnClickedCloseButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bForceTerminate = TRUE; // 전송 중단
	if ( m_pThread )	m_pThread->KillThread(); // 쓰레드 중단

	OnOK();
}

void CRanOnlienInstallerDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch( nIDEvent )
	{
		case E_START_PATCH:
		{
			UpdateControls(); // 프로그래스바 진행 업데이트
			
			if ( m_pThread->IsForceTerminate() )	//	Canceled
			{
				KillTimer ( E_START_PATCH );
			}
			else if ( !m_pThread->IsRunning() )	//	Not Running
			{
				KillTimer ( E_START_PATCH );

				if ( !m_pThread->IsFail () )	//	Failed
				{
					END_DOWNLOAD ();
				}
			}			
		}
		break;

	}

	CDialog::OnTimer(nIDEvent);
}

void CRanOnlienInstallerDlg::UpdateControls()
{
	static const ULONGLONG UL100 = 100UL;
	static const ULONGLONG UL1 = 1UL;
    ULONGLONG Pos = 0;
	ULONGLONG End = 0;
	
	int	AllPercent= 0;	
	
	NS_LOG_CONTROL::GetProcessCurPosition ( &Pos, &End );	
	End = ( !End )? UL1 : End;
	AllPercent = int( (Pos*UL100) / End);


	// 현재까지 받은 용량 
	CString strTemp;	
	float dwPos = static_cast<float>((double)Pos / ( 1024 * 1024 ));
	float dwEnd = static_cast<float>((double)End / ( 1024 * 1024 ));
	strTemp.Format( "%0.2f MB / %0.2f MB ", dwPos, dwEnd );

	SetDlgItemText ( IDC_STATE_STATIC, strTemp );


	// 다운로드 속도
	if ( m_dwBeforeTime == 0 ) 
	{
		m_dwBeforeTime = GetTickCount();
	}
	else
	{
		DWORD dwCurrentTime = GetTickCount();
		m_dwCntTime += dwCurrentTime - m_dwBeforeTime;
		m_dwBeforeTime = dwCurrentTime;
       
		if ( m_dwCntTime > 1000 )
		{
			ULONGLONG ULBps = Pos - m_ULBefore;
			
			CString strTemp2;
			
			if ( ULBps > (1024 * 1024 )) // MB
			{
				m_dwBps = static_cast<float>( (double)ULBps / (1024*1024) );
				strTemp2.Format( "%0.2f MB / 초 ", m_dwBps );
			}
			else // KB
			{
				m_dwBps = static_cast<float>( (double)ULBps / (1024) );
				strTemp2.Format( "%0.2f KB / 초 ", m_dwBps );
			}	
			
			
			m_dwCntTime = 0;
			m_ULBefore = Pos;

			SetDlgItemText ( IDC_SPEED_STATIC, strTemp2 );
		}

	}

    
	m_ctrlProgressAll.SetPos ( AllPercent );
}

void CRanOnlienInstallerDlg::BEGIN_DOWNLOAD()
{
	m_ctrlStart.EnableWindow( FALSE );

	CString strTemp = m_strSavePath + m_strFileName;
	m_pThread = new CHttpPatchThread( m_pHttpPatch, m_DownloadURL.GetString(), strTemp.GetString() , AfxGetThread()->m_nThreadID );

	if (m_pThread == NULL)
		return;

	ASSERT_VALID(m_pThread);
	m_pThread->m_pThreadParams = NULL;

	if ( !m_pThread->CreateThread(CREATE_SUSPENDED) )
	{
		delete m_pThread;
		return;
	}

	VERIFY(m_pThread->SetThreadPriority(THREAD_PRIORITY_IDLE));
	m_pThread->ResumeThread();

	SetTimer ( E_START_PATCH, 30, NULL ); // 쓰레드 생성후 타이머 작동이 맞을까...?
}

void CRanOnlienInstallerDlg::END_DOWNLOAD()
{
	m_ctrlProgressAll.SetPos ( 100 );
	m_ctrlExecute.EnableWindow( TRUE );

	if ( !m_pThread->IsFail() ) // 실패가 아니면...
	{
		UpdateData( TRUE );
		if ( m_bAutoExecute )
		{
			EXECUTE_INSTALL();
		}
	}

}

void CRanOnlienInstallerDlg::EXECUTE_INSTALL()
{
	CString strTemp;

	strTemp = m_strSavePath;
	strTemp += m_strFileName;

	int result = (int)ShellExecute( NULL , "open", strTemp.GetBuffer() ,NULL, NULL, SW_SHOW);

	if( 32 > result )
	{
		MessageBox ( "CreateProcess InstallProgram", "ERROR", MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	OnOK();
}

BOOL CRanOnlienInstallerDlg::IsVista()
{
	OSVERSIONINFO osver;

	osver.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	
	if (	::GetVersionEx( &osver ) && 
			osver.dwPlatformId == VER_PLATFORM_WIN32_NT && 
			(osver.dwMajorVersion >= 6 ) )
		return TRUE;

	return FALSE;
}

void CRanOnlienInstallerDlg::OnKillThread()
{
	if ( !m_pThread ) return;
		
	VERIFY(SetEvent(m_pThread->m_hEventKill));
	
	// wait for all threads to finish shutdown
	BOOL bThreadsLeft(TRUE);

	while (bThreadsLeft)
	{
		Sleep( 1 );// 200ms for every 100 threads
		bThreadsLeft = FALSE;
		
		if (WaitForSingleObject(m_pThread->m_hEventDead, 0) == WAIT_TIMEOUT)
			bThreadsLeft = TRUE;
	}

	VERIFY(WaitForSingleObject(m_pThread->m_hThread, INFINITE) == WAIT_OBJECT_0);
	delete m_pThread;
}

void CRanOnlienInstallerDlg::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( !m_bForceTerminate )
	{
		OnKillThread(); // 종료버튼이 눌러지지 않았다면 쓰레드 제거
	}

	SAFE_DELETE ( m_pHttpPatch );

	CDialog::PostNcDestroy();
}

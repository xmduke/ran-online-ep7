// ServerManagerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "ServerViewDlg.h"
#include "ServerManager.h"
#include "ServerManagerDlg.h"
#include "Winuser.h"
#include "ChatMsgDlg.h"
#include "s_CClientConsoleMsg.h"
#include "s_CJobSchedule.h"
#include "ServerStateDlg.h"
#include ".\servermanagerdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnContextmenuStop();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CServerManagerDlg 대화 상자
CServerManagerDlg::CServerManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerManagerDlg::IDD, pParent)
    , m_pJobDlg      (NULL)
    , m_pNetClient   (NULL)
    , m_nMaxUser     (0)
    , m_pLogFile     (NULL)
    , m_strNoticeMsg (_T(""))
	, m_bFirstRcvTracing(TRUE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    if (CoInitialize(NULL) != S_OK)
    {
        MessageBox("CoInitialize() Failed");
        return;
    }
	m_vecServerList.clear();
}

CServerManagerDlg::~CServerManagerDlg()
{  
    SAFE_DELETE(m_pLogFile);
    SAFE_DELETE(m_pNetClient);
	COdbcManager::GetInstance()->CloseSerManDB();
}

void CServerManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SERVER, m_ListServer);
	DDX_Control(pDX, IDC_LIST_TRACING, m_TracingList );
	DDX_Control(pDX, IDC_IPADDRESS, m_IPAddress);
	DDX_Control(pDX, IDC_EDIT_PORT, m_Port);
	DDX_Control(pDX, IDC_TRACING_EDIT, m_EditTracingID );
	DDX_Control(pDX, IDC_LED1, m_Led1);
	DDX_Control(pDX, IDC_EDIT_REFRESH_RATE, m_EditRefreshRate);
	DDX_Control(pDX, IDC_STATIC_MAX, m_MaxUser);
	DDX_Control(pDX, IDC_STATIC_CRT, m_CrtUser);
	DDX_Control(pDX, IDC_STATIC_HISTO, m_Histogram);
	//	DDV_MaxChars(pDX, m_strNoticeMsg, 100);
	DDX_Control(pDX, IDC_EDIT_MSG1, m_EditMsg[0]);
	DDX_Control(pDX, IDC_EDIT_MSG2, m_EditMsg[1]);
	DDX_Control(pDX, IDC_EDIT_MSG3, m_EditMsg[2]);
	DDX_Control(pDX, IDC_EDIT_MSG4, m_EditMsg[3]);
	DDX_Control(pDX, IDC_EDIT_MSG5, m_EditMsg[4]);
	DDX_Control(pDX, IDC_EDIT_MSG6, m_EditMsg[5]);
	DDX_Control(pDX, IDC_EDIT_MSG7, m_EditMsg[6]);
	DDX_Control(pDX, IDC_EDIT_MSG8, m_EditMsg[7]);
	DDX_Control(pDX, IDC_EDIT_MSG9, m_EditMsg[8]);
	DDX_Control(pDX, IDC_EDIT_MSG10, m_EditMsg[9]);
	DDX_Control(pDX, IDC_EDIT_MSG11, m_EditMsg[10]);
	DDX_Control(pDX, IDC_EDIT_MSG12, m_EditMsg[11]);
	DDX_Control(pDX, IDC_EDIT_MSG13, m_EditMsg[12]);
	DDX_Control(pDX, IDC_EDIT_MSG14, m_EditMsg[13]);
	DDX_Control(pDX, IDC_EDIT_MSG15, m_EditMsg[14]);
	DDX_Control(pDX, IDC_EDIT_MSG16, m_EditMsg[15]);
	DDX_Control(pDX, IDC_EDIT_MSG17, m_EditMsg[16]);
	DDX_Control(pDX, IDC_EDIT_MSG18, m_EditMsg[17]);
	DDX_Control(pDX, IDC_EDIT_MSG19, m_EditMsg[18]);
	DDX_Control(pDX, IDC_EDIT_MSG20, m_EditMsg[19]);

	DDX_Control(pDX, IDC_EDIT_CONSOLE, m_EditConsole);
}

BEGIN_MESSAGE_MAP(CServerManagerDlg, CDialog)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_CONNECT, OnBnClickedBtnConnect)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SERVER, OnNMDblclkListServer)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SERVER, OnNMRclickListServer)
	ON_COMMAND(ID_CONTEXTMENU_OPEN, OnContextmenuOpen)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_COMMAND(ID_CONTEXTMENU_MESSAGE, OnContextmenuMessage)
	ON_BN_CLICKED(IDC_BTN_JOB_SCHEDULE, OnBnClickedBtnJobSchedule)
	ON_COMMAND(ID_CONTEXTMENU_STOP, OnContextmenuStop)
	ON_COMMAND(ID_CONTEXTMENU_STOP_5, OnContextmenuStop5)
	ON_EN_MAXTEXT(IDC_EDIT_CONSOLE, OnEnMaxtextEditConsole)
	ON_WM_SIZE()
	ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
    ON_BN_CLICKED(IDC_BTN_MSG_SND, OnBnClickedBtnMsgSnd)
//ON_WM_SYSCOMMAND()
ON_COMMAND(ID_134, OnClickMenuExit)
ON_BN_CLICKED(IDC_SEND_TYPE1, OnBnClickedSendType1)
ON_BN_CLICKED(IDC_SEND_TYPE2, OnBnClickedSendType2)
ON_COMMAND(ID_JOB_ADD, OnJobAdd)
ON_COMMAND(ID_JOB_EDIT, OnJobEdit)
ON_COMMAND(ID_JOB_DELETE, OnJobDelete)
ON_BN_CLICKED(IDC_VIEW10_BUTTON, OnBnClickedView10Button)
ON_BN_CLICKED(IDC_VIEW20_BUTTON, OnBnClickedView20Button)
ON_BN_CLICKED(IDC_VIEW1_BUTTON, OnBnClickedView1Button)
ON_BN_CLICKED(IDC_SVRSTATE_BUTTON, OnBnClickedSvrstateButton)
ON_BN_CLICKED(IDC_TRACING_BUTTON, OnBnClickedTracingButton)
ON_WM_MOVING()
ON_BN_CLICKED(IDC_ADD_BUTTON, OnBnClickedAddButton)
ON_BN_CLICKED(IDC_DEL_BUTTON, OnBnClickedDelButton)
ON_BN_CLICKED(IDC_ADD_TYPE_CHECK, OnBnClickedAddTypeCheck)
ON_BN_CLICKED(IDC_BTN_MSG_CLEAR, OnBnClickedBtnMsgClear)
END_MESSAGE_MAP()


// CServerManagerDlg 메시지 처리기

BOOL CServerManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.
	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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

    // csv 파일 생성
    SAFE_DELETE(m_pLogFile);
	m_pLogFile = new CLogFile(_T("ServerState"));
	if (m_pLogFile)
		m_pLogFile->Write(_T("Time,ServerType,Group,User,Total,Max"));	

    // Create console 
	CClientConsoleMessage::GetInstance()->SetControl(GetDlgItem(IDC_EDIT_CONSOLE)->m_hWnd);	

	// Create cfg module
	CServerManagerCfg::GetInstance()->Load();

#if defined(KRT_PARAM) || defined(KR_PARAM)
	int nRetCode;
	nRetCode = COdbcManager::GetInstance()->OpenSerManDB( CServerManagerCfg::GetInstance()->m_strOdbcName,														
														CServerManagerCfg::GetInstance()->m_strOdbcUserName,
														CServerManagerCfg::GetInstance()->m_strOdbcPassword,
														CServerManagerCfg::GetInstance()->m_strOdbcDBName  );

    if (nRetCode == DB_ERROR)
	{
		MessageBox("DB Open Failed. Check! ODBC name", "ERROR", MB_ICONEXCLAMATION);
        PostMessage(WM_CLOSE, 0, 0);
		// Todo : program quit
	}
#endif

	// Create network module
    // Load Winsock dll
	NET_InitializeSocket();
    SAFE_DELETE(m_pNetClient);
	m_pNetClient = new CNetCtrl(m_hWnd);
	m_pNetClient->SetCtrlHandle(this);

	// Create LED Control
	m_Led1.SetLed( CLed::LED_COLOR_RED, CLed::LED_ON, CLed::LED_SQUARE );
	
	// Max, Current User 
	m_MaxUser.SetNumberOfLines(1);	
	m_MaxUser.SetXCharsPerLine(7);
	m_MaxUser.SetArrange(CMatrixStatic::RIGHT);
	m_MaxUser.SetSize(CMatrixStatic::TINY);
	m_MaxUser.SetDisplayColors(RGB(0, 0, 0), RGB(255, 181, 63), RGB(103, 64, 23));
	m_MaxUser.AdjustClientXToSize(7);
	m_MaxUser.AdjustClientYToSize(1);
	m_MaxUser.SetText(_T("0"));

	m_CrtUser.SetNumberOfLines(1);
	m_CrtUser.SetXCharsPerLine(7);
	m_CrtUser.SetArrange(CMatrixStatic::RIGHT);
	m_CrtUser.SetSize(CMatrixStatic::TINY);
	m_CrtUser.SetDisplayColors(RGB(0, 0, 0), RGB(255, 181, 63), RGB(103, 64, 23));	
	m_CrtUser.AdjustClientXToSize(7);
	m_CrtUser.AdjustClientYToSize(1);
	m_CrtUser.SetText(_T("0"));	

	// Histogram	
	m_Histogram.SetTextLeft();
	m_Histogram.SetMotionLtoR(FALSE);
	m_Histogram.Add(0);

	// Sizing Control
	CRect clientRect;
	GetClientRect(&clientRect);
	SendMessage(WM_SIZE, SIZE_RESTORED,	MAKELONG(clientRect.Width(), clientRect.Height()));

	// Setting Default value
	m_IPAddress.SetWindowText(CServerManagerCfg::GetInstance()->GetSessionIP());

//	CString strTemp;
//	strTemp.Format(_T("%d"), CServerManagerCfg::GetInstance()->GetSessionPort());
	m_Port.SetWindowText("5003");

	m_EditRefreshRate.SetWindowText(_T("60"));
	
	InitListHead(); // Init listcontrol head
	CheckRadioButton( IDC_SEND_TYPE1, IDC_SEND_TYPE2, IDC_SEND_TYPE1 );
//	GetDlgItem( IDC_BTN_SETTING )->EnableWindow(FALSE);
	BYTE i;
	for( i = 0; i < 8; i++ )
	{
		GetDlgItem( IDC_SVR_CHECK1+i )->EnableWindow(FALSE);
	}

	RECT rect;
	for( i = 0; i < 20; i++ )
	{
		m_EditMsg[i].SetLimitText(48);
		if( i == 0  )
		{
			m_EditMsg[0].GetWindowRect( &rect );
			rect.top -= 48; rect.bottom -= 48;
		}else{
			m_EditMsg[i].ShowWindow(SW_HIDE);
		}
        m_EditMsg[i].MoveWindow( &rect, TRUE );
		rect.top += 23; rect.bottom += 23;
	}

	OnBnClickedView1Button();

	m_SendMsgNum = 1;
	m_nPhoneSendCount = 0;
	
	// Start Job Schedule Timer
	SetTimer(100, 60000, 0); // 1 분

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
	GetDlgItem( IDC_TRACING_BUTTON )->ShowWindow(TRUE);
#else
	GetDlgItem( IDC_TRACING_BUTTON )->ShowWindow(FALSE);
#endif

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}



void CServerManagerDlg::InitListHead()
{
	// 상태
	// 종류
	// 이름
	// 주소
	// 포트
	// 접속자
	// Progress	
	m_ListServer.SetExtendedStyle ( LVS_EX_FULLROWSELECT ); 

	LVCOLUMN Col;

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_LEFT;	
	Col.pszText		= _T("State");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListServer.InsertColumn(0, &Col);
	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 70;
	Col.fmt			= LVCFMT_LEFT;
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	Col.pszText		= _T("Kind");
	m_ListServer.InsertColumn(1, &Col);
	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;	
	Col.cx			= 100;
	Col.fmt			= LVCFMT_LEFT;	
	Col.pszText		= _T("Grp Num");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListServer.InsertColumn(2, &Col);
	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 110;
	Col.fmt			= LVCFMT_LEFT;	
	Col.pszText		= _T("Address");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListServer.InsertColumn(3, &Col);
		
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_LEFT;	
	Col.pszText		= _T("Port");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListServer.InsertColumn(4, &Col);
		
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 100;
	Col.fmt			= LVCFMT_LEFT;	
	Col.pszText		= _T("Users/Max");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListServer.InsertColumn(5, &Col);
		
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 100;
	Col.fmt			= LVCFMT_LEFT;	
	Col.pszText		= _T("Percent");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListServer.InsertColumn(6, &Col);	

	m_ListServer.SetExtendedStyle ( LVS_EX_FULLROWSELECT ); 

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 115;
	Col.fmt			= LVCFMT_LEFT;	
	Col.pszText		= _T("Account");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_TracingList.InsertColumn(0, &Col);

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 70;
	Col.fmt			= LVCFMT_LEFT;	
	Col.pszText		= _T("UserNum");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_TracingList.InsertColumn(1, &Col);

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 65;
	Col.fmt			= LVCFMT_LEFT;
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	Col.pszText		= _T("ON/OFF");
	m_TracingList.InsertColumn(2, &Col);

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;	
	Col.cx			= 85;
	Col.fmt			= LVCFMT_LEFT;	
	Col.pszText		= _T("Con. Server");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_TracingList.InsertColumn(3, &Col);
}

CString	CServerManagerDlg::GetAppPath()
{
	CString strFullPath;
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if (!strCommandLine.IsEmpty())
	{
		DWORD dwFind = strCommandLine.ReverseFind('\\');
		if (dwFind != -1)
		{
			strFullPath = strCommandLine.Left(dwFind);
			
			if (!strFullPath.IsEmpty())
			if (strFullPath.GetAt(0) == '"')
				strFullPath = strFullPath.Right(strFullPath.GetLength() - 1);
		}
	}
	return strFullPath;
}

void CServerManagerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.	
}

void CServerManagerDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CDialog::OnClose();
}

void CServerManagerDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 서버리스트 삭제
	G_SERVER_INFO_DLG* gsid;
	POSITION pos = m_DlgList.GetHeadPosition();

	for (int i=0; i<m_DlgList.GetCount(); i++)
	{
		gsid = m_DlgList.GetAt(pos);
		if (gsid->pDlg != NULL)
        {
			SAFE_DELETE(gsid->pDlg);
        }
        SAFE_DELETE(gsid);
		m_DlgList.GetNext(pos);
		Sleep( 1 );
	}
    m_DlgList.RemoveAll();

	for( i=0; i<m_vecServerList.size(); i++ )
	{
		SAFE_DELETE(m_vecServerList[i]);
	}
	m_vecServerList.clear();

	SAFE_DELETE(m_pJobDlg);
	
	KillTimer(100);
	CLIENT_JOB::CScheduler::GetInstance()->ReleaseInstance();
	CServerManagerCfg::GetInstance()->ReleaseInstance();
	CClientConsoleMessage::GetInstance()->ReleaseInstance();
	COdbcManager::GetInstance()->ReleaseInstance();

    if ((m_pNetClient != NULL) && (m_pNetClient->IsOnline()))
    {
        m_pNetClient->CloseConnect();
    }
	SAFE_DELETE(m_pNetClient);	
	NET_CloseSocket();
}

void CServerManagerDlg::OnOK()
{	
	/*CWnd* pTemp = (CWnd*) GetDlgItem(IDC_EDIT_MSG1);
	if (pTemp == this->GetFocus())*/
	{
		// 서버 메시지 보내기에 포커스가 있다면 메시지 전송 함수 호출
		OnBnClickedBtnMsgSnd();
	}
	// CDialog::OnOK(); // Enter 키 프로그램 종료 방지
}

void CServerManagerDlg::OnCancel()
{	
	// ESC 키 프로그램 종료 방지
	if (MessageBox(_T("Really Exit Program?"), _T("Warning"), MB_OKCANCEL|MB_ICONQUESTION) != IDOK)
    {
        return;
    }
    CDialog::OnCancel();
}

// 메뉴에서 종료클릭
void CServerManagerDlg::OnClickMenuExit()
{
	OnCancel();
}

// 실수할 가능성이 있어서 삭제된 코드
// 서버 스탑...
void CServerManagerDlg::OnContextmenuStop()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	/*
	int nSelected = m_ListServer.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	G_SERVER_INFO_DLG* pTemp = reinterpret_cast <G_SERVER_INFO_DLG*> (m_ListServer.GetItemData(nSelected));

	if (pTemp != NULL)
	{
		if (MessageBox("This operation will stop the server, really execute?", "Caution", MB_OKCANCEL) == IDOK)
		{
			// NET_MSG_SVR_PAUSE	서버를 잠시 멈춘다.
			// NET_MSG_SVR_RESUME	멈추어진 서버를 다시 가동시킨다.	
			// NET_MSG_SVR_RESTART	서버를 완전히 멈추고 새롭게 가동시킨다.
			// NET_MSG_SVR_STOP		서버를 완전히 멈춘다.
			m_pNetClient->SndSvrCmd(pTemp->gsi.nServerGroup,
									pTemp->gsi.nServerNumber,
									pTemp->gsi.nServerField,
									pTemp->gsi.nServerType,
									NET_MSG_SVR_STOP);			
		}
	}
	*/
}

// 현시간으로 부터 5 분후 서버를 완전히 멈춘다.
void CServerManagerDlg::OnContextmenuStop5()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	/*
	int nHour=0;
	int nMin=0;
	int nSelected = m_ListServer.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	G_SERVER_INFO_DLG* pTemp = reinterpret_cast <G_SERVER_INFO_DLG*> (m_ListServer.GetItemData(nSelected));

	if (pTemp != NULL)
	{
		if (MessageBox("This operation will stop the server, really execute?", "Caution", MB_OKCANCEL) == IDOK)
		{
			CClientConsoleMessage::GetInstance()->WriteWithTime("= 작업등록을 시작합니다 =");
			// 작업을 등록한다.
			
			// 현재 시간을 가져온다.
			time_t curr;
			tm local;
			time(&curr); // get current time_t value
			local=*(localtime(&curr));

			nHour = local.tm_hour;
			nMin = local.tm_min;

			for (int nCount=1; nCount<6; ++nCount)
			{
				nMin = nMin + 1;
				if (nMin > 59)
				{
					nMin = 0;
					nHour = nHour + 1;
					if (nHour > 23)
					{
						nHour = 0;
					}
				}

				CString strMsg;
				strMsg.Format(_T("%d 분 후에 점검을 위해서 서버를 종료하겠습니다"), 5-nCount);
				pTemp->gsi.szServerIP;
                
				CLIENT_JOB::CBroadCast* pBroadCast; 				

				pBroadCast = new CLIENT_JOB::CBroadCast(strMsg, 
													pTemp->gsi.szServerIP, 
													pTemp->gsi.nServicePort, 
													nHour, 
													nMin);
				CLIENT_JOB::CScheduler::GetInstance()->AddJob(pBroadCast);
			}

			CLIENT_JOB::CServerStop* pServerStop;
			pServerStop = new CLIENT_JOB::CServerStop(nHour, 
													nMin, 
													CLIENT_JOB::JOB_ONCE,
													pTemp->gsi.nServerGroup,
													pTemp->gsi.nServerNumber,
													pTemp->gsi.nServerField,
													pTemp->gsi.nServerType,
													NET_MSG_SVR_STOP);
			CLIENT_JOB::CScheduler::GetInstance()->AddJob(pServerStop);
				
			// 작업관리창 리플래쉬
			if (m_pJobDlg != NULL)	m_pJobDlg->Refresh();
		}
	}
	*/
}

void CServerManagerDlg::SetLedRed()
{
	m_Led1.SetLed( CLed::LED_COLOR_RED, CLed::LED_ON, CLed::LED_SQUARE );
}

void CServerManagerDlg::SetLedGreen()
{
	m_Led1.SetLed( CLed::LED_COLOR_RED, CLed::LED_ON, CLed::LED_SQUARE );
}
void CServerManagerDlg::OnBnClickedSendType1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BYTE i;
	for( i = 0; i < 8; i++ )
	{
		GetDlgItem( IDC_SVR_CHECK1+i )->EnableWindow(FALSE);
	}
}

void CServerManagerDlg::OnBnClickedSendType2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	G_SERVER_INFO_DLG* gsid;
	size_t servSize = m_vecServerList.size();
	if (servSize)
	{
		for (size_t i=0; i< servSize; i++)
		{
			gsid = m_vecServerList[i];
			if (gsid->gsi.nServerType == NET_SERVER_AGENT)
			{
				if( gsid->gsi.nServerGroup-1 >= 0 && gsid->gsi.nServerGroup-1 < 8 )
					GetDlgItem( IDC_SVR_CHECK1+gsid->gsi.nServerGroup-1 )->EnableWindow(TRUE);
			}
			
		}
	}
}

//void CServerManagerDlg::OnBnClickedBtnSetting()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	//
//	std::vector<int> eachGroupList;	
//	G_SERVER_INFO_DLG* gsid;
//	size_t servSize = m_vecServerList.size();
//	if (servSize)
//	{
//		for (int i=0; i< servSize; i++)
//		{
//			gsid = m_vecServerList[i];
//			if (gsid->gsi.nServerType == NET_SERVER_AGENT)
//			{
//				eachGroupList.push_back( gsid->gsi.nServerGroup );
//			}
//			
//		}
//	}
//
////	CEachGroupDlg		EachDlg;
////	EachDlg.SetEachGroupList( eachGroupList );
//
//	m_pEachDlg->SetEachGroupList( eachGroupList );
//	int iNum = 0;
//
//	CButton *pButton = NULL;
//	m_vecSendServList.clear();
//	if( m_pEachDlg->DoModal() == IDOK )
//	{		
//		m_vecSendServList = m_pEachDlg->GetSelectEachGroupList();
//	}
//}





void CServerManagerDlg::OnBnClickedSvrstateButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CServerStateDlg *pServerDlg;
	pServerDlg = new CServerStateDlg(this);
	pServerDlg->SetServerState( m_vecServerList );
	pServerDlg->DoModal();
	SAFE_DELETE(pServerDlg);
	
}


void CServerManagerDlg::OnMoving(UINT fwSide, LPRECT pRect)
{
	CDialog::OnMoving(fwSide, pRect);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}





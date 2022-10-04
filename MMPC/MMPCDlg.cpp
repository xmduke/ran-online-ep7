// MMPCDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "MMPC.h"
#include "MMPCDlg.h"
#include ".\mmpcdlg.h"

#include "BitmapMan.h"

inline ROUND( float fValue)
{
	return static_cast<int>( fValue + ( fValue > 0 ? 0.5 : -0.5 ) );
}

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


// CMMPCDlg 대화 상자



CMMPCDlg::CMMPCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMMPCDlg::IDD, pParent)
	, m_nLX(0)
	, m_nTY(0)
	, m_nRX(0)
	, m_nBY(0)
	, m_nSizeX(0)
	, m_nSizeY(0)
	, m_nCalcLX(0)
	, m_nCalcBY(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMMPCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILENAME, m_csFileName);
	DDX_Text( pDX, IDC_LX, m_nLX );
	DDX_Text( pDX, IDC_TY, m_nTY );
	DDX_Text( pDX, IDC_RX, m_nRX );
	DDX_Text( pDX, IDC_BY, m_nBY );
	DDX_Control(pDX, IDCANCEL, m_ctrlButtonCancel);
	DDX_Control(pDX, IDC_VIEW, m_ctrlEditView);
	DDX_Control(pDX, IDC_HIDE, m_ctrlButtonHide);
	DDX_Control(pDX, IDC_FILENAME2, m_csDDSFileName);
}

BEGIN_MESSAGE_MAP(CMMPCDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_HIDE, OnBnClickedHide)
//	ON_WM_TIMER()
ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
END_MESSAGE_MAP()


// CMMPCDlg 메시지 처리기

BOOL CMMPCDlg::OnInitDialog()
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
	HideEditView(); // 아래 컨트롤을 숨김
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CMMPCDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMMPCDlg::OnPaint() 
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
HCURSOR CMMPCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMMPCDlg::OnBnClickedButton1()
{
	// TODO: BMP 파일 선택
	CString szFilter = "비트맵 파일(*.bmp)|*.bmp|";

	CFileDialog dlg( TRUE, ".bmp", NULL, OFN_HIDEREADONLY, szFilter );

	if ( dlg.DoModal() == IDOK )
	{
		CString strPathName( dlg.GetPathName() );
		CBitmapMan::m_cBitmapMan.LOAD( strPathName );

		m_csFileName.SetWindowText( dlg.GetPathName() );
	}
}

void CMMPCDlg::OnBnClickedOk()
{
	// TODO: MMP 파일 생성
	CString szFilter = "MMP 파일(*.mmp)|*.mmp|";

	CFileDialog dlg( FALSE, ".mmp", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter );

	if ( dlg.DoModal() == IDOK )
	{
		BeginWaitCursor();
		{
			// 에디터박스에 있는 값을 얻어온다.
			UpdateData();

			// BMP의 좌료를 얻어온다.
			RECT rect = GetBitmapPosition();

			// 좌표 계산
			MiniMapPositionCalc( rect );
		}
		EndWaitCursor();

		CString strBUFFER;

		// 파일 저장
		if( !MiniMapPositionSave( dlg.GetPathName(), strBUFFER ) )
		{
			// 저장 실패
			AfxMessageBox( "MMP 파일 생성에 실패하였습니다." );
		}
		else
		{
			ShowEditView( strBUFFER );
		}
	}

	//OnOK();
}

void CMMPCDlg::ShowEditView( CString & strBUFFER )
{
	RECT rtCancel, rtMain, rtHide;

	SetDlgItemText( IDC_VIEW, strBUFFER );

	if( m_ctrlButtonHide.IsWindowVisible() == TRUE ) return; // 이미 에디터뷰가 보이면 크기조절 하지않는다.

	m_ctrlButtonCancel.GetWindowRect( &rtCancel );
	m_ctrlButtonHide.GetWindowRect( &rtHide );
	GetWindowRect( &rtMain );

	int nGap = rtHide.bottom - rtCancel.bottom;

	MoveWindow( rtMain.left, rtMain.top,
				rtMain.right - rtMain.left, rtMain.bottom - rtMain.top + nGap, TRUE );

	m_ctrlButtonHide.ShowWindow( SW_SHOW );
}

void CMMPCDlg::HideEditView()
{
	RECT rtCancel, rtMain, rtHide;

	m_ctrlButtonCancel.GetWindowRect( &rtCancel );
	m_ctrlButtonHide.GetWindowRect( &rtHide );
	GetWindowRect( &rtMain );

	int nGap = rtHide.bottom - rtCancel.bottom;

	MoveWindow( rtMain.left, rtMain.top,
				rtMain.right - rtMain.left, rtMain.bottom - rtMain.top - nGap, TRUE );

	m_ctrlButtonHide.ShowWindow( SW_HIDE );
}

void CMMPCDlg::InitEditView()
{
	RECT rect, destRect;

	GetWindowRect( &destRect );
	m_ctrlButtonCancel.GetWindowRect( &rect );
	ScreenToClient( &rect );

	destRect.bottom = rect.bottom + 18 + GetSystemMetrics(SM_CYCAPTION);
	MoveWindow( &destRect, FALSE );
}

BOOL CMMPCDlg::MiniMapPositionSave( CString & strPathName, CString & strBUFFER )
{
	CFile file;

	if( !file.Open( strPathName, CFile::modeCreate|CFile::modeWrite) )
		return FALSE;

	BITMAP sBitmap = CBitmapMan::m_cBitmapMan.GetBitmap();

	strBUFFER.Format( "MINIMAPNAME\t\t%s\r\n", m_strDDSFileName );
	strBUFFER.AppendFormat( "MAPSIZE_X\t\t%d %d\r\n", m_nSizeX, m_nCalcLX );
	strBUFFER.AppendFormat( "MAPSIZE_Y\t\t%d %d\r\n", m_nSizeY, m_nCalcBY );
	strBUFFER.AppendFormat( "TEXTURE_SIZE\t\t%d %d\r\n", sBitmap.bmWidth, sBitmap.bmHeight );
	strBUFFER.AppendFormat( "TEXTURE_POS\t\t%d %d %d %d", 0, 0, sBitmap.bmWidth, sBitmap.bmHeight ); 

	file.Write( strBUFFER, strBUFFER.GetLength() ); // 파일 쓰기

	file.Close();

	return TRUE;
}

void CMMPCDlg::MiniMapPositionCalc( RECT & rect )
{
	float fRealSizeX = static_cast<float>(m_nRX - m_nLX);
	float fRealSizeY = static_cast<float>(m_nTY - m_nBY);

	BITMAP sBitmap = CBitmapMan::m_cBitmapMan.GetBitmap();

	m_nSizeX = ROUND( ( fRealSizeX * sBitmap.bmWidth ) / ( rect.right - rect.left ) );
	m_nCalcLX = m_nLX - ROUND( (float)m_nSizeX * rect.left / sBitmap.bmWidth );

	m_nSizeY = ROUND( ( fRealSizeY * sBitmap.bmHeight ) / ( rect.bottom - rect.top ) );
	m_nCalcBY = m_nBY - ROUND( (float)m_nSizeY * rect.top / sBitmap.bmHeight );
}

RECT CMMPCDlg::GetBitmapPosition()
{
	CClientDC WinDC(this);

	CDC MemDC;
	MemDC.CreateCompatibleDC( &WinDC );

	BITMAP sBitmap = CBitmapMan::m_cBitmapMan.GetBitmap();
	HBITMAP hBitmap = CBitmapMan::m_cBitmapMan.GetHBitmap();

	MemDC.SelectObject( (HBITMAP)hBitmap ); // 현재 오브젝트를 저장했다가 나중에 선택하는게 에러남.

	///////////////////////////////
	INT nLEFT(0);
	INT nTOP(0);
	INT nRIGHT(sBitmap.bmWidth-1);
	INT nBOTTOM(sBitmap.bmHeight-1);

	BOOL bOut(FALSE);
	///////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////
	COLORREF colorRef = GetPixel(MemDC, nLEFT, nTOP);
	for(int i=nTOP; i<=nBOTTOM && !bOut; ++i) // 위쪽 라인을 검사한다.
	{
		for(int j=nLEFT; j<=nRIGHT; ++j)
		{
			COLORREF color = GetPixel(MemDC, j, i);
			if(colorRef != color )
			{
				nTOP = i;
				bOut = TRUE;
				break;
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////
	bOut = FALSE;
	colorRef = GetPixel(MemDC, nLEFT, nBOTTOM);
	for(int i=nBOTTOM; i>=nTOP && !bOut; --i) // 아래쪽 라인을 검사한다.
	{
		for(int j=nLEFT; j<=nRIGHT; ++j)
		{
			COLORREF color = GetPixel(MemDC, j, i);
			if(colorRef != color)
			{
				nBOTTOM = i;
				bOut = TRUE;
				break;
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////
	bOut = FALSE;
	colorRef = GetPixel(MemDC, nLEFT, nTOP);
	for(int i=nLEFT; i<=nRIGHT && !bOut; ++i) // 왼쪽 라인을 검사한다.
	{
		for(int j=nTOP; j<=nBOTTOM; ++j)
		{
			COLORREF color = GetPixel(MemDC, i, j);
			if(colorRef != color)
			{
				nLEFT = i;
				bOut = TRUE;
				break;
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////
	bOut = FALSE;
	colorRef = GetPixel(MemDC, nRIGHT, nTOP);
	for(int i=nRIGHT; i>=nLEFT && !bOut; --i) // 오른쪽 라인을 검사한다.
	{
		for(int j=nTOP; j<=nBOTTOM; ++j)
		{
			COLORREF color = GetPixel(MemDC, i, j);
			if(colorRef != color)
			{
				nRIGHT = i;
				bOut = TRUE;
				break;
			}
		}
	}

	RECT rect;

	rect.left = nLEFT;
	rect.top = nTOP;
	rect.right = nRIGHT;
	rect.bottom = nBOTTOM;

	return rect;
}

void CMMPCDlg::OnBnClickedHide()
{
	HideEditView();
}

void CMMPCDlg::OnBnClickedButton2()
{
	// TODO: DDS 파일 선택
	CString szFilter = "DDS 파일(*.dds)|*.dds|";

	CFileDialog dlg( TRUE, ".dds", NULL, OFN_HIDEREADONLY, szFilter );

	if ( dlg.DoModal() == IDOK )
	{
		CString strPathName( dlg.GetPathName() );
		m_csDDSFileName.SetWindowText( dlg.GetPathName() );

		m_strDDSFileName = dlg.GetFileName();
	}
}

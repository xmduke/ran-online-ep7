// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "VersionManager.h"

#include "MainFrm.h"
#include "CCfg.h"
#include "CConsoleMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.	
	m_pLeft = NULL;
	m_pRight = NULL;
	m_bInitSplitter = FALSE;
}

CMainFrame::~CMainFrame()
{
	COdbcManager::GetInstance()->CloseVerDB();
	COdbcManager::GetInstance()->ReleaseInstance();
	CCfg::GetInstance()->ReleaseInstance();
    CConsoleMessage::GetInstance()->ReleaseInstance();
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}	

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
    /*
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);	
	DockControlBar(&m_wndToolBar);
    */

	CCfg::GetInstance()->Load("VersionManager.cfg");
	
	int nRetCode;
	nRetCode = COdbcManager::GetInstance()->OpenVerDB(CCfg::GetInstance()->m_szOdbcName,
								CCfg::GetInstance()->m_szOdbcUserName,
								CCfg::GetInstance()->m_szOdbcPassword,
								CCfg::GetInstance()->m_szOdbcDbName);
	if (nRetCode == DB_ERROR)
	{
		MessageBox("DB Open Failed. Check! ODBC name", "ERROR", MB_ICONEXCLAMATION);
        PostMessage(WM_CLOSE, 0, 0);
		// Todo : program quit
	}

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	CRect rc;
	GetClientRect(&rc);

	m_wndSplitter.CreateStatic(this, 1, 2);
	int nLeftSize = rc.Width() / 3;
	int nRightSize = rc.Width() - nLeftSize;
	m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CFormLeft),  CSize( nLeftSize,  0), pContext);
	m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CFormRight), CSize( nRightSize, 0), pContext);
	
	m_pLeft	 = (CFormLeft *)  m_wndSplitter.GetPane(0,0);
    m_pRight = (CFormRight *) m_wndSplitter.GetPane(0,1);

	SetActiveView((CView*)m_wndSplitter.GetPane(0, 1));
	m_bInitSplitter = TRUE;

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.
    WNDCLASS* a;
    a = (WNDCLASS*) cs.lpszClass;
	return TRUE;
}


// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

// CMainFrame 메시지 처리기
void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);
	CRect cr;
	GetWindowRect(&cr);

	if (  m_bInitSplitter && nType != SIZE_MINIMIZED )
	{
		/*m_mainSplitter.SetRowInfo( 0, cy, 0 );
		m_mainSplitter.SetColumnInfo( 0, cr.Width() / 2, 50);
		m_mainSplitter.SetColumnInfo( 1, cr.Width() / 2, 50);
		
		m_mainSplitter.RecalcLayout();*/
	}
}


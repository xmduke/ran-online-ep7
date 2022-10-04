// ServerViewDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServerManager.h"
#include "ServerViewDlg.h"
#include "ServerManagerDlg.h"

// CServerViewDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CServerViewDlg, CDialog)
CServerViewDlg::CServerViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerViewDlg::IDD, pParent)
{
	m_pDlg = (CDialog*) pParent;
	Create();
	m_PinButton.SetWnd(this);	
}

CServerViewDlg::~CServerViewDlg()
{
}

BOOL CServerViewDlg::Create()
{
	return CDialog::Create(CServerViewDlg::IDD);
}

BOOL CServerViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_PinButton.SetPinned(TRUE);

	// Top most window
	SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	
	m_UserHistogram.SetTextLeft();
	m_UserHistogram.SetMotionLtoR(FALSE);

	m_UserHistogram.Add(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CServerViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_PIN, m_PinButton);
	DDX_Control(pDX, IDC_STATIC_USER, m_UserHistogram);
}


BEGIN_MESSAGE_MAP(CServerViewDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_NCDESTROY()
	ON_WM_SYSCOLORCHANGE()
END_MESSAGE_MAP()

// CServerViewDlg 메시지 처리기입니다.

void CServerViewDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pDlg != NULL) {
		// modeless case -- do not call base class OnOK
		UpdateData(TRUE);
		m_pDlg->PostMessage(WM_GOODBYE, IDOK);
		DestroyWindow();
	}
	else {
		DestroyWindow();
	}
}

void CServerViewDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pDlg != NULL) {
		// modeless case -- do not call base class OnCancel
		m_pDlg->PostMessage(WM_GOODBYE, IDCANCEL);
		DestroyWindow();
	}
	else {		
		DestroyWindow();
	}
}

// 다이얼로그종료, 메모리 정리
void CServerViewDlg::OnNcDestroy()
{
	CDialog::OnNcDestroy();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CServerManagerDlg *pDlg;
	pDlg = (CServerManagerDlg*) m_pDlg;
	pDlg->RemoveDlgList(this);
	delete this;
}

void CServerViewDlg::OnSysColorChange()
{
	CDialog::OnSysColorChange();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	//Inform the push pin of the system color changes
	m_PinButton.ReloadBitmaps();
}

void CServerViewDlg::TopWindow()
{
	if (m_PinButton.IsPinned())
		SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	else
		SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

void CServerViewDlg::UpdateStatus(G_SERVER_INFO gsi)
{
	int nPercent = (int) (gsi.nServerCurrentClient * 100/gsi.nServerMaxClient);
	if (nPercent < 5) 
		nPercent = 5;
	m_UserHistogram.Add(nPercent);
}

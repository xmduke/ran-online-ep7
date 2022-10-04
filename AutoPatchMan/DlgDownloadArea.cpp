// DlgDownloadArea.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AutoPatchMan.h"
#include "DlgDownloadArea.h"
#include ".\dlgdownloadarea.h"
#include "LauncherText.h"


// CDlgDownloadArea 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgDownloadArea, CDialog)
CDlgDownloadArea::CDlgDownloadArea(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDownloadArea::IDD, pParent)
	, m_nDownloadArea(0)
{
}

CDlgDownloadArea::~CDlgDownloadArea()
{
}

void CDlgDownloadArea::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_AREA1, m_nDownloadArea);
}


BEGIN_MESSAGE_MAP(CDlgDownloadArea, CDialog)
END_MESSAGE_MAP()


// CDlgDownloadArea 메시지 처리기입니다.

BOOL CDlgDownloadArea::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetWindowText( ID2LAUNCHERTEXT("IDD_DIALOG_DOWNLOAD_AREA"));
	SetDlgItemText( IDC_AREA_STATIC, ID2LAUNCHERTEXT("IDC_AREA_STATIC") );
	SetDlgItemText( IDOK, ID2LAUNCHERTEXT("DOWNAREA_ID_OK") );
	
	SetDlgItemText( IDC_RADIO_AREA1, m_strAreaName[0] );
	SetDlgItemText( IDC_RADIO_AREA2, m_strAreaName[1] );
	SetDlgItemText( IDC_RADIO_AREA3, m_strAreaName[2] );
//	SetDlgItemText( IDC_RADIO_AREA4, m_strAreaName[3] );
//	SetDlgItemText( IDC_RADIO_AREA5, m_strAreaName[4] );
//	SetDlgItemText( IDC_RADIO_AREA6, m_strAreaName[5] ); // 중국지역 추가

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

#include "stdafx.h"
#include "VersionManager.h"
#include "DlgNewFolder.h"
#include ".\dlgnewfolder.h"

// CDlgNewFolder 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgNewFolder, CDialog)
CDlgNewFolder::CDlgNewFolder(CString strTemp, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewFolder::IDD, pParent)
{
	m_strFolder = strTemp;
}

CDlgNewFolder::~CDlgNewFolder()
{
}

void CDlgNewFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_EditFolder);
}


BEGIN_MESSAGE_MAP(CDlgNewFolder, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgNewFolder 메시지 처리기입니다.

BOOL CDlgNewFolder::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_EditFolder.SetWindowText(m_strFolder);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgNewFolder::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_EditFolder.GetWindowText(m_strFolder);
	OnOK();
}

// EachGroupDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include <vector>
#include "ServerManager.h"
#include "EachGroupDlg.h"
#include ".\eachgroupdlg.h"


// CEachGroupDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEachGroupDlg, CDialog)
CEachGroupDlg::CEachGroupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEachGroupDlg::IDD, pParent)
{
}

CEachGroupDlg::~CEachGroupDlg()
{
}

void CEachGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SVR_GROUP_CHECK1, m_SvrGroupCheck[0]);
	DDX_Control(pDX, IDC_SVR_GROUP_CHECK2, m_SvrGroupCheck[1]);
	DDX_Control(pDX, IDC_SVR_GROUP_CHECK3, m_SvrGroupCheck[2]);
	DDX_Control(pDX, IDC_SVR_GROUP_CHECK4, m_SvrGroupCheck[3]);
	DDX_Control(pDX, IDC_SVR_GROUP_CHECK5, m_SvrGroupCheck[4]);
	DDX_Control(pDX, IDC_SVR_GROUP_CHECK6, m_SvrGroupCheck[5]);
	DDX_Control(pDX, IDC_SVR_GROUP_CHECK7, m_SvrGroupCheck[6]);
	DDX_Control(pDX, IDC_SVR_GROUP_CHECK8, m_SvrGroupCheck[7]);
}


BEGIN_MESSAGE_MAP(CEachGroupDlg, CDialog)
END_MESSAGE_MAP()


// CEachGroupDlg 메시지 처리기입니다.
void CEachGroupDlg::SetEachGroupList( std::vector<int> vecEachGroup )
{ 
	m_vecEachGroup.clear(); 
	m_vecEachGroup = vecEachGroup; 

	GetDlgItem(IDC_SVR_GROUP_CHECK1)->EnableWindow(TRUE);
}

void CEachGroupDlg::GetSelectEachGroupList()
{
	BYTE i;
	m_vecCheckServ.clear();
	for( i = 0; i < 8; i++ )
	{
		CButton *pButton = (CButton *)GetDlgItem(IDC_SVR_GROUP_CHECK1+i);
		if( pButton->GetCheck() )
			m_vecCheckServ.push_back(i);
	}
}

BOOL CEachGroupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	BYTE i, j;
	bool bFind = FALSE;
	for( i = 0; i < 8; i++ )
	{
		bFind = FALSE;
		for( j = 0; j < m_vecEachGroup.size(); j++ )
		{
			// Server Group Num 은 1부터 시작하므로 i에 1을 더한다.
			if( i+1 == m_vecEachGroup[j] ) bFind = TRUE;			
		}
		if( bFind )
		{
			GetDlgItem(IDC_SVR_GROUP_CHECK1+i)->EnableWindow(TRUE);
		}else{
			GetDlgItem(IDC_SVR_GROUP_CHECK1+i)->EnableWindow(FALSE);
		}
	}



	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


// ServerStateDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServerViewDlg.h"
#include "ServerManager.h"
#include "ServerManagerDlg.h"
#include "ServerStateDlg.h"
#include ".\serverstatedlg.h"


// CServerStateDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CServerStateDlg, CDialog)
CServerStateDlg::CServerStateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerStateDlg::IDD, pParent)
{
	m_vecState.clear();
}

CServerStateDlg::~CServerStateDlg()
{
}

void CServerStateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CRT1, m_CurrentNum[0]);
	DDX_Control(pDX, IDC_STATIC_CRT2, m_CurrentNum[1]);
	DDX_Control(pDX, IDC_STATIC_CRT3, m_CurrentNum[2]);
	DDX_Control(pDX, IDC_STATIC_CRT4, m_CurrentNum[3]);
	DDX_Control(pDX, IDC_STATIC_CRT5, m_CurrentNum[4]);
	DDX_Control(pDX, IDC_STATIC_CRT6, m_CurrentNum[5]);
	DDX_Control(pDX, IDC_STATIC_CRT7, m_CurrentNum[6]);
	DDX_Control(pDX, IDC_STATIC_CRT8, m_CurrentNum[7]);

	DDX_Control(pDX, IDC_STATIC_MAX1, m_MaxNum[0]);
	DDX_Control(pDX, IDC_STATIC_MAX2, m_MaxNum[1]);
	DDX_Control(pDX, IDC_STATIC_MAX3, m_MaxNum[2]);
	DDX_Control(pDX, IDC_STATIC_MAX4, m_MaxNum[3]);
	DDX_Control(pDX, IDC_STATIC_MAX5, m_MaxNum[4]);
	DDX_Control(pDX, IDC_STATIC_MAX6, m_MaxNum[5]);
	DDX_Control(pDX, IDC_STATIC_MAX7, m_MaxNum[6]);
	DDX_Control(pDX, IDC_STATIC_MAX8, m_MaxNum[7]);

	DDX_Control(pDX, IDC_STATIC_SVRNUM1, m_SvrNum[0]);
	DDX_Control(pDX, IDC_STATIC_SVRNUM2, m_SvrNum[1]);
	DDX_Control(pDX, IDC_STATIC_SVRNUM3, m_SvrNum[2]);
	DDX_Control(pDX, IDC_STATIC_SVRNUM4, m_SvrNum[3]);
	DDX_Control(pDX, IDC_STATIC_SVRNUM5, m_SvrNum[4]);
	DDX_Control(pDX, IDC_STATIC_SVRNUM6, m_SvrNum[5]);
	DDX_Control(pDX, IDC_STATIC_SVRNUM7, m_SvrNum[6]);
	DDX_Control(pDX, IDC_STATIC_SVRNUM8, m_SvrNum[7]);

}


BEGIN_MESSAGE_MAP(CServerStateDlg, CDialog)
END_MESSAGE_MAP()


// CServerStateDlg 메시지 처리기입니다.
void CServerStateDlg::SetServerState( std::vector<G_SERVER_INFO_DLG*> vecState )
{
	m_vecState = vecState;
	/*int i;
	char szTempChar1[256], szTempChar2[256];
	for( i = 0; i < 8; i++ )
	{
		m_MaxNum[i].ShowWindow(SW_HIDE);
		m_CurrentNum[i].ShowWindow(SW_HIDE);
	}
	for( i = 0; i < vecState.size(); i++ )
	{
		if(vecState[i]->gsi.nServerType == SERVER_AGENT)
		{
			m_MaxNum[i].ShowWindow(SW_SHOW);
			m_CurrentNum[i].ShowWindow(SW_SHOW);

			sprintf( szTempChar1, "%d", vecState[i]->gsi.nServerCurrentClient );
			sprintf( szTempChar2, "%d", vecState[i]->gsi.nServerMaxClient );

			m_CurrentNum[i].SetText(_T(szTempChar1));
			m_MaxNum[i].SetText(_T(szTempChar2));
		}
	}*/
}

BOOL CServerStateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	BYTE i;
	char szTempChar1[256], szTempChar2[256];

	for( i = 0; i < 8; i++ )
	{
		m_MaxNum[i].SetNumberOfLines(1);	
		m_MaxNum[i].SetXCharsPerLine(7);
		m_MaxNum[i].SetArrange(CMatrixStatic::RIGHT);
		m_MaxNum[i].SetSize(CMatrixStatic::TINY);
		m_MaxNum[i].SetDisplayColors(RGB(0, 0, 0), RGB(255, 181, 63), RGB(103, 64, 23));
		m_MaxNum[i].AdjustClientXToSize(7);
		m_MaxNum[i].AdjustClientYToSize(1);
		m_MaxNum[i].SetText(_T("0"));

		m_CurrentNum[i].SetNumberOfLines(1);	
		m_CurrentNum[i].SetXCharsPerLine(7);
		m_CurrentNum[i].SetArrange(CMatrixStatic::RIGHT);
		m_CurrentNum[i].SetSize(CMatrixStatic::TINY);
		m_CurrentNum[i].SetDisplayColors(RGB(0, 0, 0), RGB(255, 181, 63), RGB(103, 64, 23));
		m_CurrentNum[i].AdjustClientXToSize(7);
		m_CurrentNum[i].AdjustClientYToSize(1);
		m_CurrentNum[i].SetText(_T("0"));

		m_SvrNum[i].SetNumberOfLines(1);	
		m_SvrNum[i].SetXCharsPerLine(2);
		m_SvrNum[i].SetArrange(CMatrixStatic::RIGHT);
		m_SvrNum[i].SetSize(CMatrixStatic::TINY);
		m_SvrNum[i].SetDisplayColors(RGB(0, 0, 0), RGB(255, 181, 63), RGB(103, 64, 23));
		m_SvrNum[i].AdjustClientXToSize(2);
		m_SvrNum[i].AdjustClientYToSize(1);
		sprintf( szTempChar1, "%d", i+1 );
		m_SvrNum[i].SetText(_T(szTempChar1));

		m_SvrNum[i].ShowWindow(SW_HIDE);
		m_MaxNum[i].ShowWindow(SW_HIDE);
		m_CurrentNum[i].ShowWindow(SW_HIDE);
	}

	int nServerNum;
	for( i = 0; i < m_vecState.size(); i++ )
	{
		if(m_vecState[i]->gsi.nServerType == SERVER_AGENT && m_vecState[i]->gsi.nServerNumber < 8)
		{
			nServerNum = m_vecState[i]->gsi.nServerGroup-1;
			if( nServerNum < 0 ) continue;
			m_MaxNum[nServerNum].ShowWindow(SW_SHOW);
			m_CurrentNum[nServerNum].ShowWindow(SW_SHOW);
			m_SvrNum[nServerNum].ShowWindow(SW_SHOW);

			sprintf( szTempChar1, "%d", m_vecState[i]->gsi.nServerCurrentClient );
			sprintf( szTempChar2, "%d", m_vecState[i]->gsi.nServerMaxClient );

			m_CurrentNum[nServerNum].SetText(_T(szTempChar1));
			m_MaxNum[nServerNum].SetText(_T(szTempChar2));
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

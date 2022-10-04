// JobManagerBroadcst.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include <vector>
#include "ServerManager.h"
#include "JobManagerBroadcast.h"
#include "s_CJobSchedule.h"
#include ".\jobmanagerbroadcast.h"

// CJobManagerBroadcast 대화 상자입니다.

IMPLEMENT_DYNAMIC(CJobManagerBroadcast, CDialog)
CJobManagerBroadcast::CJobManagerBroadcast(CWnd* pParent /*=NULL*/)
	: CDialog(CJobManagerBroadcast::IDD, pParent)
	, m_strMsg(_T(""))
{
	m_nHour		 = 0;
	m_nMin		 = 0;
	m_nType		 = 0;
	m_nDayOfWeek = 0; 
	m_nYear		 = 0;	  
	m_nMonth	 = 0;	  
	m_nDay		 = 0;		  
}

CJobManagerBroadcast::~CJobManagerBroadcast()
{
}

void CJobManagerBroadcast::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_HOUR, m_ComboHour);
	DDX_Control(pDX, IDC_COMBO_MIN, m_ComboMin);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_ComboType);
	DDX_Text(pDX, IDC_EDIT_BROADCAST_MESSAGE, m_strMsg);
	DDV_MaxChars(pDX, m_strMsg, 980);
}


BEGIN_MESSAGE_MAP(CJobManagerBroadcast, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_SEND_TYPE2, OnBnClickedSendType2)
	ON_BN_CLICKED(IDC_SEND_TYPE1, OnBnClickedSendType1)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnCbnSelchangeComboType)
	ON_NOTIFY(MCN_SELCHANGE, IDC_MONTHCALENDAR1, OnMcnSelchangeMonthcalendar1)
END_MESSAGE_MAP()


// CJobManagerBroadcast 메시지 처리기입니다.

BOOL CJobManagerBroadcast::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	InitControl();
		
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CJobManagerBroadcast::InitControl()
{
	CString strTemp;
	for( int i=0; i<24; ++i )
	{
		strTemp.Format("%d", i);
		m_ComboHour.AddString(strTemp);
		m_ComboHour.SetItemData(i, i);		
	}
	m_ComboHour.SetCurSel(0);

	for( i=0; i<60; ++i )
	{
		strTemp.Format("%d", i);
		m_ComboMin.AddString(strTemp);
		m_ComboMin.SetItemData(i, i);
	}
	m_ComboMin.SetCurSel(0);

	CheckRadioButton( IDC_SEND_TYPE1, IDC_SEND_TYPE2, IDC_SEND_TYPE1 );
	for( i = 0; i < 8; i++ )
	{
		GetDlgItem( IDC_SVR_CHECK1+i )->EnableWindow(FALSE);
	}

	GetDlgItem( IDC_MONTHCALENDAR1 )->ShowWindow(SW_HIDE);


	// JOB_ONCE=0, // Job execute once
	// JOB_HOUR=1, // Job execute every hour
	// JOB_DAILY=2 // Job execute every day
	m_ComboType.AddString(_T("ONCE"));
	m_ComboType.SetItemData(0, CLIENT_JOB::JOB_ONCE);
	m_ComboType.AddString(_T("Every HOUR"));
	m_ComboType.SetItemData(1, CLIENT_JOB::JOB_HOUR);
	m_ComboType.AddString(_T("Every Day"));
	m_ComboType.SetItemData(2, CLIENT_JOB::JOB_DAILY);
	m_ComboType.AddString(_T("Every Sunday"));
	m_ComboType.SetItemData(3, CLIENT_JOB::JOB_SUN);
	m_ComboType.AddString(_T("Every Monday"));
	m_ComboType.SetItemData(4, CLIENT_JOB::JOB_MON);
	m_ComboType.AddString(_T("Every Tuesday"));
	m_ComboType.SetItemData(5, CLIENT_JOB::JOB_TUE);
	m_ComboType.AddString(_T("Every Wednesday"));
	m_ComboType.SetItemData(6, CLIENT_JOB::JOB_WED);
	m_ComboType.AddString(_T("Every Thursday"));
	m_ComboType.SetItemData(7, CLIENT_JOB::JOB_THU);
	m_ComboType.AddString(_T("Every Friday"));
	m_ComboType.SetItemData(8, CLIENT_JOB::JOB_FRI);
	m_ComboType.AddString(_T("Every Saturday"));
	m_ComboType.SetItemData(9, CLIENT_JOB::JOB_SAT);
	m_ComboType.AddString(_T("Special Day"));
	m_ComboType.SetItemData(10, CLIENT_JOB::JOB_SPECIALDAY);
	m_ComboType.SetCurSel(0);

	if( m_vecGroupList.size() != 0 && m_strMsg.GetLength() != 0 )
	{
		SetDlgItemText( IDC_EDIT_BROADCAST_MESSAGE, m_strMsg.GetString() );

		m_ComboHour.SetCurSel(m_nHour);
		m_ComboMin.SetCurSel(m_nMin);
		m_ComboType.SetCurSel(m_nType);

		SYSTEMTIME currentTime; 
		currentTime.wYear  = m_nYear;	  
		currentTime.wMonth = m_nMonth;	  
		currentTime.wDay   = m_nDay;	
		CMonthCalCtrl* pCalendar = (CMonthCalCtrl*)GetDlgItem(IDC_MONTHCALENDAR1); 
		pCalendar->SetCurSel(&currentTime);

		CButton *pButton;
		if( m_vecGroupNum[0] == -1 )
		{
			pButton = (CButton *)GetDlgItem( IDC_SEND_TYPE1 );
			pButton->SetCheck(TRUE);
			pButton = (CButton *)GetDlgItem( IDC_SEND_TYPE2 );
			pButton->SetCheck(FALSE);
		}else{
			pButton = (CButton *)GetDlgItem( IDC_SEND_TYPE1 );
			pButton->SetCheck(FALSE);
			pButton = (CButton *)GetDlgItem( IDC_SEND_TYPE2 );
			pButton->SetCheck(TRUE);

			BYTE i;
			for( i = 0; i < m_vecGroupNum.size(); i++ )
			{
				pButton = (CButton *)GetDlgItem( IDC_SVR_CHECK1+i );
				pButton->SetCheck(TRUE);
			}
		}

		if( m_nType == 10 ) GetDlgItem( IDC_MONTHCALENDAR1 )->ShowWindow(SW_SHOW);


	}

}

void CJobManagerBroadcast::SetInitData( std::vector<int> vecGroupList, CString strMsg, int nType, std::vector<int> vecGroupNum, 
									    int nHour/* =0 */, int nMin/* =0 */, int nDayOfWeek/* =0 */, int nMonth/* =0 */, 
										int nDay/* =0 */, int nYear/* =0 */ )
{

	m_vecGroupList = vecGroupList;
	m_strMsg	  = strMsg;
	m_nHour		  = nHour;
	m_nMin		  = nMin;
	m_nType		  = nType;
	m_vecGroupNum = vecGroupNum;
	m_nDayOfWeek  = nDayOfWeek; // 요일
	m_nYear		  = nYear;	   // 년도
	m_nMonth	  = nMonth;	   // 달
	m_nDay		  = nDay;	   // 날짜
}

void CJobManagerBroadcast::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if( m_strMsg.GetLength() < 10 ) return;

	m_vecGroupNum.clear();

	SYSTEMTIME currentTime; 
	CMonthCalCtrl* pCalendar = (CMonthCalCtrl*)GetDlgItem(IDC_MONTHCALENDAR1); 
	pCalendar->GetCurSel(&currentTime);

	m_nYear  = currentTime.wYear;	  
	m_nMonth = currentTime.wMonth;	  
	m_nDay   = currentTime.wDay;		
	m_nHour		= static_cast <int> (m_ComboHour.GetItemData(m_ComboHour.GetCurSel()));
	m_nMin		= static_cast <int> (m_ComboMin.GetItemData(m_ComboMin.GetCurSel())); 
	m_nType		= static_cast <int> (m_ComboType.GetItemData(m_ComboType.GetCurSel()));

	if( m_nType >= CLIENT_JOB::JOB_SUN )
	{
		m_nDayOfWeek = m_nType - CLIENT_JOB::JOB_SUN;
		m_nDayOfWeek++;
	}

	CButton *pButton = (CButton *)GetDlgItem( IDC_SEND_TYPE2 );
	if( pButton->GetCheck() )
	{
		BYTE i;
		for( i = 0; i < 8; i++ )
		{
			pButton = (CButton *)GetDlgItem( IDC_SVR_CHECK1+i );
			if( pButton->GetCheck() )
			{
				m_vecGroupNum.push_back( i );
			}
		}
	}else{
		m_vecGroupNum.push_back(-1);
	}
	m_strBroadcastMsg = m_strMsg;

	if( m_vecGroupNum.size() == 0 ) return;
	OnOK();
}

void CJobManagerBroadcast::OnBnClickedSendType1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BYTE i;
	for( i = 0; i < 8; i++ )
	{
		GetDlgItem( IDC_SVR_CHECK1+i )->EnableWindow(FALSE);
	}
}

void CJobManagerBroadcast::OnBnClickedSendType2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BYTE i;
	for( i = 0; i < m_vecGroupList.size(); i++ )
	{
		GetDlgItem( IDC_SVR_CHECK1+m_vecGroupList[i]-1 )->EnableWindow(TRUE);
	}
}

void CJobManagerBroadcast::OnCbnSelchangeComboType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( m_ComboType.GetCurSel() == 10 )
	{
		GetDlgItem( IDC_MONTHCALENDAR1 )->ShowWindow(SW_SHOW);
	}else{
		GetDlgItem( IDC_MONTHCALENDAR1 )->ShowWindow(SW_HIDE);
	}


	if( m_ComboType.GetCurSel() == 1 )
	{
		m_ComboHour.EnableWindow(FALSE);
	}else{
		m_ComboHour.EnableWindow(TRUE);
	}

}



void CJobManagerBroadcast::OnMcnSelchangeMonthcalendar1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMSELCHANGE pSelChange = reinterpret_cast<LPNMSELCHANGE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


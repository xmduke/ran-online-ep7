// JobManagerDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServerManager.h"
#include "ServerManagerDlg.h"
#include "JobManagerBroadcast.h"

#include "s_CJobSchedule.h"
#include ".\jobmanagerdlg.h"

using namespace CLIENT_JOB;

// CJobManagerDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CJobManagerDlg, CDialog)
CJobManagerDlg::CJobManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJobManagerDlg::IDD, pParent)
{
	m_pDlg = reinterpret_cast <CDialog*> (pParent);
	Create();
}

CJobManagerDlg::~CJobManagerDlg()
{
}

BOOL CJobManagerDlg::Create()
{
	return CDialog::Create(CJobManagerDlg::IDD);
}

void CJobManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_JOB, m_ListJob);
}

BEGIN_MESSAGE_MAP(CJobManagerDlg, CDialog)	
	ON_WM_NCDESTROY()	
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BTN_BROADCAST_MESSAGE_ADD, OnBnClickedBtnBroadcastMessageAdd)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_JOB, OnNMRclickListJob)
	ON_COMMAND(ID_JOB_ADD, OnJobAdd)
	ON_COMMAND(ID_JOB_EDIT, OnJobEdit)
	ON_COMMAND(ID_JOB_DELETE, OnJobDelete)
	ON_BN_CLICKED(IDC_SCH_EXPORT, OnBnClickedSchExport)
	ON_BN_CLICKED(IDC_SCH_IMPORT, OnBnClickedSchImport)
END_MESSAGE_MAP()

// CJobManagerDlg 메시지 처리기입니다.

BOOL CJobManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	ShowWindow(SW_SHOW || SW_RESTORE);

	InitControl();

	Refresh();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CJobManagerDlg::InitControl()
{
	m_ListJob.SetExtendedStyle(LVS_EX_FULLROWSELECT); 

    LVCOLUMN Col;

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_LEFT;	
	Col.pszText		= _T("Hour");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListJob.InsertColumn(0, &Col);

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_LEFT;
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	Col.pszText		= _T("Min");
	m_ListJob.InsertColumn(1, &Col);

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 90;
	Col.fmt			= LVCFMT_LEFT;
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	Col.pszText		= _T("Type");
	m_ListJob.InsertColumn(2, &Col);

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 400;
	Col.fmt			= LVCFMT_LEFT;
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	Col.pszText		= _T("Description");
	m_ListJob.InsertColumn(3, &Col);
}

void CJobManagerDlg::OnNcDestroy()
{
	CDialog::OnNcDestroy();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CServerManagerDlg *pDlg;
	pDlg = reinterpret_cast <CServerManagerDlg*> (m_pDlg);
	pDlg->m_pJobDlg = NULL;
	delete this;
}

void CJobManagerDlg::Refresh()
{
	m_ListJob.DeleteAllItems();
	std::vector<CBroadCast> vecJob = CLIENT_JOB::CScheduler::GetInstance()->GetJobList();
	
	size_t i;
	for (i = 0; i < vecJob.size(); i++ )
	{	
		CBroadCast action = vecJob[i];
        
		int nCount;
		nCount = m_ListJob.GetItemCount();

		CString strTemp;
		// Save pointer
		LV_ITEM lvItem;		
		ZeroMemory(&lvItem, sizeof(LV_ITEM));
		lvItem.mask		= LVIF_TEXT; // 이미지 속성 지정
		lvItem.iItem	= nCount;
		lvItem.iSubItem = 0;		
		if( action.GetType() == JOB_HOUR )
		{
			lvItem.pszText	= "every";
			m_ListJob.InsertItem(&lvItem); // 새로운 열의 시작은 InsertItem을 사용한다.
		}else{
			strTemp.Format("%d", action.GetHour());
			lvItem.pszText	= strTemp.GetBuffer();
			m_ListJob.InsertItem(&lvItem); // 새로운 열의 시작은 InsertItem을 사용한다.
		}
		
		strTemp.Format("%d", action.GetMin());
		m_ListJob.SetItemText(nCount, 1, strTemp.GetString());

		switch (action.GetType())
		{
		case JOB_ONCE:
			m_ListJob.SetItemText(nCount, 2, _T("ONCE"));
			break;
		case JOB_HOUR:
			m_ListJob.SetItemText(nCount, 2, _T("Every HOUR"));
			break;
		case JOB_DAILY:
			m_ListJob.SetItemText(nCount, 2, _T("Every Day"));
			break;
		case JOB_SUN:
			m_ListJob.SetItemText(nCount, 2, _T("Every SUN"));
			break;
		case JOB_MON:
			m_ListJob.SetItemText(nCount, 2, _T("Every MON"));
			break;
		case JOB_TUE:
			m_ListJob.SetItemText(nCount, 2, _T("Every TUE"));
			break;
		case JOB_WED:
			m_ListJob.SetItemText(nCount, 2, _T("Every WED"));
			break;
		case JOB_THU:
			m_ListJob.SetItemText(nCount, 2, _T("Every THU"));
			break;
		case JOB_FRI:
			m_ListJob.SetItemText(nCount, 2, _T("Every FRI"));
			break;
		case JOB_SAT:
			m_ListJob.SetItemText(nCount, 2, _T("Every SAT"));
			break;
		case JOB_SPECIALDAY:
			{
				strTemp.Format("%d:%d:%d", action.GetYear(), action.GetMonth(), action.GetDay());
				m_ListJob.SetItemText(nCount, 2, strTemp );
			}
			break;

		}
		strTemp = action.GetDescription();
		m_ListJob.SetItemText(nCount, 3, strTemp.GetString());
//		m_ListJob.SetItemData(nCount, reinterpret_cast <DWORD_PTR> (action));
	}
}


void CJobManagerDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CJobManagerDlg::OnBnClickedBtnBroadcastMessageAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CJobManagerBroadcast Dlg;	
	Dlg.SetGroupList(m_vecGroupList);
	if (Dlg.DoModal() == IDOK)
	{
		int nHour = Dlg.m_nHour;
		int nMin  = Dlg.m_nMin;
		int nType = Dlg.m_nType;
		int	nDayOfWeek = Dlg.m_nDayOfWeek;
		int	nYear      = Dlg.m_nYear;	  
		int	nMonth	   = Dlg.m_nMonth;	  
		int	nDay	   = Dlg.m_nDay;		  
		std::vector<int> vecGroupNum = Dlg.m_vecGroupNum;
		CString strBroadcastMsg = Dlg.m_strBroadcastMsg;

		if (strBroadcastMsg.GetLength() > 0)
		{
			CLIENT_JOB::CBroadCast BroadCast; 			
			BroadCast.SetValue( m_vecGroupList, strBroadcastMsg, nType, vecGroupNum, nHour, nMin, nDayOfWeek, nMonth, nDay, nYear );


			CLIENT_JOB::CScheduler::GetInstance()->AddBrocastJob(BroadCast);
			Refresh();
		}
	}
}

void CJobManagerDlg::OnNMRclickListJob(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 마우스 좌표 얻기
	POINT point;
	GetCursorPos(&point);

	// 메뉴 표시
	CMenu menuTemp, *pContextMenu;
	menuTemp.LoadMenu(IDR_SERVER_JOB_MENU);
	pContextMenu = menuTemp.GetSubMenu(0);
	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN,		
		point.x,          // 메뉴가 표시될 x 좌표
		point.y,          // 메뉴가 표시될 y 좌표
		AfxGetMainWnd()   // WM_COMMAND 메시지를 받을 윈도우
		);
	*pResult = 0;
}


void CJobManagerDlg::OnJobAdd()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	OnBnClickedBtnBroadcastMessageAdd();
}

void CJobManagerDlg::OnJobEdit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	int nSelected = m_ListJob.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	if (nSelected != -1) // 선택된 스케쥴이 없음
	{
		CJobManagerBroadcast *pDlg = new CJobManagerBroadcast(this);	
		std::vector<CBroadCast> vecJob = CLIENT_JOB::CScheduler::GetInstance()->GetJobList();		

		pDlg->SetInitData( vecJob[nSelected].GetGroupList(), vecJob[nSelected].GetStr(), vecJob[nSelected].GetType(), vecJob[nSelected].GetGroupNum(),
						   vecJob[nSelected].GetHour(), vecJob[nSelected].GetMin(), vecJob[nSelected].GetDayOfWeek(), vecJob[nSelected].GetMonth(),
						   vecJob[nSelected].GetDay(), vecJob[nSelected].GetYear() );
		if (pDlg->DoModal() == IDOK)
		{
			int nHour = pDlg->m_nHour;
			int nMin  = pDlg->m_nMin;
			int nType = pDlg->m_nType;
			int	nDayOfWeek = pDlg->m_nDayOfWeek;
			int	nYear      = pDlg->m_nYear;	  
			int	nMonth	   = pDlg->m_nMonth;	  
			int	nDay	   = pDlg->m_nDay;		  
			std::vector<int> vecGroupNum = pDlg->m_vecGroupNum;
			CString strBroadcastMsg = pDlg->m_strBroadcastMsg;
			
			vecJob[nSelected].SetValue( m_vecGroupList, strBroadcastMsg, nType, vecGroupNum, nHour, nMin, nDayOfWeek, nMonth, nDay, nYear );
			CLIENT_JOB::CScheduler::GetInstance()->SetJobList( vecJob );
			Refresh();

			SAFE_DELETE( pDlg );
		}
	}
}

void CJobManagerDlg::OnJobDelete()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	int nSelected = m_ListJob.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	if (nSelected != -1) // 선택된 스케쥴이 없음
	{
		std::vector<CBroadCast> vecJob = CLIENT_JOB::CScheduler::GetInstance()->GetJobList();
		vecJob.erase( vecJob.begin() + nSelected );
		CLIENT_JOB::CScheduler::GetInstance()->SetJobList( vecJob );
		Refresh();
	}
}

void CJobManagerDlg::OnBnClickedSchExport()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Serever Schedule File (*.sch)|*.sch|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,".sch",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CJobManagerDlg*)this);


	char szTempChar[256];
	GetCurrentDirectory( 256, szTempChar );
	dlg.m_ofn.lpstrInitialDir = szTempChar;

	if ( dlg.DoModal() == IDOK )
	{
		FILE *pFile = fopen( dlg.GetFileName().GetString(), "wb" );
		if( pFile == NULL )
		{
			MessageBox( "File Save Error", "Error" );
			return;
		}

		int nHour, nMin, nType, nDayOfWeek,	nYear, nMonth, nDay;		  
		std::vector<int> vecGroupNum;
		CString strBroadcastMsg;

		std::vector<CBroadCast> vecJob = CLIENT_JOB::CScheduler::GetInstance()->GetJobList();
		size_t i, j, nSize;
		nSize = vecJob.size();
		fwrite( &nSize, sizeof(int), 1, pFile );
		for( i = 0; i < vecJob.size(); i++ )
		{
			nType = vecJob[i].GetType();
			nHour = vecJob[i].GetHour();
			nMin  = vecJob[i].GetMin();
			nDayOfWeek  = vecJob[i].GetDayOfWeek();
			nYear       = vecJob[i].GetYear();	  
			nMonth	    = vecJob[i].GetMonth();	  
			nDay	    = vecJob[i].GetDay();		  
			strBroadcastMsg = vecJob[i].GetStr();
			vecGroupNum = vecJob[i].GetGroupNum();

			fwrite( &nType, sizeof(int), 1, pFile );
			fwrite( &nHour, sizeof(int), 1, pFile );
			fwrite( &nMin, sizeof(int), 1, pFile );
			fwrite( &nDayOfWeek, sizeof(int), 1, pFile );
			fwrite( &nYear, sizeof(int), 1, pFile );
			fwrite( &nMonth, sizeof(int), 1, pFile );
			fwrite( &nDay, sizeof(int), 1, pFile );
			nSize = strBroadcastMsg.GetLength();
			fwrite( &nSize, sizeof(int), 1, pFile );
			fwrite( strBroadcastMsg.GetString(), sizeof(char), nSize+1, pFile );

			nSize = vecGroupNum.size();
			fwrite( &nSize, sizeof(int), 1, pFile );
			for( j = 0; j < vecGroupNum.size(); j++ )
			{
				fwrite( &vecGroupNum[i], sizeof(int), 1, pFile );
			}
		}

		fclose( pFile );
	}
}

void CJobManagerDlg::OnBnClickedSchImport()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Serever Schedule File (*.sch)|*.sch|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".sch",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CJobManagerDlg*)this);

	char szTempChar[256];
	GetCurrentDirectory( 256, szTempChar );
	dlg.m_ofn.lpstrInitialDir = szTempChar;
	if ( dlg.DoModal() == IDOK )
	{
		CLIENT_JOB::CScheduler::GetInstance()->ClearJobList();

		FILE *pFile = fopen( dlg.GetFileName().GetString(), "rb" );
		if( pFile == NULL )
		{
			MessageBox( "File Open Error", "Error" );
			return;
		}


		int nHour, nMin, nType, nDayOfWeek,	nYear, nMonth, nDay, nTemp;		  
		std::vector<int> vecGroupNum;
		CString strBroadcastMsg;
		char szTempChar[256];

		size_t i, j, nSize1, nSize2;	
		fread( &nSize1, sizeof(int), 1, pFile );
		for( i = 0; i < nSize1; i++ )
		{
			ZeroMemory( szTempChar, 256 );
			vecGroupNum.clear();

			fread( &nType, sizeof(int), 1, pFile );
			fread( &nHour, sizeof(int), 1, pFile );
			fread( &nMin, sizeof(int), 1, pFile );
			fread( &nDayOfWeek, sizeof(int), 1, pFile );
			fread( &nYear, sizeof(int), 1, pFile );
			fread( &nMonth, sizeof(int), 1, pFile );
			fread( &nDay, sizeof(int), 1, pFile );

			fread( &nSize2, sizeof(int), 1, pFile );
			fread( szTempChar, sizeof(char), nSize2+1, pFile );
			strBroadcastMsg = szTempChar;

			fread( &nSize2, sizeof(int), 1, pFile );
			for( j = 0; j < nSize2; j++ )
			{
				fread( &nTemp, sizeof(int), 1, pFile );
				vecGroupNum.push_back( nTemp );
			}
	
			CLIENT_JOB::CBroadCast BroadCast; 			
			BroadCast.SetValue( m_vecGroupList, strBroadcastMsg, nType, vecGroupNum, nHour, nMin, nDayOfWeek, nMonth, nDay, nYear );
			CLIENT_JOB::CScheduler::GetInstance()->AddBrocastJob(BroadCast);
		}

		fclose( pFile );

		Refresh();
	}
}

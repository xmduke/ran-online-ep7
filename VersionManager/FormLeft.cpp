// FormLeft.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VersionManager.h"
#include "FormLeft.h"
#include "MainFrm.h"
#include "DlgNewFolder.h"
#include "CCfg.h"
#include ".\formleft.h"

// CFormLeft

IMPLEMENT_DYNCREATE(CFormLeft, CFormView)

CFormLeft::CFormLeft()
	: CFormView(CFormLeft::IDD)
{
	m_bInit = FALSE;
	nSelectedItem = 0;
}

CFormLeft::~CFormLeft()
{
}

void CFormLeft::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
	DDX_Control(pDX, IDC_LIST1, m_Folder);
}

BEGIN_MESSAGE_MAP(CFormLeft, CFormView)	
	ON_WM_SIZE()	
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnLvnItemchangedList1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnNMRclickList1)	
	ON_COMMAND(ID__MAKE, OnMakeCmd)
	ON_COMMAND(ID__DELETE, OnDeleteCmd)
    ON_WM_TIMER()
END_MESSAGE_MAP()

// CFormLeft 진단입니다.

#ifdef _DEBUG
void CFormLeft::AssertValid() const
{
	CFormView::AssertValid();
}

void CFormLeft::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CFormLeft 메시지 처리기입니다.
void CFormLeft::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_Folder.SetExtendedStyle ( LVS_EX_FULLROWSELECT ); 

	InitListHead(); // 리스트 컨트롤 헤더 초기화
	InitListData(); // 리스트 컨트롤 Data 초기화

    SetTimer(100, 1000, NULL);

	m_bInit = TRUE;
}

void CFormLeft::InitListHead()
{
	LVCOLUMN Col;
	CString strTemp;

	strTemp			= "Directory Name";	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 400;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_Folder.InsertColumn(0, &Col);
}

void CFormLeft::InitListData()
{	
	int nRetCode;
	// Erase all list items
	m_Folder.DeleteAllItems();
	// Erase all vector elements
	m_vFolder.erase(m_vFolder.begin(), m_vFolder.end());

	CMainFrame* pFrame = (CMainFrame *) AfxGetMainWnd();
	nRetCode = COdbcManager::GetInstance()->GetFolderList(m_vFolder);

	if (nRetCode == DB_ERROR)
	{
		MessageBox("폴더리스트를 가져올 수 없습니다", "ERROR", MB_ICONEXCLAMATION);
		return;
	}

	for (int i=0; i<(int) m_vFolder.size(); i++)
	{		
		int nCount; 
		nCount = m_Folder.GetItemCount();

		LV_ITEM lvItem;
		::memset(&lvItem, 0, sizeof(LV_ITEM));
		lvItem.mask = LVIF_TEXT; // 속성 지정
		lvItem.iItem = nCount;
		lvItem.iSubItem = 0;		
		lvItem.pszText = (&m_vFolder[i])->strName.GetBuffer();
		// lvItem.iImage = 0; // 이미지의 색인 번호
		m_Folder.InsertItem(&lvItem); // 새로운 열의 시작은 InsertItem을 사용한다.
	}    
}

void CFormLeft::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 컨트롤 사이즈 조정
	if ( m_bInit && nType != SIZE_MINIMIZED )
	{		
		CRect cRect;
		GetClientRect(&cRect);
				
		// CRect ctlRect;
		// m_Folder.GetWindowRect(&ctlRect);
		m_Edit.MoveWindow(10, 5, cx-15, 25);
		m_Folder.MoveWindow(10, 30, cx-15, cy-30);
	}
}

void CFormLeft::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSelected = m_Folder.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	if (nSelected != -1)
	{
		// 선택된 폴더를 에디터 박스에 출력한다.
		m_Edit.SetWindowText((&m_vFolder[nSelected])->strName.GetString());
		// 선택된 폴더의 파일들을 오른쪽 리스트에 보여준다.
		CMainFrame* pFrame = (CMainFrame *) AfxGetMainWnd();
		pFrame->m_pRight->FillData((&m_vFolder[nSelected])->nIdx);		
		nSelectedItem = nSelected;
	}
	*pResult = 0;
}

void CFormLeft::OnNMRclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 선택된 서버가 있는지 확인한다.
	int nSelected = m_Folder.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	
	if (nSelected != -1) // 선택된 항목이 있음
	{	
		// 마우스 좌표 얻기
		POINT point;
		GetCursorPos(&point);
		
		// 메뉴 표시
		CMenu menu;
		menu.LoadMenu (IDR_CONTEXT_LEFT);
		CMenu* pContextMenu = menu.GetSubMenu (0);		
        pContextMenu->TrackPopupMenu (
					TPM_LEFTALIGN |	TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
					point.x, point.y,
					AfxGetMainWnd ());
	}
	*pResult = 0;
}

void CFormLeft::OnMakeCmd()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	// 새로운 폴더를 생성한다.	

	CString strTemp;
	m_Edit.GetWindowText(strTemp);
	CDlgNewFolder dlg(strTemp);
	int nRet;
	nRet = (int) dlg.DoModal();

	if (nRet == IDOK)
	{
		strTemp = CCfg::GetInstance()->m_szTargetPath + dlg.m_strFolder;		
		if (CreateDirectory(strTemp, NULL) == 0)
		{
			// 생성실패
			MessageBox("생성에 실패하였습니다", "ERROR", MB_ICONEXCLAMATION);
		}
		else
		{
			// 생성성공
			MessageBox("생성에 성공하였습니다");
			// DB 에 생성된 폴더 이름 입력
			CMainFrame* pFrame = (CMainFrame *) AfxGetMainWnd();
			if (COdbcManager::GetInstance()->CreateFolder(dlg.m_strFolder) != DB_OK)
			{
				MessageBox("디렉토리는 생성되었지만 DB 에 입력되지는 않았습니다");
			
			}
			// Left form refresh
			InitListData();
			// Right form refresh
			pFrame->m_pLeft->InitListData();
		}	
	}
}

void CFormLeft::OnDeleteCmd()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString strTemp;
	m_Edit.GetWindowText(strTemp);
	strTemp = CCfg::GetInstance()->m_szTargetPath + strTemp;

	if (RemoveDirectory(strTemp) == 0)
	{
		MessageBox("삭제에 실패하였습니다", "ERROR", MB_ICONEXCLAMATION);
	}
	else	
	{
		// 선택된 서버가 있는지 확인한다.
		int nSelected = m_Folder.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		
		if (nSelected != -1) // 선택된 항목이 있음
		{	
			int nDir = m_vFolder[nSelected].nIdx; 
			//// 디렉토리 정보를 DB 에서 지운다.
			CMainFrame* pFrame = (CMainFrame *) AfxGetMainWnd();
			if (COdbcManager::GetInstance()->RemoveFolder(nDir) == DB_OK)
			{
				MessageBox("올바르게 삭제되었습니다");
			}
			else
			{
				MessageBox("디렉토리는 지워졌지만 DB 에서 지워지지 않았습니다");
			}
			// Left form refresh
			InitListData();
			// Right form refresh
			pFrame->m_pLeft->InitListData();
		}
		else
		{
			MessageBox("선택된 디렉토리가 없습니다");
		}
	}
}

void CFormLeft::OnTimer(UINT nIDEvent)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    if (nIDEvent == 100)
    {
        KillTimer(100);
        if (m_Folder.GetItemCount() > 0)
        {
            m_Folder.SetFocus();
            m_Folder.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);          
        }
    }

    CFormView::OnTimer(nIDEvent);
}

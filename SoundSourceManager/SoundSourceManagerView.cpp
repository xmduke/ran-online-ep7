// SoundSourceManagerView.cpp : CSoundSourceManagerView 클래스의 구현
//

#include "stdafx.h"
#include "SoundSourceManager.h"

#include "SoundSourceManagerDoc.h"
#include "SoundSourceManagerView.h"

#include "SoundSourceMan.h"
#include "SoundSourceDlg.h"

#include "dsutil.h"
#include "dxutil.h"
#include ".\soundsourcemanagerview.h"
#include "WavFileIntegrity.h"
#include "File2ListDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSoundSourceManagerView

static	char	g_szDataDir[]		= "Sounds\\Sfx\\";

IMPLEMENT_DYNCREATE(CSoundSourceManagerView, CFormView)

BEGIN_MESSAGE_MAP(CSoundSourceManagerView, CFormView)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_WRITE, OnBnClickedButtonWrite)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, OnBnClickedButtonModify)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SOUNDSOURCE_TABLE, OnNMDblclkListSoundsourceTable)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SOUNDSOURCE_TABLE, OnNMClickListSoundsourceTable)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SOUNDSOURCE_TABLE, OnNMRclickListSoundsourceTable)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnBnClickedButtonFind)
	ON_CBN_SELCHANGE(IDC_COMBO_CATEGORY, OnCbnSelchangeComboCategory)
	ON_COMMAND(ID_MENUITEM_TEXTOUT, OnMenuitemTextout)
	ON_COMMAND(ID_MENUITEM_LOAD, OnMenuitemLoad)
	ON_COMMAND(ID_MENUITEM_SAVE, OnMenuitemSave)
	ON_COMMAND(ID_MENUITEM_INTEGRITY, OnMenuitemIntegrity)
	ON_COMMAND(ID_MENUITEM_FILE2LIST_TRACE, OnMenuitemFile2listTrace)
END_MESSAGE_MAP()

// CSoundSourceManagerView 생성/소멸

CSoundSourceManagerView::CSoundSourceManagerView()
	: CFormView(CSoundSourceManagerView::IDD)
	, m_valKeyword(_T(""))
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_pSoundManager = NULL;
	m_pSound = NULL;
	m_bNormal = FALSE;
}

CSoundSourceManagerView::~CSoundSourceManagerView()
{
	if ( m_pSound != NULL )
	{
		m_pSound->Stop();
		SAFE_DELETE ( m_pSound );
	}	
	SAFE_DELETE ( m_pSoundManager );
	SAFE_DELETE ( m_pSoundSourceMan );
}

void CSoundSourceManagerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SOUNDSOURCE_TABLE, m_ctrlSoundSourceTable);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_ctrlButtonDelete);
	DDX_Control(pDX, IDC_BUTTON_MODIFY, m_ctrlButtonModify);
	DDX_Control(pDX, IDC_BUTTON_FIND, m_ctrlButtonFind);
	DDX_Control(pDX, IDC_BUTTON_WRITE, m_ctrlButtonWrite);
	DDX_Control(pDX, IDC_EDIT_KEYWORD, m_ctrlEditKeyword);
	DDX_Text(pDX, IDC_EDIT_KEYWORD, m_valKeyword);
	DDX_Control(pDX, IDC_COMBO_CATEGORY, m_ctrlCategory);
	DDX_Control(pDX, IDC_STATIC_CATEGORY, m_ctrlStaticCategory);
}

BOOL CSoundSourceManagerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.

	return CFormView::PreCreateWindow(cs);
}

void CSoundSourceManagerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	//	데이타
	m_pSoundSourceMan = new CSoundSourceMan;

	CSoundSourceManagerApp	*pApp = (CSoundSourceManagerApp*)AfxGetApp();	
	CString DataDirectory = pApp->m_strAppPath + g_szDataDir;

	m_pSoundSourceMan->SetDataDirectory ( DataDirectory );
	CreateDirectory ( DataDirectory.GetString(), NULL );

	m_ctrlSoundSourceTable.SetExtendedStyle (
		m_ctrlSoundSourceTable.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_ImageList.Create ( IDB_BITMAP_STATE, 16, 1, (COLORREF)-1 );
	m_ctrlSoundSourceTable.SetImageList ( &m_ImageList, LVSIL_SMALL );

	RECT	rect;
	m_ctrlSoundSourceTable.GetClientRect ( &rect );
	LONG	Width = rect.right - rect.left;

	LV_COLUMN	lvcolumn;
	int			ColumnCount = 7;	
	char		*ColumnName[7] = { "!", "ID", "종류", "버퍼수", "파일", "날짜", "코멘트" };
	int			ColumnWidthPercent[7] = { 2, 10, 10, 10, 15, 15, 38 };

	for ( int i = 0; i < ColumnCount; i++ )
	{		
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM
			| LVCF_TEXT | LVCF_WIDTH;// | LVCF_IMAGE;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = ColumnName[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = ( Width * ColumnWidthPercent[i] ) / 100;
		m_ctrlSoundSourceTable.InsertColumn (i, &lvcolumn);		
	}

	for ( i = 0; i < NTypeDesc::DescCount; i++ )
	{
		int nIndex = m_ctrlCategory.AddString ( NTypeDesc::Desc[i] );
		m_ctrlCategory.SetItemData ( nIndex, i );
	}
    m_ctrlCategory.SetCurSel ( m_ctrlCategory.GetCount () - 1 );

	//	<--	로드데이타를 화면에 뿌림	-->	//
	ReloadAllItems();

	m_pSoundManager = new CSoundManager();
    
	if( FAILED( m_pSoundManager->Initialize( m_hWnd, DSSCL_PRIORITY, 1, 22050, 16 ) ) )
	{
		MessageBox( "DirectSound 초기화에 실패했습니다.", 
					"DirectSound", MB_OK | MB_ICONERROR );
		return;
	}

	m_bNormal = TRUE;
}




// CSoundSourceManagerView 진단

#ifdef _DEBUG
void CSoundSourceManagerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSoundSourceManagerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CSoundSourceManagerDoc* CSoundSourceManagerView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSoundSourceManagerDoc)));
	return (CSoundSourceManagerDoc*)m_pDocument;
}
#endif //_DEBUG


// CSoundSourceManagerView 메시지 처리기

void CSoundSourceManagerView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if( m_ctrlSoundSourceTable.m_hWnd )
	{
		m_ctrlStaticCategory.MoveWindow ( 20, 13, 70, 17, FALSE );
		m_ctrlCategory.MoveWindow ( 90, 10, 180, 20, FALSE );		

		RECT	rect;
		GetClientRect ( &rect );

		LONG	Width = rect.right - rect.left;
		LONG	Height = rect.bottom - rect.top;
        
		m_ctrlSoundSourceTable.MoveWindow( 20, 40, Width - 40, Height - 80, FALSE);

		((CEdit*)GetDlgItem(IDC_EDIT_SRCDIRECTORY))->MoveWindow ( Width-320, 10, 300, 20, FALSE );

		//	<--	총 셀 길이 구함	-->	//
		int	ColumnCount = 7;
		LONG	TotalColumnWidth = 0;
		LONG	ColumnWidth = 0;
		int		ColumnPercent = 0;
		for ( int i = 0; i < ColumnCount; i++ )
		{
			ColumnWidth = m_ctrlSoundSourceTable.GetColumnWidth ( i );
			TotalColumnWidth += ColumnWidth;
		}

		//	<--	셀의 % 구함	-->	//
		int	ColumnWidthPercent[7];
		for ( int i = 0; i < ColumnCount; i++ )
		{
			ColumnWidth = m_ctrlSoundSourceTable.GetColumnWidth ( i );
			ColumnWidthPercent[i] = (int)((float)( ColumnWidth * 100 ) / (float)TotalColumnWidth + 0.5f);
		}

        //	<--	테이블 실제 가로 크기를 구함	-->	//
		RECT	TableRect;
		m_ctrlSoundSourceTable.GetClientRect ( &TableRect );
		LONG	TableWidth = TableRect.right - TableRect.left;

		//	<--	테이블의 실제 가로 길이에 %만큼 Column길이를 적용시킴	-->	//
		for ( int i = 0; i < ColumnCount; i++ )
		{
			m_ctrlSoundSourceTable.SetColumnWidth ( i, ( TableWidth * ColumnWidthPercent[i] ) / 100 );
		}

		//	<--	버튼 및 에디트박스 컨트롤 이동	-->	//
		RECT	ButtonRect;
		m_ctrlButtonDelete.GetClientRect ( &ButtonRect );
		LONG	ButtonWidth = ButtonRect.right - ButtonRect.left;
		LONG	ButtonHeight = ButtonRect.bottom - ButtonRect.top;

//	<--	작동하지 않음, 이상함
//		RECT	EditRect;
//		m_ctrlEditKeyword.GetClientRect ( &EditRect );
//		LONG	EditWidth = EditRect.right - EditRect.left;
//	-->
		LONG	EditWidth = 200;

		LONG	ButtonInterval = 10;
		LONG	ButtonCx = (Width / 2) - ((ButtonWidth * 4 + EditWidth + ButtonInterval * 4) / 2);
		m_ctrlButtonDelete.MoveWindow	( ButtonCx + ButtonWidth * 0 + ButtonInterval * 0 + EditWidth * 0, Height - 30, ButtonWidth, ButtonHeight, FALSE );
		m_ctrlButtonModify.MoveWindow	( ButtonCx + ButtonWidth * 1 + ButtonInterval * 1 + EditWidth * 0, Height - 30, ButtonWidth, ButtonHeight, FALSE );
		m_ctrlEditKeyword.MoveWindow	( ButtonCx + ButtonWidth * 2 + ButtonInterval * 2 + EditWidth * 0, Height - 30, EditWidth,   ButtonHeight, FALSE );
		m_ctrlButtonFind.MoveWindow		( ButtonCx + ButtonWidth * 2 + ButtonInterval * 3 + EditWidth * 1, Height - 30, ButtonWidth, ButtonHeight, FALSE );
		m_ctrlButtonWrite.MoveWindow	( ButtonCx + ButtonWidth * 3 + ButtonInterval * 4 + EditWidth * 1, Height - 30, ButtonWidth, ButtonHeight, FALSE );

		InvalidateRect ( NULL );			 
	}
}

void CSoundSourceManagerView::OnBnClickedButtonWrite()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSoundSourceDlg	dlg;
	
	dlg.SetState ( eNew );
	if ( dlg.DoModal() == IDOK )
	{	
		//	<--	이전 사운드 객체 죽이기	-->	//
		if( m_pSound )
		{
			m_pSound->Stop();
			m_pSound->Reset();		
			SAFE_DELETE( m_pSound );
		}

		ReloadAllItems ();
	}
}

void CSoundSourceManagerView::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DeleteSelectItems();
}

void CSoundSourceManagerView::OnBnClickedButtonModify()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION	pos = m_ctrlSoundSourceTable.GetFirstSelectedItemPosition ();
	if ( pos )
	{
		//	<--	이전 사운드 객체 죽이기	-->	//
		if( m_pSound )
		{
			m_pSound->Stop();
			m_pSound->Reset();		
			SAFE_DELETE( m_pSound );
		}

		int nItem = m_ctrlSoundSourceTable.GetNextSelectedItem ( pos );
		if ( nItem == -1 )
		{
			return ;
		}

		SSoundSource* pRecord = (SSoundSource*)m_ctrlSoundSourceTable.GetItemData ( nItem );		

		CSoundSourceDlg	dlg;
		dlg.SetState ( eModify, pRecord->ID );
		if ( dlg.DoModal () )
		{
			ReloadAllItems ();
		}
	}
}

void CSoundSourceManagerView::OnNMDblclkListSoundsourceTable(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	//	<--	사운드 매니저 객체 초기화에 실패한 경우
	if ( !m_bNormal )
	{
		return;
	}

	//	<--	이전 사운드 객체 죽이기	-->	//
	if( m_pSound )
	{
		m_pSound->Stop();
		m_pSound->Reset();		
		SAFE_DELETE( m_pSound );
	}
	
	POSITION	pos = m_ctrlSoundSourceTable.GetFirstSelectedItemPosition ();
	int nItem = m_ctrlSoundSourceTable.GetNextSelectedItem ( pos );
	if ( nItem == -1 )
	{
		return ;
	}

	SSoundSource* pRecord = (SSoundSource*)m_ctrlSoundSourceTable.GetItemData ( nItem );

	//	<--	재생 파일 이름(FullPath) 만들기	-->	//
	CString	TargetFile = GetSSM()->GetDataDirectory() + pRecord->FileName;
	
	//	<--	Wav파일 버퍼에 들어 올리기	-->	//
	if( FAILED( m_pSoundManager->Create( &m_pSound, TargetFile.GetBuffer(), 0, GUID_NULL ) ) )
	{
		MessageBox ( "버퍼 생성에 실패하였습니다." );
		return; 
	}

	if( FAILED( m_pSound->Play( 0, 0 ) ) )
	{
		MessageBox ( "재생에 실패하였습니다." );
		return;
	}
}

void CSoundSourceManagerView::OnNMClickListSoundsourceTable(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	CSoundSourceManagerDoc *pDoc = (CSoundSourceManagerDoc *) GetDocument ();
	POSITION	pos = m_ctrlSoundSourceTable.GetFirstSelectedItemPosition ();
	int nItem = m_ctrlSoundSourceTable.GetNextSelectedItem ( pos );
	if ( nItem == -1 )
	{
		return ;
	}

	SSoundSource* pRecord = (SSoundSource*)m_ctrlSoundSourceTable.GetItemData ( nItem );

	m_ctrlButtonDelete.EnableWindow ( pRecord->State );
	m_ctrlButtonModify.EnableWindow ( pRecord->State );
}

void CSoundSourceManagerView::OnNMRclickListSoundsourceTable(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	int pMenuID = 0;
	CMenu Menu;

	DWORD SelectionMade;
	VERIFY ( Menu.LoadMenu ( IDR_MENU_POPUP ) );

	CMenu* pPopup = Menu.GetSubMenu ( pMenuID );
	ASSERT ( pPopup != NULL );	

	POINT pp;
	GetCursorPos (&pp);
	SelectionMade = pPopup->TrackPopupMenu (
		TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
		pp.x, pp.y, this );

	pPopup->DestroyMenu ();

	switch ( SelectionMade )
	{
	case ID_COMPLETEDATA_LOCK:
		{			
			//	<--	TRUE는 LOCK을 거는 것임, 수정 시작된다는 이야기	-->	//
			ChangeState ( TRUE );
		}
		break;

	case ID_COMPLETEDATA_UNLOCK:
		{	
			//	<--	FALSE는 UNLOCK을 거는 것임, 수정 끝났다는 이야기	-->	//
			ChangeState ( FALSE );
		}
		break;

	case ID_COMPLETEDATA_DELETE:
		{
			if ( MessageBox ( "정말 삭제하시겠습니까", "질문", MB_YESNO|MB_ICONQUESTION ) == IDYES )
			{
				DeleteSelectItems();
			}
			return;
		}
		break;

	case ID_COMPLETEDATA_COPY:
		{
			ITEMIDLIST *pidBrowse;
			TCHAR pszPathname[MAX_PATH] = {0};

			CString strTargetDir;

			BROWSEINFO Brinfo;
			memset( &Brinfo, 0 , sizeof(Brinfo));
			Brinfo.pszDisplayName = pszPathname;
			Brinfo.lpszTitle = _T("폴더를 지정하세요");
			Brinfo.ulFlags = BIF_RETURNONLYFSDIRS;

			pidBrowse = ::SHBrowseForFolder(&Brinfo);
			if ( pidBrowse != NULL )
			{
				SHGetPathFromIDList(pidBrowse , pszPathname );		
				strTargetDir = pszPathname;
				strTargetDir += "\\";

				CopySelectItems ( GetSSM()->GetDataDirectory(), strTargetDir );
			}
		}
		break;

	default:
		return;		
	};
}

void	CSoundSourceManagerView::CopySelectItems ( const CString& strOriginDir, const CString& strTargetDir )
{
	POSITION	pos = m_ctrlSoundSourceTable.GetFirstSelectedItemPosition ();
	if ( !pos )	return;

	int nCount = 0;
	while ( pos )
	{        
		int nItem = m_ctrlSoundSourceTable.GetNextSelectedItem ( pos );
		SSoundSource* pRecord = (SSoundSource*)m_ctrlSoundSourceTable.GetItemData ( nItem );
		if ( !pRecord )
		{
			GASSERT ( 0 && "레코드가 존재하지 않습니다." );
			continue;
		}

		CString strSrcFile = strOriginDir + pRecord->FileName;
		CString strTarFile = strTargetDir + pRecord->FileName;

		if ( !CopyFile ( strSrcFile, strTarFile, TRUE ) )
		{
			DWORD dwError = GetLastError ();
			switch ( dwError )
			{
			case ERROR_FILE_EXISTS:
				CString strTryAgain;
				strTryAgain.Format ( "%s가 존재합니다. 덮어쓰시겠습니까?", strTarFile );
				if ( MessageBox ( strTryAgain, "질문", MB_YESNO|MB_ICONQUESTION ) == IDYES )
				{
					CopyFile ( strSrcFile, strTarFile, FALSE );
				}
				break;
			}
		}

		nCount++;
	} 

	CString strResult;
	strResult.Format ( "소스 : %s\n타겟 : %s\n"
		"%d개의 파일 복사에 성공하였습니다.", strOriginDir, strTargetDir, nCount );
	MessageBox ( strResult );
}

void	CSoundSourceManagerView::DeleteSelectItems()
{
	POSITION	pos = m_ctrlSoundSourceTable.GetFirstSelectedItemPosition ();

	if ( !pos )
	{
		return;
	}

	//	<--	이전 사운드 객체 죽이기	-->	//
	if( m_pSound )
	{
		m_pSound->Stop();
		m_pSound->Reset();		
		SAFE_DELETE( m_pSound );
	}

	int	TotalCount = 0;
	int CurCount = 0;
	while ( pos )
	{        
		int nItem = m_ctrlSoundSourceTable.GetNextSelectedItem ( pos );
		SSoundSource* pRecord = (SSoundSource*)m_ctrlSoundSourceTable.GetItemData ( nItem );
		if ( pRecord->State )
		{
			TotalCount++;
		}
	}

	if ( !TotalCount )
	{
		MessageBox ( "수정이 끝난 것은 삭제되지 않습니다." );
		return ;
	}

	int *pItems = new int [ TotalCount ];
	pos = m_ctrlSoundSourceTable.GetFirstSelectedItemPosition ();
	while ( pos )
	{
		int nItem = m_ctrlSoundSourceTable.GetNextSelectedItem ( pos );
		SSoundSource* pRecord = (SSoundSource*)m_ctrlSoundSourceTable.GetItemData ( nItem );
		if ( pRecord->State )
		{
			pItems[CurCount] = nItem;
			CurCount++;
		}
	}

	if ( TotalCount != CurCount )
	{
		MessageBox ( "오류임돠, 구간 삭제에 문제 있슴돠" );
		return ;
	}

    CurCount = TotalCount;
	while ( CurCount )
	{
		int nItem = pItems[CurCount - 1];
		SSoundSource* pRecord = (SSoundSource*)m_ctrlSoundSourceTable.GetItemData ( nItem );
		GetSSM()->DelRecord ( pRecord->ID );
		m_ctrlSoundSourceTable.DeleteItem ( nItem );

		CurCount--;
	}

	delete [] pItems;

	ReloadAllItems();
}

void	CSoundSourceManagerView::ChangeState( BOOL State )
{
	CSoundSourceManagerDoc *pDoc = (CSoundSourceManagerDoc *) GetDocument ();
	POSITION	pos = m_ctrlSoundSourceTable.GetFirstSelectedItemPosition ();
	
	while ( pos )	//	마우스가 레코드에 제대로 클릭된 경우만 처리한다.
	{
		int nItem = m_ctrlSoundSourceTable.GetNextSelectedItem ( pos );
		if ( nItem == -1 )
		{
			MessageBox ( "오류발생 ChangeState" );
			return ;
		}

		SSoundSource* pRecord = (SSoundSource*)m_ctrlSoundSourceTable.GetItemData ( nItem );
		pRecord->State = State;
	}

	ReloadAllItems();
}

void CSoundSourceManagerView::OnBnClickedButtonFind()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ReloadAllItems();
}

void	CSoundSourceManagerView::ReloadAllItems()
{
	m_ctrlSoundSourceTable.DeleteAllItems ();

	SSoundSource*	pReloadSource;
	CString			str;

	UpdateData ( TRUE );

	int nIndex = m_ctrlCategory.GetCurSel ();
	int	nCategory = (int)m_ctrlCategory.GetItemData ( nIndex );

    int Amount = 0;
	GetSSM()->GoTail ();
	for ( int i = 0; i < GetSSM()->GetCount(); i++ )
	{
		pReloadSource = GetSSM()->GetRecord ();

		if ( pReloadSource->TYPE == nCategory || nCategory == 7 )
		{
			if ( CompareAllToken ( pReloadSource->Comment, m_valKeyword ) )
			{
				str.Format ( "%d", pReloadSource->ID );
				int nItem = m_ctrlSoundSourceTable.InsertItem ( Amount, NULL, pReloadSource->State );		
				m_ctrlSoundSourceTable.SetItemText ( Amount, 1, str );	//	ID
				m_ctrlSoundSourceTable.SetItemText ( Amount, 2, NTypeDesc::Desc[pReloadSource->TYPE] );
				str.Format ( "%d", pReloadSource->BufferCount );
				m_ctrlSoundSourceTable.SetItemText ( Amount, 3, str );
				m_ctrlSoundSourceTable.SetItemText ( Amount, 4, pReloadSource->FileName );
				m_ctrlSoundSourceTable.SetItemText ( Amount, 5, pReloadSource->LastUpdated );
				m_ctrlSoundSourceTable.SetItemText ( Amount, 6, pReloadSource->Comment );

				m_ctrlSoundSourceTable.SetItemData ( nItem, (DWORD_PTR)pReloadSource );

				Amount++;
			}
		}

		GetSSM()->GoPrev ();
	}
}

BOOL CSoundSourceManagerView::CompareAllToken ( CString Left, CString Right )
{	
	char szRight[1024];
	int	LengthRight = 0;
	CString Token[32];
	int TokenCount = 0;
	
	strcpy ( szRight, Right.GetString () );
	LengthRight = Right.GetLength ();

	for ( int i = 0; i < LengthRight; i++ )
	{
		//	<--	첫번째부터 공백일때	-->	//
		if ( i == 0 && szRight[i] == ' ' )
		{			
		}
		//	<--	두번째부터 공백일때 -->	//
		else if ( szRight[i] == ' ' && szRight[i - 1] != ' ' )
		{			
			TokenCount++;			
		}
		else
		{
			if ( !Token[TokenCount].GetLength () )
			{
				Token[TokenCount] = szRight[i];
			}
			else
			{
				Token[TokenCount] += szRight[i];
			}
		}
	}

	BOOL	bResult = TRUE;
    for ( int j = 0; j <= TokenCount; j++ )
	{
		 if ( !strstr ( Left.GetString (), Token[j].GetString () ) )
		 {
			 bResult = FALSE;
		 }
	}

	return bResult;
}

void CSoundSourceManagerView::OnCbnSelchangeComboCategory()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_valKeyword = "";
	UpdateData ( FALSE );
	ReloadAllItems();
}

void CSoundSourceManagerView::OnMenuitemTextout()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString szFilter = "사운드 목록 텍스트 파일 (*.txt)|*.txt|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,".txt",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter);

//	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{

		FILE* fp = fopen ( dlg.GetFileName ().GetString(), "wt" );
		if ( !fp ) return ;

		CString strTemp;
		GetSSM ()->GoHead ();
		while ( !GetSSM()->IsEnd () )
		{
			SSoundSource* pSoundSource = GetSSM()->GetRecord();		
			fprintf ( fp, "%s | %-64.64s | %s\n",
				pSoundSource->FileName, pSoundSource->Comment,
				pSoundSource->LastUpdated );
			GetSSM()->GoNext ();
		}
		fclose ( fp );

		MessageBox ( "정상출력되었습니다.", "확인", MB_OK );
	}
}

void CSoundSourceManagerView::OnMenuitemLoad()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString szFilter = "웨이브 테이블 (*.sg)|*.sg|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".sg",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter);

//	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		if ( !m_pSoundSourceMan->LoadFile ( dlg.GetPathName().GetString() ) )
		{
			MessageBox ( "파일 읽기에 실패하였습니다.", "경고", MB_OK );
			return ;
		}
		CString strDirectory = dlg.GetPathName ();
		strDirectory = strDirectory.Left ( strDirectory.ReverseFind ( '\\' ) + 1 );
		m_pSoundSourceMan->SetDataDirectory ( strDirectory );
		
		ReloadAllItems();

		((CEdit*)GetDlgItem ( IDC_EDIT_SRCDIRECTORY ))->SetWindowText ( strDirectory );		
	}
}

void CSoundSourceManagerView::OnMenuitemSave()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString szFilter = "웨이브 테이블 (*.sg)|*.sg|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,".sg",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter);

//	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		if ( !m_pSoundSourceMan->SaveFile ( dlg.GetPathName().GetString() ) )
			MessageBox ( "파일 쓰기에 실패하였습니다.", "경고", MB_OK );
	}
}

void CSoundSourceManagerView::OnMenuitemIntegrity()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CWavFileIntegrity dlg;
	dlg.DoModal ();
}

void CSoundSourceManagerView::OnMenuitemFile2listTrace()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CFile2ListDialog dlg;
	dlg.DoModal ();
}

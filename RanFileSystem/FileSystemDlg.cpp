// FileSystemDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "FileSystem.h"
#include "FileSystemDlg.h"
#include "NewFolderDlg.h"
#include ".\filesystemdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// CFileSystemDlg 대화 상자

static int CALLBACK FileListItemSortFunc(LPARAM lParam1,LPARAM lParam2,LPARAM /*lParamSort*/)
{
	SFileHandle* pFH1 = (SFileHandle*)lParam1;
	SFileHandle* pFH2 = (SFileHandle*)lParam2;

	CString strItem1;
	if(pFH1->GetFileContext()->iType == FT_DIR)
	{
		strItem1.Format("!%s",pFH1->GetFileContext()->strName);
	}
	else
	{
		strItem1 = pFH1->GetFileContext()->strName;
	}

	CString strItem2;
	if(pFH2->GetFileContext()->iType == FT_DIR)
	{
		strItem2.Format("!%s",pFH2->GetFileContext()->strName);
	}
	else
	{
		strItem2 = pFH2->GetFileContext()->strName;
	}

	return strcmp(strItem1,strItem2);
}

void CFileSystemDlg::UpdateFileList()
{
	m_FileList.DeleteAllItems();

	int iCount = 0;
	CString strFileSize;
	SFileHandleList* pFHL = m_FileSystem.GetFileHandleList();
	for(SFileHandleList_i it=pFHL->begin();it!=pFHL->end();++it)
	{
		m_FileList.InsertItem(iCount,(*it)->GetFileContext()->strName,(*it)->GetFileContext()->iType);

		m_FileList.SetItemData(iCount,(DWORD)(*it));

		if((*it)->GetFileContext()->iType == FT_FILE)
		{
			strFileSize.Format("%dKB",((*it)->GetFileContext()->lCompressSize+999)/1000);
			m_FileList.SetItemText(iCount,1,LPCTSTR(strFileSize));
		}

		iCount++;
	}

	m_FileList.SortItems(FileListItemSortFunc,NULL);
}

void CFileSystemDlg::DropFile(HDROP hDrop)
{
	if(!m_FileList.GetItemCount())
	{
		AfxMessageBox("가상파일시스템이 생성되어 있지 않습니다.");
		return;
	}

	int iCount = DragQueryFile(hDrop,0xFFFFFFFF,NULL,0);

	TCHAR strPath[_MAX_PATH];
	for(int i=0;i<iCount;i++)
	{
		DragQueryFile(hDrop,i,strPath,_MAX_PATH);

		FILE* bFile = fopen(strPath,"rb");
		if(bFile) fclose(bFile);

		TCHAR strName[_MAX_FNAME];
		m_FileSystem.GetNameOnly(strName,strPath);

		if(bFile)
		{
			if(m_FileSystem.CheckNameExist(strName))
			{
				if(m_iOverwriteType < 0) m_iOverwriteType = AfxMessageBox("동일한 이름의 파일이 이미 존재합니다.\n\n모든 파일을 새로운 파일로 덮어쓰시려면 '예' 를\n\n모든 기존의 파일을 보존하시려면 '아니오' 를 눌러주십시오!   ",MB_YESNO);
				if(m_iOverwriteType == IDNO) continue;

				m_FileSystem.Remove(strName);
			}
			if(!m_FileSystem.AddFile(strPath))
			{
				CString strError;
				strError.Format("파일을 추가하는데 실패하였습니다.\n\n%s",strPath);
				AfxMessageBox(strError);
			}
		}
		else
		{
			if(!DropFolder(strPath))
			{
				CString strError;
				strError.Format("폴더를 추가하는데 실패하였습니다.\n\n%s",strPath);
				AfxMessageBox(strError);
			}
		}
	}

	m_iOverwriteType = -1;

	UpdateFileList();

	DragFinish(hDrop);
}

BOOL CFileSystemDlg::DropFolder(const TCHAR* strPath)
{
	TCHAR strOldPath[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH,strOldPath);
	SetCurrentDirectory(strPath);

	TCHAR strName[_MAX_FNAME];
	m_FileSystem.GetNameOnly(strName,strPath);

	if(!m_FileSystem.CheckNameExist(strName))
	{
		m_FileSystem.AddDir(strName);
	}
	if(!m_FileSystem.ChangeDir(strName)) return FALSE;

	CFileFind FileFind;
	if(FileFind.FindFile())
	{
		for(BOOL bNext=TRUE;bNext;)
		{
			bNext = FileFind.FindNextFile();
			CString strP = FileFind.GetFilePath();
			CString strN = FileFind.GetFileName();
			if(strN == "." || strN == "..") continue;

			if(FileFind.IsDirectory())
			{
				DropFolder(LPCTSTR(strP));
			}
			else
			{
				if(m_FileSystem.CheckNameExist(strN))
				{
					if(m_iOverwriteType < 0) m_iOverwriteType = AfxMessageBox("동일한 이름의 파일이 이미 존재합니다.\n\n모든 파일을 새로운 파일로 덮어쓰시려면 '예' 를\n\n모든 기존의 파일을 보존하시려면 '아니오' 를 눌러주십시오!",MB_YESNO);
					if(m_iOverwriteType == IDNO) continue;

					m_FileSystem.Remove(strN);
				}
				if(!m_FileSystem.AddFile(strP))
				{
					CString strError;
					strError.Format("파일을 추가하는데 실패하였습니다.\n\n%s",strPath);
					AfxMessageBox(strError);
				}
			}
		}
	}

	FileFind.Close();

	m_FileSystem.ChangeDir("..");

	SetCurrentDirectory(strOldPath);

	return TRUE;
}

CFileSystemDlg::CFileSystemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileSystemDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_iOverwriteType = -1;
}

void CFileSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILELIST, m_FileList);
}

BEGIN_MESSAGE_MAP(CFileSystemDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(IDM_FILE_NEW, OnFileNew)
	ON_COMMAND(IDM_FILE_OPEN, OnFileOpen)
	ON_COMMAND(IDM_FILE_CLOSE, OnFileClose)
	ON_BN_CLICKED(IDC_NEWFOLDER, OnBnClickedNewfolder)
	ON_BN_CLICKED(IDC_INSERTFILE, OnBnClickedInsertfile)
	ON_BN_CLICKED(IDC_REMOVE, OnBnClickedRemove)
	ON_BN_CLICKED(IDC_OPTIMIZE, OnBnClickedOptimize)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_FILELIST, OnLvnEndlabeleditFilelist)
	ON_NOTIFY(NM_DBLCLK, IDC_FILELIST, OnNMDblclkFilelist)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnBnClickedButtonOpen)
END_MESSAGE_MAP()

// CFileSystemDlg 메시지 처리기

BOOL CFileSystemDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_FileList.DragAcceptFiles();
	m_FileList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	RECT Rect;
	m_FileList.GetWindowRect(&Rect);

	const int COLUMN_WIDTH_GAB = 4;
	const int SIZECOLUMN_WIDTH = 80;
	m_FileList.InsertColumn(0,"이름",LVCFMT_LEFT,(Rect.right-Rect.left)-(SIZECOLUMN_WIDTH+COLUMN_WIDTH_GAB));
	m_FileList.InsertColumn(1,"크기",LVCFMT_RIGHT,SIZECOLUMN_WIDTH);

	m_ImageList.Create(IDB_FILELISTICON,16,2,RGB(0,255,0));
	m_FileList.SetImageList(&m_ImageList,LVSIL_SMALL);

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CFileSystemDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CFileSystemDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CFileSystemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFileSystemDlg::OnFileNew()
{
	CFileDialog Dlg(FALSE,".rpf","filename.rpf",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"파일(*.rpf)|*.rpf||",NULL);
	if(Dlg.DoModal() == IDCANCEL) return;

	if(!m_FileSystem.NewFileSystem(LPCTSTR(Dlg.GetPathName()))) return;
	if(!m_FileSystem.OpenFileSystem(LPCTSTR(Dlg.GetPathName()))) return;

	m_strPath = Dlg.GetPathName();

	UpdateFileList();
}

void CFileSystemDlg::OnFileOpen()
{
	CFileDialog Dlg(TRUE,".rpf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"파일(*.rpf)|*.rpf||",NULL);
	if(Dlg.DoModal() == IDCANCEL) return;

	if(!m_FileSystem.OpenFileSystem(LPCTSTR(Dlg.GetPathName()))) return;

	m_strPath = Dlg.GetPathName();

	UpdateFileList();
}

void CFileSystemDlg::OnFileClose()
{
	m_FileSystem.CloseFileSystem();

	m_strPath.Empty();

	UpdateFileList();
}

void CFileSystemDlg::OnBnClickedNewfolder()
{
	if(!m_FileList.GetItemCount())
	{
		AfxMessageBox("가상파일시스템이 생성되어 있지 않습니다.");
		return;
	}

	CNewFolderDlg Dlg;
	if(Dlg.DoModal() == IDCANCEL) return;
	if(Dlg.m_strFolderName.IsEmpty()) return;

	m_FileSystem.AddDir(LPCTSTR(Dlg.m_strFolderName));

	UpdateFileList();
}

void CFileSystemDlg::OnBnClickedInsertfile()
{
	if(!m_FileList.GetItemCount())
	{
		AfxMessageBox("가상파일시스템이 생성되어 있지 않습니다.");
		return;
	}

	CFileDialog Dlg(TRUE);
	if(Dlg.DoModal() == IDCANCEL) return;

	TCHAR strName[_MAX_FNAME];
	m_FileSystem.GetNameOnly(strName,LPCTSTR(Dlg.GetPathName()));
	if(m_FileSystem.CheckNameExist(strName))
	{
		if(AfxMessageBox("동일한 이름의 파일이 이미 존재합니다.\n\n모든 파일을 새로운 파일로 덮어쓰시려면 '예' 를\n\n모든 기존의 파일을 보존하시려면 '아니오' 를 눌러주십시오!   ",MB_YESNO) == IDNO) return;

		m_FileSystem.Remove(strName);
	}

	if(!m_FileSystem.AddFile(LPCTSTR(Dlg.GetPathName())))
	{
		CString strError;
		strError.Format("파일을 추가하는데 실패하였습니다.\n\n%s",LPCTSTR(Dlg.GetPathName()));
		AfxMessageBox(strError);
	}

	UpdateFileList();
}

void CFileSystemDlg::OnBnClickedRemove()
{
	if(!m_FileList.GetItemCount())
	{
		AfxMessageBox("가상파일시스템이 생성되어 있지 않습니다.");
		return;
	}

	if(!m_FileList.GetSelectedCount()) return;

	if(AfxMessageBox("선택된 파일 혹은 폴더를 삭제하시겠습니까?", MB_YESNO) == IDYES)
	{
		CStringArray strRemove;

		POSITION Pos = m_FileList.GetFirstSelectedItemPosition();
		while(Pos)
		{
			int iItem = m_FileList.GetNextSelectedItem(Pos);
			SFileHandle* pFH = (SFileHandle*)m_FileList.GetItemData(iItem);

			strRemove.Add(pFH->GetFileContext()->strName);
		}

		for(int i=0;i<strRemove.GetSize();i++) m_FileSystem.Remove(strRemove[i]);

		UpdateFileList();
	}
}

void CFileSystemDlg::OnBnClickedOptimize()
{
	if(!m_FileList.GetItemCount())
	{
		AfxMessageBox("가상파일시스템이 생성되어 있지 않습니다.");
		return;
	}

	CString strPath = m_strPath;
	strPath = strPath.Left(strPath.ReverseFind('\\'));
	strPath = strPath + "\\Optimized.rpf";

	if(!m_FileSystem.OptimizeFileSystem(strPath))
	{
		AfxMessageBox("최적화작업에 실패하였습니다!");
		return;
	}

	m_FileSystem.CloseFileSystem();
	remove(m_strPath);
	rename(strPath,m_strPath);
	m_FileSystem.OpenFileSystem(m_strPath);

	UpdateFileList();

	AfxMessageBox("최적화작업을 성공적으로 수행하였습니다!");
}

void CFileSystemDlg::OnLvnEndlabeleditFilelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	if(!pDispInfo->item.pszText) return;

	SFileHandle* pFH = (SFileHandle*)m_FileList.GetItemData(pDispInfo->item.iItem);
	if(!m_FileSystem.Rename(pFH->GetFileContext()->strName,pDispInfo->item.pszText))
	{
		AfxMessageBox("선택된 파일 혹은 폴더의 이름을 변경할수 없습니다!");
		return;
	}

	m_FileList.SetItemText(pDispInfo->item.iItem,0,pDispInfo->item.pszText);

	*pResult = 0;
}

void CFileSystemDlg::OnNMDblclkFilelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(m_FileList.GetSelectedCount() != 1) return;

	NMITEMACTIVATE* pNMItemActivate = (NMITEMACTIVATE*)pNMHDR;
	if(pNMItemActivate->iItem >= 0)
	{
		SFileHandle* pFH = (SFileHandle*)m_FileList.GetItemData(pNMItemActivate->iItem);

		if(pFH->GetFileContext()->iType == FT_DIR)
		{
			m_FileSystem.ChangeDir(pFH->GetFileContext()->strName);

			UpdateFileList();
		}
	}

	*pResult = 0;
}

BOOL CFileSystemDlg::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message)
	{
	case WM_DROPFILES:
		if(pMsg->hwnd == m_FileList.GetSafeHwnd()) DropFile((HDROP)pMsg->wParam);
		break;
	case WM_KEYDOWN:
		if(pMsg->hwnd == m_FileList.GetSafeHwnd() && pMsg->wParam == VK_DELETE) OnBnClickedRemove();
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CFileSystemDlg::OnBnClickedButtonOpen()
{
	if(!m_FileList.GetItemCount())
	{
		AfxMessageBox("가상파일시스템이 생성되어 있지 않습니다.");
		return;
	}

	if(!m_FileList.GetSelectedCount()) return;

	CStringArray strOpenFile;

	POSITION Pos = m_FileList.GetFirstSelectedItemPosition();
	while(Pos)
	{
		int iItem = m_FileList.GetNextSelectedItem(Pos);
		SFileHandle* pFH = (SFileHandle*)m_FileList.GetItemData(iItem);

		strOpenFile.Add(pFH->GetFileContext()->strName);
	}

	for(int i=0;i<strOpenFile.GetSize();i++) 
	{
		SFileHandle * pFileHandle = m_FileSystem.OpenFile(strOpenFile[i]);
		if( pFileHandle )
		{
			TCHAR szFileName[256] = {0};
			m_FileSystem.GetNameOnly( szFileName, strOpenFile[i] );

			CFile cFile;
			cFile.Open( szFileName, CFile::modeCreate | CFile::modeWrite );

			BYTE buffer[4096];
			DWORD dwRead;

			do
			{
				dwRead = pFileHandle->Read( buffer, 4096 );
				cFile.Write(buffer, dwRead);
			}
			while(dwRead > 0);

			cFile.Close();

			m_FileSystem.CloseFile( pFileHandle );
		}
	}

	UpdateFileList();
}
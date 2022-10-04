// ErrorReportDlg.cpp : 구현 파일
//

#include "stdafx.h"

#include <shlwapi.h>

#include "ErrorReport.h"
#include "ErrorReportDlg.h"
#include ".\errorreportdlg.h"
#include ".\export\BaseProcessor.h"
#include ".\export\MapProcessor.h"
#include ".\export\PdbProcessor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const TCHAR szLOGFILE[MAX_PATH] = { "errorlog.xml" };


// CErrorReportDlg 대화 상자



CErrorReportDlg::CErrorReportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CErrorReportDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CErrorReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REPORT_TREE, m_ctrTree);
	DDX_Control(pDX, IDC_FOLDER_EDIT2, m_txtMapPdbFolder);
}

BEGIN_MESSAGE_MAP(CErrorReportDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CErrorReportDlg 메시지 처리기

BOOL CErrorReportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	ClearTree();
	m_rxFunctionName.assign("([0-9A-Za-z_:]+)\\(.*\\)");

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CErrorReportDlg::OnPaint() 
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
HCURSOR CErrorReportDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CErrorReportDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strFolderPath;
	
	if ( OpenFolderDialog( strFolderPath ) )
	{
		SetDlgItemText( IDC_FOLDER_EDIT, strFolderPath );
		MessageBox( strFolderPath );
	}
	
}

void CErrorReportDlg::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strFolderPath;
	
	if ( OpenFolderDialog( strFolderPath ) )
	{
		SetDlgItemText( IDC_FOLDER_EDIT2, strFolderPath );
		MessageBox( strFolderPath );
	}
}

BOOL CErrorReportDlg::OpenFolderDialog( CString& strFolderPath )
{
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = this->m_hWnd; // 현재 핸들
	bi.lpszTitle = "폴더 선택"; // 폴더선택창 열었을때 보여지는 문구
	bi.pidlRoot = NULL;
	bi.lpfn = BrowseCallbackProc; // 콜백함수
	bi.lParam = 0;
	bi.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_NONEWFOLDERBUTTON | BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;

	LPITEMIDLIST pidlFolder = SHBrowseForFolder(&bi);
	
	if (pidlFolder == NULL) return FALSE;

	TCHAR szPath[_MAX_PATH] = {0};
	SHGetPathFromIDList(pidlFolder, szPath);

	strFolderPath = szPath;

	return TRUE;
}

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM dwData)
{
	switch (uMsg)
	{
	case BFFM_SELCHANGED:
		{
			TCHAR szText[_MAX_PATH] = {0};
			SHGetPathFromIDList(reinterpret_cast<LPITEMIDLIST>(lParam), szText);
			SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, reinterpret_cast<LPARAM>(szText));
		}
		break;
	}
	return 0;
}




void CErrorReportDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strDumpPath;
	GetDlgItemText( IDC_FOLDER_EDIT , strDumpPath );

	if (GetFileAttributes(strDumpPath) == INVALID_FILE_ATTRIBUTES)
	{
		MessageBox( "DumpFile Folder Path Error" );
		return;
	}

	CString strMapPdbFolder;
	GetDlgItemText( IDC_FOLDER_EDIT2 , strMapPdbFolder );
	if (GetFileAttributes(strMapPdbFolder) == INVALID_FILE_ATTRIBUTES)
	{
		MessageBox( "Map//Pdb Folder Path Error" );
		return;
	}

	// 통계내기
	ReportResearch( strDumpPath, strMapPdbFolder );

	MessageBox ( "ReportResearch Complate","", MB_OK );

}

void CErrorReportDlg::ReportResearch( CString& strDumpFilePath, CString& strMabPdpPath )
{
	CString strLogFile;
	CString strSearchFolder;
	CString strDumpFolder;
	CFileFind finder;
	
	BOOL bFind ( FALSE );

	strSearchFolder.Format( "%s\\*.*", strDumpFilePath );
	bFind = finder.FindFile( strSearchFolder );

	if ( !bFind )
	{
		MessageBox( "Can't Open DumpFile Folder" );
	}

	ClearTree();

	while( bFind )
	{
		bFind = finder.FindNextFile();

		strDumpFolder = finder.GetFileName();

		if ( strDumpFolder == "." || strDumpFolder == ".." )	continue;

//		MessageBox( strDumpFolder );

		if ( finder.IsDirectory() )
		{
			strLogFile.Format( "%s\\%s\\%s", strDumpFilePath, finder.GetFileName(), szLOGFILE );
			
//			MessageBox( strLogFile );
			if ( LoadXmlDocument( strLogFile ) )
			{
				StackTraceList();
			}
		}
	}

}
void CErrorReportDlg::ClearTree()
{
	m_ctrTree.DeleteAllItems ();
	m_TreeRoot = m_ctrTree.InsertItem ( "Root Node" );
}

BOOL CErrorReportDlg::LoadXmlDocument( CString& strLogFile )
{
	m_XmlDoc.Clear();
	m_pXMLNodeProcess = NULL;
	m_pPdbProcessor.reset();
	m_mapModules.clear();
	
	TiXmlNode* TextNode = NULL;
	TiXmlElement* nodeElem;	

	TCHAR szVersion[MAX_PATH]={0};
	TCHAR szPlatform[MAX_PATH]={0};
	

	if ( !m_XmlDoc.LoadFile(strLogFile) )
	{
		MessageBox("XML File Load Error");        
		return FALSE;
	}
	
	// Version 확인
	nodeElem = m_XmlDoc.FirstChildElement("report");
	if ( !nodeElem )
	{
		MessageBox("Not Read Node report version");
		return FALSE;
	}

	_tcscpy(szVersion,nodeElem->Attribute("version"));
	if ( _tcscmp(szVersion, _T("1")) )
	{
		MessageBox("Version Error", szVersion );
		return FALSE;
	}
	
	// 플랫폼 확인
	nodeElem = GetChildElement( nodeElem, CString("platform") );
	if ( !nodeElem )
	{
		MessageBox("Not Read Node platform");
		return FALSE;
	}

	_tcscpy(szPlatform,nodeElem->FirstChild()->Value());
	if ( _tcscmp( szPlatform,_T("Win32") ) )
	{
		MessageBox( "Platform Error", szPlatform );
		return FALSE;
	}

	if ( ( m_pXMLNodeProcess = GetChildElement ( m_XmlDoc.RootElement(), CString("error"))) )
	{
		CString strProcessID;
		GetXMLNodeText(m_pXMLNodeProcess, CString("process/id"), strProcessID);
		
		if (! strProcessID.IsEmpty())
		{
			CString strExpression;
			strExpression.Format(_T("processes/process[id=%s]"), strProcessID);

			m_pXMLNodeProcess = GetChildElement ( m_XmlDoc.RootElement(), strExpression );
		}
	}
    
	return TRUE;
}

void CErrorReportDlg::StackTraceList()
{
	TiXmlElement *rootElem = NULL, *nodeElem = NULL;
	CString strElem = "threads/thread[status=interrupted]/stack/frame";
	
	rootElem = m_XmlDoc.RootElement();
	
	nodeElem = GetChildElement( rootElem, strElem );
	if ( !nodeElem ) return;

	CStackEntry StackEntry;

	CString strExe;

	m_stackList.clear();

	while ( nodeElem )
	{
		GetStackEntry ( nodeElem, StackEntry );		
		strExe = StackEntry.m_strModule.Right( 3 );

		if ( strExe == "exe" )		m_stackList.push_front( StackEntry );

/*		
		CString Temp;
		Temp = StackEntry.m_strAddress;
		Temp += "\n";
		Temp += StackEntry.m_strFunctionInfo;
		Temp += "\n";
		Temp += StackEntry.m_strSourceFile;
		Temp += "\n";
		Temp += StackEntry.m_strLineInfo;
		Temp += "\n";
		Temp += StackEntry.m_strModule;

		MessageBox( Temp, "", MB_OK );
*/
		

		nodeElem = nodeElem->NextSiblingElement( "frame" );		
	}

	AddTree();
}

void CErrorReportDlg::AddTree()
{
	if ( m_stackList.empty() )	return;

	std::list<CStackEntry>::iterator iter = m_stackList.begin();
	CStackEntry& sStackEntry = (CStackEntry)*iter;

	CString strExeName;

	int nFind = sStackEntry.m_strModule.ReverseFind( _T('\\') );
    strExeName = sStackEntry.m_strModule.Right( sStackEntry.m_strModule.GetLength() - (nFind+1) );	
//	MessageBox ( strExeName, "", MB_OK );

	HTREEITEM hParentItem = GetTreeChildItem( m_TreeRoot, strExeName );
	HTREEITEM hChildItem = NULL;

	CString strItemText;
	DWORD	nCnt;
	
	if ( hParentItem != NULL ) 
	{
		nCnt = m_ctrTree.GetItemData( hParentItem );
		nCnt++;
		strItemText.Format( "%s(%d)", strExeName , nCnt );
		m_ctrTree.SetItemText( hParentItem, strItemText );
		m_ctrTree.SetItemData( hParentItem, nCnt );
	}	
	else
	{
		strItemText.Format( "%s(%d)", strExeName , 1 );
		hParentItem = m_ctrTree.InsertItem ( strItemText, m_TreeRoot );		
		nCnt = 1;
		m_ctrTree.SetItemData( hParentItem, nCnt );
	}

	for ( ; iter != m_stackList.end(); iter++ )
	{
		sStackEntry = (CStackEntry)*iter;

		if ( sStackEntry.m_strFunctionName.IsEmpty() ) continue;

		hChildItem = GetTreeChildItem( hParentItem, sStackEntry.m_strFunctionName );
		if ( hChildItem ) 
		{
			// 횟수 추가
			nCnt = m_ctrTree.GetItemData( hChildItem );
			nCnt++;
			strItemText.Format( "%s(%d)", sStackEntry.m_strFunctionName , nCnt );
			m_ctrTree.SetItemText( hChildItem, strItemText );
			m_ctrTree.SetItemData( hChildItem, nCnt );
		}
		else
		{
			strItemText.Format( "%s(%d)", sStackEntry.m_strFunctionName , 1 );
			hChildItem = m_ctrTree.InsertItem ( strItemText, hParentItem );
			nCnt = 1;
			m_ctrTree.SetItemData( hChildItem, nCnt );
		}

		hParentItem = hChildItem;
	}
}

HTREEITEM CErrorReportDlg::GetTreeChildItem( HTREEITEM hParentItem, CString& strItem )
{
	if ( !hParentItem ) return NULL;

	HTREEITEM hChildItem = NULL;
	CString strItemText, strItemTextNew;

	hChildItem = m_ctrTree.GetChildItem( hParentItem );

	while ( hChildItem ) 
	{
		
        strItemText = m_ctrTree.GetItemText( hChildItem );		
		int nFind = strItemText.ReverseFind( _T('(') );

		if ( nFind > 0 ) strItemTextNew = strItemText.Left( nFind );
		else  strItemTextNew = strItemText;


		if ( strItemTextNew == strItem ) return hChildItem;
		
		hChildItem = m_ctrTree.GetNextItem(hChildItem, TVGN_NEXT);
	}

	return NULL;
}

void CErrorReportDlg::GetStackEntry( TiXmlElement* rootElem, CStackEntry& rStackEntry )
{
	TiXmlElement* nodeElem = NULL ;
	CString strPath;
	
	strPath = "module";
	GetXMLNodeText( rootElem, strPath, rStackEntry.m_strModule );

	strPath = "address";
	GetXMLNodeText( rootElem, strPath, rStackEntry.m_strAddress );

	strPath = "function/name";
	GetXMLNodeText( rootElem, strPath, rStackEntry.m_strFunctionName );

	strPath = "function/offset";
	GetXMLNodeText( rootElem, strPath, rStackEntry.m_strFunctionOffset );

	strPath = "file";
	GetXMLNodeText( rootElem, strPath, rStackEntry.m_strSourceFile );

	strPath = "line/number";
	GetXMLNodeText( rootElem, strPath, rStackEntry.m_strLineNumber );

	strPath = "line/offset";
	GetXMLNodeText( rootElem, strPath, rStackEntry.m_strLineOffset );

	UpdateStackEntryFromMap( rStackEntry );
}

void CErrorReportDlg::UpdateStackEntryFromMap(CStackEntry& rStackEntry)
{
	if (! rStackEntry.m_strAddress.IsEmpty() &&
		(
			rStackEntry.m_strFunctionName.IsEmpty() ||
			rStackEntry.m_strSourceFile.IsEmpty() ||
			rStackEntry.m_strLineNumber.IsEmpty()
		))
	{
		int nAddrPos = rStackEntry.m_strAddress.Find(_T(':'));
		if (nAddrPos >= 0)
			++nAddrPos;
		else
			nAddrPos = 0;
		PVOID ptrAddress = (PVOID)_tcstoui64((PCTSTR)rStackEntry.m_strAddress + nAddrPos, NULL, 16);
		if (ptrAddress != NULL)
		{
			boost::shared_ptr<CBaseProcessor> pBaseProcessor(GetModuleInfo(rStackEntry.m_strModule));
			if (pBaseProcessor.get() != NULL)
			{
				boost::shared_ptr<CBaseFnInfo> pFnInfo;
				DWORD64 dwDisplacement64;
				if (pBaseProcessor->FindFunctionInfo(ptrAddress, pFnInfo, dwDisplacement64))
				{
					std::string strFunctionName(pFnInfo->GetName());
					boost::match_results<std::string::const_iterator> what;
					if (boost::regex_search(strFunctionName, what, m_rxFunctionName))
						rStackEntry.m_strFunctionName = CA2CT(what[1].str().c_str());
					else
						rStackEntry.m_strFunctionName = CA2CT(strFunctionName.c_str());
					if (dwDisplacement64 != 0)
						rStackEntry.m_strFunctionOffset.Format(_T("%I64u"), dwDisplacement64);
					else
						rStackEntry.m_strFunctionOffset.Empty();
				}
				boost::shared_ptr<CBaseFileInfo> pFileInfo;
				boost::shared_ptr<CBaseLineInfo> pLineInfo;
				DWORD dwDisplacement32;
				if (pBaseProcessor->FindLineInfo(ptrAddress, pFileInfo, pLineInfo, dwDisplacement32))
				{
					rStackEntry.m_strSourceFile = CA2CT(pFileInfo->GetFileName().c_str());
					rStackEntry.m_strLineNumber.Format(_T("%u"), pLineInfo->GetNumber());
					if (dwDisplacement32 != 0)
						rStackEntry.m_strLineOffset.Format(_T("%I32u"), dwDisplacement32);
					else
						rStackEntry.m_strLineOffset.Empty();
				}
			}
		}
	}
}


boost::shared_ptr<CBaseProcessor> CErrorReportDlg::GetModuleInfo(const CString& strModule)
{
	boost::shared_ptr<CBaseProcessor> pBaseProcessor;
	if (strModule.IsEmpty())
		return pBaseProcessor;
	CModuleMap::iterator itModule = m_mapModules.find(strModule);
	if (itModule == m_mapModules.end())
	{
		if (m_pXMLNodeProcess != NULL)
		{
			CString strExpression;
			strExpression.Format(_T("modules/module[name=%s]"), strModule);
			TiXmlElement* pXMLNodeModule;
			
			if ( (pXMLNodeModule = GetChildElement( m_pXMLNodeProcess , strExpression )) )
			{
				CString strBaseAddress;
				GetXMLNodeText(pXMLNodeModule, CString("base"), strBaseAddress);
				PVOID ptrBaseAddress = (PVOID)_tcstoui64(strBaseAddress, NULL, 0);
				CString strModuleSize;
				GetXMLNodeText(pXMLNodeModule, CString("size"), strModuleSize);
				DWORD dwModuleSize = _tcstoul(strModuleSize, NULL, 0);
				if (ptrBaseAddress != NULL && dwModuleSize != 0)
				{
					TCHAR szModuleName[MAX_PATH] = "";
					_tcscpy(szModuleName, PathFindFileName(strModule));
					TCHAR szMapPdbFolder[MAX_PATH];
					m_txtMapPdbFolder.GetWindowText(szMapPdbFolder, countof(szMapPdbFolder));
					TCHAR szSystemFolder[MAX_PATH] = "";
					GetSystemDirectory(szSystemFolder, countof(szSystemFolder));
					TCHAR szMapPdbFile[MAX_PATH];
					if (! FindFileByPattern(szMapPdbFolder, szModuleName, _T(".map"), szMapPdbFile) &&
						! FindFileByPattern(szMapPdbFolder, szModuleName, _T(".pdb"), szMapPdbFile) &&
						! FindFileByPattern(szSystemFolder, szModuleName, _T(".map"), szMapPdbFile) &&
						! FindFileByPattern(szSystemFolder, szModuleName, _T(".pdb"), szMapPdbFile))
					{
						return pBaseProcessor;
					}
					CBaseProcessor::PROCESSED_FILE_TYPE eProcessedFileType = CBaseProcessor::GetFileType(szMapPdbFile);
					switch (eProcessedFileType)
					{
					case CBaseProcessor::PFT_MAP:
						{
							CMapProcessor* pMapProcessor = new CMapProcessor();
							pBaseProcessor.reset(pMapProcessor);
							pMapProcessor->LoadMapText(szMapPdbFile);
							pMapProcessor->SetBaseAddress(ptrBaseAddress);
						}
						break;
					case CBaseProcessor::PFT_PDB:
						{
							CPdbProcessor* pPdbProcessor = STATIC_DOWNCAST(CPdbProcessor, m_pPdbProcessor.get());
							if (! pPdbProcessor)
							{
								pPdbProcessor = new CPdbProcessor();
								m_pPdbProcessor.reset(pPdbProcessor);
							}
							pBaseProcessor = m_pPdbProcessor;
							pPdbProcessor->LoadModule(szMapPdbFile, ptrBaseAddress, dwModuleSize);
						}
						break;
					}
					if (pBaseProcessor.get() != NULL)
						m_mapModules.insert(CModuleMap::value_type(strModule, pBaseProcessor));
				}
			}
		}
	}
	else
		pBaseProcessor = itModule->second;
	return pBaseProcessor;
}

// Element 값을 가져온다.
// strChildPath 입력 방법
// Root를 제외한 경로 입력 child1/child2/child3
// 특정값을 가진 Child를 찾을경우 [] Command 사용
// child1/child2[stats=ok]/child3 ->child2 노드 중에 하위노드인 stats 값이 OK인 child2 노드를 선택
TiXmlElement* CErrorReportDlg::GetChildElement( TiXmlElement* rootElem, CString& strChildPath )
{
	CString resTokenChild;
	CString strCompare;
	CString strChild, strElem, strValue;

	
	int nPosChild = 0;
	int nPosFormat = 0;

	if ( !rootElem ) return NULL;
	
	TiXmlElement* nodeElem = rootElem; 
	TiXmlElement* nodeElemChild = rootElem; 	
	TiXmlElement* nodeElemChild2 = NULL; 	

	resTokenChild = strChildPath.Tokenize("/",nPosChild);

	while ( resTokenChild != "")
	{
		int nFind = resTokenChild.Find( _T("[") );
		if ( nFind > 0 )
		{
			nPosFormat = 0;
			strCompare = resTokenChild;
			strChild = strCompare.Tokenize("[=]",nPosFormat );
			strElem = strCompare.Tokenize("[=]",nPosFormat );
			strValue = strCompare.Tokenize("[=]",nPosFormat );

			nodeElemChild = nodeElem->FirstChildElement( strChild );

			while( nodeElemChild ) 
			{
				nodeElemChild2 = nodeElemChild->FirstChildElement( strElem );
				if ( !nodeElemChild2 ) return NULL;

				CString strChildValue = nodeElemChild2->FirstChild()->Value();
				if ( strChildValue == strValue ) break;
				
				nodeElemChild = nodeElemChild->NextSiblingElement( strChild );
			}
		}
		else
		{
            nodeElemChild = nodeElem->FirstChildElement( resTokenChild );
		}
		
		if ( !nodeElemChild ) return NULL;		
		
		resTokenChild= strChildPath.Tokenize("/",nPosChild);		
		nodeElem = nodeElemChild;
	};

	return nodeElem;
}

bool CErrorReportDlg::GetXMLNodeText(TiXmlElement* rootElem, CString& strChildPath, CString& strNodeText )
{
	TiXmlElement* nodeElem = NULL;
	TiXmlNode* nodeChild = NULL;

	nodeElem = GetChildElement( rootElem, strChildPath );
	if ( nodeElem ) 
	{
		nodeChild = nodeElem->FirstChild();
		if ( nodeChild ) strNodeText = nodeChild->Value();
		else strNodeText.Empty();
		return true;
	}

	return false;
	
}


bool CErrorReportDlg::FindFileByPattern(PCTSTR pszMapPdbFolder, PCTSTR pszModuleName, PCTSTR pszFileExt, PTSTR pszFileName)
{
	PathCombine(pszFileName, pszMapPdbFolder, pszModuleName);
	PathRenameExtension(pszFileName, pszFileExt);
	return (GetFileAttributes(pszFileName) != INVALID_FILE_ATTRIBUTES);
}
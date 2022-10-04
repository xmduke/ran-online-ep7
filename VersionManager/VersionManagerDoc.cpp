// VersionManagerDoc.cpp : CVersionManagerDoc 클래스의 구현
//

#include "stdafx.h"
#include "VersionManager.h"
#include "VersionManagerDoc.h"
#include "DlgCompress.h"
#include "DlgFTP.h"
#include "CMinFtp.h"
#include "CCfg.h"
#include "DlgFileCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVersionManagerDoc

IMPLEMENT_DYNCREATE(CVersionManagerDoc, CDocument)

BEGIN_MESSAGE_MAP(CVersionManagerDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
    ON_COMMAND(ID_FTP, OnFtp)
    ON_COMMAND(ID_CHECK, OnCheck)
    ON_COMMAND(ID_FILE_SAVE, OnFileSave)
END_MESSAGE_MAP()


// CVersionManagerDoc 생성/소멸

CVersionManagerDoc::CVersionManagerDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.	
}

CVersionManagerDoc::~CVersionManagerDoc()
{
}

BOOL CVersionManagerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 다시 초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CVersionManagerDoc serialization

void CVersionManagerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}


// CVersionManagerDoc 진단

#ifdef _DEBUG
void CVersionManagerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVersionManagerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CVersionManagerDoc 명령

void CVersionManagerDoc::GetAppPath()
{	
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if ( !strCommandLine.IsEmpty() )
	{
		DWORD dwFind = strCommandLine.ReverseFind ( '\\' );
		if ( dwFind != -1 )
		{
			m_strFullPath = strCommandLine.Left ( dwFind );
			
			if ( !m_strFullPath.IsEmpty() )
			if ( m_strFullPath.GetAt(0) == '"' )
				m_strFullPath = m_strFullPath.Right ( m_strFullPath.GetLength() - 1 );
		}
	}
}

void CVersionManagerDoc::OnFileOpen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.	
	char szFilters[]=
      "Ran Version List File (*.lst)|*.lst|All Files (*.*)|*.*||";

	CFileDialog DlgFile(TRUE, "lst", "*.lst", OFN_ENABLESIZING, szFilters, NULL);
	    
    GetAppPath();
    DlgFile.m_pOFN->lpstrInitialDir = m_strFullPath;
	
	if (DlgFile.DoModal() == IDOK ) 
	{
		POSITION pos = DlgFile.GetStartPosition();
		CString strPathName = DlgFile.GetNextPathName(pos);

		CMainFrame* pFrame = (CMainFrame *) AfxGetMainWnd();

        CDlgCompress DlgCompress(pFrame, strPathName);
		DlgCompress.DoModal();

        pFrame->m_pLeft->InitListData();
        pFrame->m_pRight->InitListData();
	}
}

void CVersionManagerDoc::OnFtp()
{
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
    TRACE("OnFtp \n");
    CDlgFTP DlgFtp;
    DlgFtp.DoModal();   
}

void CVersionManagerDoc::OnCheck()
{
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
    TRACE("OnCheck \n");
    CDlgFileCheck DlgCheck;
    DlgCheck.DoModal();
}

void CVersionManagerDoc::OnFileSave()
{
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
    TRACE("OnFileSave \n");    

    // DB 에서 전체 파일리스트를 가져온다    
    std::vector<FullFileList> vFullFileList; 
    COdbcManager::GetInstance()->GetAllFileList( vFullFileList );
    
    // 파일리스트를 저장한다.
    CFile tmpFile("filelist.bin", CFile::modeCreate | CFile::modeWrite);

    RanFileList sTemp;
    SecureZeroMemory( &sTemp, sizeof(RanFileList) );

    int nTotalCount = (int) vFullFileList.size(); // 전체 파일갯수
    
	// 첫번째 레코드는 파일리스트 버전이다.
#ifdef CHINAPARAM
    sTemp.nVersion = 2;
#else
	sTemp.nVersion = 1;
#endif
    tmpFile.Write(&sTemp, sizeof(RanFileList));

    // 두번째 레코드는 전체 파일갯수이다.
    sTemp.nVersion = nTotalCount;
    tmpFile.Write(&sTemp, sizeof(RanFileList));

    // 두번째 레코드 부터 마지막까지 파일경로와 파일명을 저장한다.    
    for (int i=0; i<(int) vFullFileList.size(); i++)
	{	
        SecureZeroMemory( &sTemp, sizeof(RanFileList) );
        sTemp.nVersion = (&vFullFileList[i])->nVersion;
		::StringCchCopy(
			sTemp.szFileName,
			FILENAME_SIZE,
			(&vFullFileList[i])->strFileName.GetString() );

        ::StringCchCopy(
			sTemp.szSubPath,
			SUBPATH_SIZE,
			(&vFullFileList[i])->strFolder.GetString() );
#ifdef CHINAPARAM
		::StringCchCopy(
			sTemp.szMD5,
			MD5_SIZE,
			(&vFullFileList[i])->strMD5.GetString() );
#endif
        tmpFile.Write(&sTemp, sizeof(RanFileList));
    }

    tmpFile.Close();

    // 파일리스트를 압축해서 등록한다.  
     
    CString strFullPath;
    CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

    if ( !strCommandLine.IsEmpty() )
    {
        DWORD dwFind = strCommandLine.ReverseFind ( '\\' );
        if ( dwFind != -1 )
        {
            strFullPath = strCommandLine.Left ( dwFind );

            if ( !strFullPath.IsEmpty() )
            if ( strFullPath.GetAt(0) == '"' )
            strFullPath = strFullPath.Right ( strFullPath.GetLength() - 1 );
        }
    }  

    CString ListPath;
    ListPath.Format("%s\\filelist.bin", strFullPath);

    CString TargetPath;
    TargetPath.Format("%s\\", CCfg::GetInstance()->m_szTargetPath);

    CMainFrame* pFrame = (CMainFrame *) AfxGetMainWnd();
    if (CAB_UTIL_MIN::MinMakeCab(ListPath.GetString(), TargetPath.GetString()) == TRUE)
    {
        pFrame->MessageBox("파일리스트 생성을 완료했습니다");           
    }
    else
    {
        pFrame->MessageBox("파일리스트 압축에 실패하였습니다");               
    }    
}
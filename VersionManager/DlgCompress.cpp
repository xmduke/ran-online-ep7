// DlgCompress.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include <process.h>
#include "VersionManager.h"
#include "DlgCompress.h"
#include "MainFrm.h"

#include "MIN_CAB_UTIL.h"
#include "CConsoleMessage.h"
#include "ListLoader.h"
#include "CCfg.h"
#include ".\dlgcompress.h"

// CDlgCompress 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgCompress, CDialog)
CDlgCompress::CDlgCompress(CMainFrame* pFrame, std::vector<CompressFile> &v, CWnd* pParent /*=NULL*/, int nMode)
	: CDialog(CDlgCompress::IDD, pParent)
{
	m_vCompressFile = v;
	m_pFrame		= pFrame;
	m_nMode			= nMode;
}

CDlgCompress::CDlgCompress(CMainFrame* pFrame, CString strFileName, CWnd* pParent, int nMode)   // 표준 생성자입니다.
	: CDialog(CDlgCompress::IDD, pParent)
{
	m_nMode			= nMode;
	m_pFrame		= pFrame;
	m_strFileName	= strFileName;
}

CDlgCompress::~CDlgCompress()
{
	CConsoleMessage::GetInstance()->ReleaseInstance();
}

void CDlgCompress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	
	DDX_Control(pDX, IDC_PROGRESS1, m_Pregress);
	DDX_Control(pDX, IDC_STATIC_PG, m_StaticCtl);
}


BEGIN_MESSAGE_MAP(CDlgCompress, CDialog)
	ON_WM_TIMER()
    ON_EN_MAXTEXT(IDC_EDIT_CONSOLE, OnEnMaxtextEditConsole)
END_MESSAGE_MAP()


// CDlgCompress 메시지 처리기입니다.

BOOL CDlgCompress::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CConsoleMessage::GetInstance()->SetControl(GetDlgItem(IDC_EDIT_CONSOLE)->m_hWnd);	
	m_Pregress.SetRange(0, 100);	
	m_Pregress.SetPos(0);

	if (m_nMode == 1) // 일반적 파일추가
	{
		SetTimer(100, 1000, NULL);
	}
	else if(m_nMode == 2)
	{
		SetTimer(200, 1000, NULL);
	}
	else
	{
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgCompress::OnTimer(UINT nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == 100)
	{
		KillTimer(100);
		threadCompressStart();
	}
	
	if (nIDEvent == 200)
	{
		KillTimer(200);
		threadCompressFromFileStart();
	}

	CDialog::OnTimer(nIDEvent);
}

void CDlgCompress::threadCompressStart()
{
	DWORD dwThreadId;

	HANDLE hThread = (HANDLE) ::_beginthreadex(
								NULL, 
								0, 
								CompressStart,
								this,
								0,
								(unsigned int*) &dwThreadId);
}

void CDlgCompress::threadCompressFromFileStart()
{
	DWORD dwThreadId;

	HANDLE hThread = (HANDLE) ::_beginthreadex(
								NULL, 
								0, 
								CompressFromFileStart,
								this,
								0,
								(unsigned int*) &dwThreadId);
}

unsigned int WINAPI CDlgCompress::CompressFromFileStart( void* pDlg )
{
	CDlgCompress* pDLG = (CDlgCompress*) pDlg;

	CListLoader ListLoader( pDLG->m_strFileName );

	// 디렉토리부터 생성하고 DB 에 입력한다.
	std::vector<FolderList>::iterator pos;
	int nTotalCount = (int) ListLoader.m_vFolderList.size();
    int nRetCode;
	DWORD dwRetCode;
	CString strFolder;

    CConsoleMessage::GetInstance()->Write("----폴더를 체크합니다----");

	//////////////////////////////////////////////////////////////////////////////
	// 루트디렉토리는 무조건 생성한다. "\\"
	strFolder = CCfg::GetInstance()->m_szTargetPath;
	CreateDirectory(strFolder, NULL);
	COdbcManager::GetInstance()->CreateFolder("\\");
	//////////////////////////////////////////////////////////////////////////////

	for (pos = ListLoader.m_vFolderList.begin(); pos<ListLoader.m_vFolderList.end(); ++pos)
	{		
		// DB 에 폴더가 있는지 체크한다.        
        nRetCode = COdbcManager::GetInstance()->IsFolderExist((*pos).strName);
        strFolder = CCfg::GetInstance()->m_szTargetPath + (*pos).strName;
        strFolder.Replace('/', '\\');

        if (nRetCode == TRUE)
        {
			// DB 에 있고 실제 존재한다면 스킵
            dwRetCode = GetFileAttributes(strFolder);
            if (dwRetCode == INVALID_FILE_ATTRIBUTES) // DB 에는 있지만 존재하지 않는 경우
            {
                // 디렉토리를 생성한다.
                if (CreateDirectory(strFolder, NULL) == 0)
		        {
			        // 생성실패
                    CConsoleMessage::GetInstance()->Write("%s 디렉토리가 생성되지 않았습니다", strFolder.GetString());			        
		        }
		        else
		        {
			        // 생성성공
		        }
            } 
            else // DB 에 입력되어 있고 실제로 존재하는 경우 
            {                                   
            }
        }
        else // DB 에 없는경우
        {
            // 디렉토리를 생성한다.
            if (CreateDirectory(strFolder, NULL) == 0)
		    {
			    // 생성실패
				CConsoleMessage::GetInstance()->Write("%s 디렉토리를 체크하십시오", strFolder.GetString());
		    }
		    else
		    {
			    // 생성성공
			    // DB 에 생성된 폴더 이름 입력
                CString strXXX = (*pos).strName;
                strXXX.Replace('/', '\\');
			    if (COdbcManager::GetInstance()->CreateFolder(strXXX) != DB_OK)
			    {
				    // 생성에는 성공 DB 에는 입력되지 않았음
                    CConsoleMessage::GetInstance()->Write("%s 디렉토리가 DB 에 입력되지 않았습니다.", strFolder.GetString());    			
			    }
                else
                {
                    // 성공
                }
		    }            
        }	
	}
    CConsoleMessage::GetInstance()->Write("----폴더 체크 완료----");
	// 파일을 하나씩 압축해서 해당 폴더로 옮긴다.
	std::vector<CompressFile>::iterator posFile;

    CConsoleMessage::GetInstance()->Write("----파일을 체크합니다----");
    
    // 디렉토리 고유번호를 넣어서 세팅한다.
	for (posFile = ListLoader.m_vFile.begin(); posFile<ListLoader.m_vFile.end(); ++posFile)
	{
		// 디렉토리의 고유번호를 구한후 값을 넣는다.
        int nDidx = COdbcManager::GetInstance()->GetFolderNum((*posFile).strPurePath);

        if (nDidx == DB_ERROR)
        {
            CConsoleMessage::GetInstance()->Write("%s 의 고유값을 찾지 못했습니다", (*posFile).strTarget.GetString());
        }
        else
        {
            (*posFile).nDir = nDidx;
            CConsoleMessage::GetInstance()->Write("%s 의 고유값은 %d 입니다", (*posFile).strTarget.GetString(), nDidx);
        }

        // 파일을 등록한다.
	}

    nTotalCount = (int) ListLoader.m_vFile.size();
    int nCount = 0;

    // 파일을 압축해서 넣는다.
    for (posFile = ListLoader.m_vFile.begin(); posFile<ListLoader.m_vFile.end(); ++posFile)
	{
		nCount++;
		CString strTemp;
        CompressFile sTemp;
        sTemp = (CompressFile) *posFile;
		strTemp.Format("%s %d/%d", sTemp.strFileName.GetString(), nCount, nTotalCount);
		pDLG->m_StaticCtl.SetWindowText(strTemp);
		sTemp.strSrc.GetString();
		// 압축해서 해당 디렉토리에 저장한다.
		if (CAB_UTIL_MIN::MinMakeCab(sTemp.strSrc.GetString(), sTemp.strTarget.GetString()) == TRUE)
		{ 
			// 같은 이름의 파일이 있는지 조사한다.
			BOOL bExist = COdbcManager::GetInstance()->IsFileExist(sTemp.strFileName);
			if (bExist)
			{
                // 같은 디렉토리의 같은 파일인지 조사한다.
                int nDirNum = COdbcManager::GetInstance()->GetFileFolderNum(sTemp.strFileName);
                
                // 같은 디렉토리의 같은 파일이면 업데이트 한다.
                if (nDirNum == sTemp.nDir) 
                {               
				    COdbcManager::GetInstance()->UpdateFile( sTemp.strFileName, sTemp.strMD5 );
                    COdbcManager::GetInstance()->UpdateFileStateFalse( sTemp.strFileName );
				    // CConsoleMessage::GetInstance()->Write("%s 기존 파일을 교체하였습니다", sTemp.strFileName.GetString());
                }                
                else // 다른 디렉토리의 같은 파일명이면 압축을 취소한다.
                {
                    CConsoleMessage::GetInstance()->Write("%s 파일은 이미 존재합니다", sTemp.strFileName.GetString());
                }
            }
			else
			{
				// 신규파일
				COdbcManager::GetInstance()->InsertFile( (*posFile).nDir, (*posFile).strFileName, (*posFile).strMD5 );
				// CConsoleMessage::GetInstance()->Write("%s 새로운 파일을 등록했습니다", sTemp.strFileName.GetString());
			}
		}
		else
		{
			CConsoleMessage::GetInstance()->Write("%s 파일 압축에 실패하였습니다", sTemp.strFileName.GetString());			
		}		
		
		pDLG->m_Pregress.SetPos((int)((nCount*100)/nTotalCount));
	}

    CConsoleMessage::GetInstance()->Write("----파일 체크 완료----");

	return 0;
}

unsigned int WINAPI CDlgCompress::CompressStart( void* pDlg )
{
	CDlgCompress* pDLG = (CDlgCompress*) pDlg;

	std::vector<CompressFile>::iterator pos;
	int nTotalCount = (int) pDLG->m_vCompressFile.size();
	int nCount = 0;

	for (pos = pDLG->m_vCompressFile.begin(); pos<pDLG->m_vCompressFile.end(); ++pos)
	{
		nCount++;
		CString strTemp;
        CompressFile sTemp;
        sTemp = (CompressFile) *pos;
		strTemp.Format("%s %d/%d", sTemp.strFileName.GetString(), nCount, nTotalCount);
		pDLG->m_StaticCtl.SetWindowText(strTemp);
        
		sTemp.strSrc.GetString();
		// 압축해서 해당 디렉토리에 저장한다.
		if (CAB_UTIL_MIN::MinMakeCab(sTemp.strSrc.GetString(), sTemp.strTarget.GetString()) == TRUE)
		{ 
			// 같은 이름의 파일이 있는지 조사한다.
			BOOL bExist = COdbcManager::GetInstance()->IsFileExist(sTemp.strFileName);
			if (bExist)
			{
				COdbcManager::GetInstance()->UpdateFile( sTemp.strFileName, sTemp.strMD5 );
                COdbcManager::GetInstance()->UpdateFileStateFalse(sTemp.strFileName); 
				CConsoleMessage::GetInstance()->Write("%s 기존 파일을 교체하였습니다", sTemp.strFileName.GetString());
			}
			else
			{
				COdbcManager::GetInstance()->InsertFile( sTemp.nDir, sTemp.strFileName, sTemp.strMD5 );
				CConsoleMessage::GetInstance()->Write("%s 새로운 파일을 등록했습니다", sTemp.strFileName.GetString());
			}
		}
		else
		{
			CConsoleMessage::GetInstance()->Write("%s 파일 압축에 실패하였습니다", sTemp.strFileName.GetString());			
		}
		pDLG->m_Pregress.SetPos((int)((nCount*100)/nTotalCount));
	}

	return 0;
}

void CDlgCompress::OnEnMaxtextEditConsole()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    GetDlgItem(IDC_EDIT_CONSOLE)->SetWindowText("");
}

// DlgFileCheck.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VersionManager.h"
#include <vector>
#include "DlgFileCheck.h"
#include "CConsoleMessage.h"
#include "CCfg.h"
#include "DatabaseTable.h"
#include "c_COdbcManager.h"
#include ".\dlgfilecheck.h"

// CDlgFileCheck 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgFileCheck, CDialog)
CDlgFileCheck::CDlgFileCheck(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFileCheck::IDD, pParent)
{
}

CDlgFileCheck::~CDlgFileCheck()
{
}

void CDlgFileCheck::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS1, m_ProgressCheck);
    DDX_Control(pDX, IDC_STATIC_PG, m_StaticCheck);
}


BEGIN_MESSAGE_MAP(CDlgFileCheck, CDialog)
    ON_WM_TIMER()
    ON_EN_MAXTEXT(IDC_EDIT_CHECK, OnEnMaxtextEditCheck)
END_MESSAGE_MAP()


// CDlgFileCheck 메시지 처리기입니다.

BOOL CDlgFileCheck::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    CConsoleMessage::GetInstance()->SetControl(GetDlgItem(IDC_EDIT_CHECK)->m_hWnd);
    m_ProgressCheck.SetRange(0, 100);	
	m_ProgressCheck.SetPos(0);
    SetTimer(100, 1000, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgFileCheck::StartFileCheck()
{
    // DB 에 입력된 파일과, 실제로 존재하는지 비교검사한다.
    CConsoleMessage::GetInstance()->Write("------- 파일체크를 시작합니다 ---------");

    // DB 에서 전체 파일리스트를 가져온다
    CConsoleMessage::GetInstance()->Write("전체 파일리스트를 가져옵니다");
    std::vector<FullFileList> vFullFileList; 
    COdbcManager::GetInstance()->GetAllFileList(vFullFileList);

    // 실제파일이 존재하는지 비교한다.
    CString strSrc(CCfg::GetInstance()->m_szTargetPath);

    int nTotalCount = (int) vFullFileList.size();
    int nCount = 0;
    int nErrorNum = 0;   

    CConsoleMessage::GetInstance()->Write("파일 비교를 시작합니다");

    for (int i=0; i<(int) vFullFileList.size(); i++)
	{	        
        nCount++;
        CString strFullPath;
        strFullPath = strSrc + (&vFullFileList[i])->strFolder + (&vFullFileList[i])->strFileName; 

        CString strTemp;        
		strTemp.Format("%s %d/%d", (&vFullFileList[i])->strFileName, nCount, nTotalCount);
		m_StaticCheck.SetWindowText(strTemp);

        if (IsExist(strFullPath)) // 존재한다면
        {
            
        }
        else // 존재하지 않는다면
        {
            nErrorNum++;
            CConsoleMessage::GetInstance()->Write("%d %s 파일이 존재하지 않습니다", nCount, strFullPath.GetString());
        }

        m_ProgressCheck.SetPos((int)((nCount*100)/nTotalCount));
    }

    CConsoleMessage::GetInstance()->Write("%d 개 파일오류를 발견했습니다", nErrorNum);
    CConsoleMessage::GetInstance()->Write("------- 파일체크가 완료되었습니다 ---------");
}

BOOL CDlgFileCheck::IsExist(CString strFullPath)
{
    DWORD dwReturn;
    dwReturn = GetFileAttributes(strFullPath);
    if (dwReturn == INVALID_FILE_ATTRIBUTES)
    {
        DWORD dwError = GetLastError();        
        return FALSE;
    }
    else if(dwReturn == FILE_ATTRIBUTE_DIRECTORY)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }

    
    //CFileFind find;
    //int ret = find.FindFile(m_strDesFile);
    //존재하면 nonzero, 존재하지 않으면 0를 리턴합니다.
}

void CDlgFileCheck::OnTimer(UINT nIDEvent)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    if (nIDEvent == 100)
    {
        KillTimer(100);
		StartFileCheck();
    }

    CDialog::OnTimer(nIDEvent);
}

void CDlgFileCheck::OnEnMaxtextEditCheck()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    GetDlgItem(IDC_EDIT_CHECK)->SetWindowText("");
}

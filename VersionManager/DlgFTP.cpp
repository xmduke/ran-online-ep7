// DlgFTP.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include <process.h>
#include "VersionManager.h"
#include "DlgFTP.h"
#include "CConsoleMessage.h"
#include "CCfg.h"
#include "CMinFtp.h"
#include <vector>
#include "DatabaseTable.h"
#include "c_COdbcManager.h"
#include ".\dlgftp.h"

// CDlgFTP 대화 상자입니다.

static unsigned int WINAPI UploadThread(void* p)
{
	CDlgFTP* pThread = (CDlgFTP*) p;
	pThread->UploadThreadProc();
	return 0;
}

IMPLEMENT_DYNAMIC(CDlgFTP, CDialog)
CDlgFTP::CDlgFTP(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFTP::IDD, pParent)
{
    m_bStop = FALSE;
}

CDlgFTP::~CDlgFTP()
{
}

void CDlgFTP::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS_FTP, m_ProgressFTP);
    DDX_Control(pDX, IDC_STATIC_FTP, m_StaticFTP);
}


BEGIN_MESSAGE_MAP(CDlgFTP, CDialog)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_STOP, OnBnClickedStop)
    ON_BN_CLICKED(IDC_START, OnBnClickedStart)
    ON_EN_MAXTEXT(IDC_EDIT_FTP, OnEnMaxtextEditFtp)
END_MESSAGE_MAP()


// CDlgFTP 메시지 처리기입니다.

BOOL CDlgFTP::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    CConsoleMessage::GetInstance()->SetControl(GetDlgItem(IDC_EDIT_FTP)->m_hWnd);	
	m_ProgressFTP.SetRange(0, 100);	
	m_ProgressFTP.SetPos(0);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgFTP::OnTimer(UINT nIDEvent)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    CDialog::OnTimer(nIDEvent);
}

int CDlgFTP::UploadThreadProc()
{
    // ftp 에 연결한다.	
    CConsoleMessage::GetInstance()->Write("---------- FTP Upload 를 시작합니다 ---------");
    CMinFtp* pFtp = new CMinFtp();

	int nFtpCount = static_cast <int> (CCfg::GetInstance()->m_vFtpIP.size());

	for (int nFtpCrt=1; nFtpCrt<=nFtpCount; nFtpCrt++)
	{
		CString strIP = CCfg::GetInstance()->m_vFtpIP[nFtpCrt-1];
		int nRetCode = pFtp->Connect(strIP.GetString(),
									CCfg::GetInstance()->m_nFtpPort,
									CCfg::GetInstance()->m_szFtpUserName,
									CCfg::GetInstance()->m_szFtpPassword);
		if (nRetCode == NET_ERROR)
		{
			CConsoleMessage::GetInstance()->Write("-------------------------------------------------------------------");
			CConsoleMessage::GetInstance()->Write("ERROR:%d %s FTP 연결에 실패하였습니다.", nFtpCrt, strIP.GetString());
			CConsoleMessage::GetInstance()->Write("-------------------------------------------------------------------");
			return -1;
		}

		// FTP 의 최초 접속 ROOT 디렉토리 위치
		CString strFirstDir;
		
		if (pFtp->m_pConnect->GetCurrentDirectory(strFirstDir) == 0)
		{
			CConsoleMessage::GetInstance()->Write("ERROR:GetCurrentDirectory %d", GetLastError());
			return -1;
		}

		// 전체 폴더리스트를 가져온다.
		std::vector<FolderList> vFolder;
		COdbcManager::GetInstance()->GetFolderList(vFolder);

		// FTP 에 폴더를 생성시킨다
		// 주의 : 아래와 같은 방식으로 하지 않으면...
		// Windows 계열과 Unix 계열에서 동시에 동작하지 않는다.

		CConsoleMessage::GetInstance()->Write("--FTP 에 폴더를 체크합니다--");
		for (int i=0; i<(int) vFolder.size(); i++)
		{	
			CString strTmpFolder= (&vFolder[i])->strName;
	        
			CString resToken;
			int curPos= 0;

			resToken= strTmpFolder.Tokenize("\\",curPos);
			while (resToken != "")
			{            
				pFtp->CreateDirectory(resToken);
				pFtp->SetCurrentDirectory(resToken);
				resToken= strTmpFolder.Tokenize("\\",curPos);
			}
			pFtp->SetCurrentDirectory(strFirstDir);        
		}   

		// 전체 파일리스트를 가져온다.
		CConsoleMessage::GetInstance()->Write("전체 파일리스트를 가져옵니다");
		std::vector<FullFileList> vFullFileList;    
		COdbcManager::GetInstance()->GetNotUploadedFileList(vFullFileList);
	    
		// 순회하면서 파일을 하나씩 업로드 한다.
		CConsoleMessage::GetInstance()->Write("파일을 업로드 합니다");
		CString strSrcPath;
		strSrcPath.Format("%s", CCfg::GetInstance()->m_szTargetPath);

		int nTotalCount = (int) vFullFileList.size();
		int nCount = 0;


		for (int i=0; i<(int) vFullFileList.size(); i++)
		{	
			nCount++;       

			CString strMakePath;
			CString strFileName = (&vFullFileList[i])->strFileName;
			strMakePath = strSrcPath + (&vFullFileList[i])->strFolder + strFileName;

			CString strTemp;        
			strTemp.Format("%s %d/%d", strFileName.GetString(), nCount, nTotalCount);
			m_StaticFTP.SetWindowText(strTemp);

			CString strChgfolder;
			strChgfolder = strFirstDir + (&vFullFileList[i])->strFolder;
			strChgfolder.Replace('\\', '/');
			pFtp->SetCurrentDirectory(strChgfolder);
			if (pFtp->PutFile(strMakePath, strFileName) == NET_ERROR)
			{
				CConsoleMessage::GetInstance()->Write("ERROR:%s 파일업로드 에러", strFileName.GetString());
				CConsoleMessage::GetInstance()->Write("INFO:%s 재시도", strFileName.GetString());
				if (pFtp->PutFile(strMakePath, strFileName) == NET_ERROR)
				{
					CConsoleMessage::GetInstance()->Write("ERROR:%s 파일업로드 재시도 실패", strFileName.GetString());					
				}
				else
				{
					CConsoleMessage::GetInstance()->Write("INFO:%s 파일업로드 재시도 성공", strFileName.GetString());
				}
			}
			else
			{
				// 업로드 성공
				// DB 에 업로드 했다고 기록한다.
				if (nFtpCrt == nFtpCount)
				{
					COdbcManager::GetInstance()->UpdateFileStateTrue((&vFullFileList[i])->nIdx);
				}
			}
			m_ProgressFTP.SetPos((int)((nCount*100)/nTotalCount));

			if (m_bStop)
			{
				CConsoleMessage::GetInstance()->Write("업로드를 중단합니다");
				pFtp->DisConnect();
				delete pFtp;
				return -1;
			}
			Sleep( 5 );
		}

		// 마지막으로 파일리스트를 업로드 한다.
		CConsoleMessage::GetInstance()->Write("파일리스트를 업로드 합니다");    
		if (pFtp->SetCurrentDirectory(strFirstDir) == NET_ERROR)
		{
			CConsoleMessage::GetInstance()->Write("ERROR:파일리스트 SetCurrentDirectory 실패");
		}
		else
		{
			if (pFtp->PutFile(strSrcPath + '\\' + "filelist.bin.cab", "filelist.bin.cab") == NET_ERROR)
			{
                CConsoleMessage::GetInstance()->Write("ERROR:파일리스트 업로드 실패");
			}
			else
			{
				CConsoleMessage::GetInstance()->Write("INFO:파일리스트 업로드 성공");
			}
		}
	   
		// ftp 연결을 해제한다.
		CConsoleMessage::GetInstance()->Write("FTP 연결을 해제합니다");
		pFtp->DisConnect();
	}
    delete pFtp;
	CConsoleMessage::GetInstance()->Write("---------- FTP Upload 를 완료했습니다 ---------");
    return 0;
}

void CDlgFTP::OnBnClickedStop()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_bStop = TRUE;
}

void CDlgFTP::OnBnClickedStart()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_bStop = FALSE;
    StartUploadThread();
}

int CDlgFTP::StartUploadThread()
{	
	DWORD dwThreadID = 101;	
	
	/*m_hThread = ::CreateThread(
						NULL, 
					    0, 
					    (LPTHREAD_START_ROUTINE) UploadThread, 
					    this, 
					    0, 
					    &dwThreadID );*/
	m_hThread = (HANDLE) ::_beginthreadex(
								NULL,
								0,
								UploadThread,
								this, 
								0, 
								(unsigned int*) &dwThreadID );
	if (m_hThread == NULL)
	{
		// 쓰래드 생성에 실패하였습니다.
        CConsoleMessage::GetInstance()->Write("쓰래드 생성에 실패하였습니다");
		return NET_ERROR;
	}
	else
	{
		return NET_OK;
	}
}

void CDlgFTP::OnEnMaxtextEditFtp()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    GetDlgItem(IDC_EDIT_FTP)->SetWindowText("");
}

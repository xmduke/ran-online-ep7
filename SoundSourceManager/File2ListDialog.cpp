// File2ListDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SoundSourceManager.h"
#include "File2ListDialog.h"
#include ".\file2listdialog.h"
#include "MainFrm.h"
#include "SoundSourceManagerView.h"
#include "SoundSourceMan.h"

// CFile2ListDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CFile2ListDialog, CDialog)
CFile2ListDialog::CFile2ListDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFile2ListDialog::IDD, pParent)
{
}

CFile2ListDialog::~CFile2ListDialog()
{
}

void CFile2ListDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFile2ListDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BEGIN, OnBnClickedButtonBegin)
	ON_BN_CLICKED(IDC_BUTTON_FILE, OnBnClickedButtonFile)
	ON_BN_CLICKED(IDC_BUTTON_END, OnBnClickedButtonEnd)
END_MESSAGE_MAP()


// CFile2ListDialog 메시지 처리기입니다.

void CFile2ListDialog::OnBnClickedButtonBegin()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame	*pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CSoundSourceManagerView *pView = (CSoundSourceManagerView *) pMainFrame->GetActiveView();	

	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LIST_LOG);

	while ( pListBox->GetCount () ) pListBox->DeleteString ( 0 );

	CString strLog;
	CString strFileFind = pView->GetSSM()->GetDataDirectory () + "*.wav";

	CFileFind filefind;
	if ( !filefind.FindFile ( strFileFind ) )
	{
		pListBox->AddString ( "파일이 존재하지 않습니다." );
		return ;
	}

	while ( filefind.FindNextFile () )
	{
		CString strFileName = filefind.GetFileName ();

        bool bFOUND = false;

		//
		pView->GetSSM ()->GoHead ();
		while ( !pView->GetSSM ()->IsEnd () )
		{
			SSoundSource* pRecord = pView->GetSSM()->GetRecord ();		
			if ( !pRecord )
			{			
				pListBox->AddString ( "레코드를 찾을 수 없습니다." );
				return ;
			}

			if ( strFileName == pRecord->FileName )
			{
				bFOUND = true;
				break;
			}

			pView->GetSSM ()->GoNext ();
		}

        if ( !bFOUND )
		{
			strLog.Format ( "[N]%s", strFileName );
			pListBox->AddString ( strLog );
		}
	}
}

void CFile2ListDialog::OnBnClickedButtonFile()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LIST_LOG);	

	FILE* fp = fopen ( "file2list.txt", "wt" );
	if ( fp )
	{
		char szItem[256];
		const int nCount = pListBox->GetCount ();
		for ( int i = 0; i < nCount; i++ )
		{
			pListBox->GetText ( i, szItem );
			fprintf ( fp, "%s\n", szItem );
		}

		fclose ( fp );
	}
}

void CFile2ListDialog::OnBnClickedButtonEnd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}

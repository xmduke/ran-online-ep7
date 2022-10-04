// NpcTalkDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "NpcAction.h"
#include "NpcTalkDialog.h"
#include "NpcTalk.h"
#include "NpcTalkControl.h"
#include "NpcDialogueSet.h"
#include "NpcDialogue.h"
#include "NpcDialogueCase.h"

// CNpcTalkDialog 대화 상자입니다.

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW


IMPLEMENT_DYNAMIC(CNpcTalkDialog, CDialog)
CNpcTalkDialog::CNpcTalkDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNpcTalkDialog::IDD, pParent)
{
	m_pTalkControl = NULL;
	m_pNpcDialogueSet = NULL;
	m_bModify = FALSE;
}

CNpcTalkDialog::~CNpcTalkDialog()
{
}

void CNpcTalkDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNpcTalkDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_CBN_SELCHANGE(IDC_COMBO_ACTION_TYPE, OnCbnSelchangeComboActionType)
	ON_CBN_SELCHANGE(IDC_COMBO_ACTION_DATA, OnCbnSelchangeComboActionData)
END_MESSAGE_MAP()


// CNpcTalkDialog 메시지 처리기입니다.

void CNpcTalkDialog::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnCancel ();
}

void CNpcTalkDialog::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	assert ( m_pTalkControl && "데이타 설정이 잘못되었습니다." );

	CString strTemp;

	GetDlgItem ( IDC_EDIT_TALKID )->GetWindowText ( strTemp );
	m_Talk.m_NativeID = (DWORD) atoi ( strTemp.GetBuffer () );

	GetDlgItem ( IDC_EDIT_TALK )->GetWindowText ( strTemp );
	m_Talk.m_szTalk = strTemp;

	if ( !strTemp.GetLength () )
	{
		MessageBox ( "설명부분이 비어있습니다.", "경고", MB_OK | MB_ICONASTERISK );
		return ;
	}

	if ( !m_bModify )
	{
		SNpcTalk* pNewTalk = new SNpcTalk;
		*pNewTalk = m_Talk;

		if ( m_pTalkControl->AddTalk ( pNewTalk ) )
		{
			OnOK ();
		}
		else
		{
			SAFE_DELETE ( pNewTalk );
			MessageBox ( "이미 등록된 ID가 존재합니다." );
		}
	}
	else
	{
		SNpcTalk* pTalk = m_pTalkControl->GetTalk ( m_Talk.m_NativeID );
		*pTalk = m_Talk;
		OnOK ();
	}
}

void CNpcTalkDialog::SetTalkData ( CNpcDialogueSet* pNpcDialogueSet, CNpcTalkControl* pTalkControl )
{
	assert ( pTalkControl && "널이 설정되고 있습니다." );
	assert ( pNpcDialogueSet && "널이 설정되고 있습니다." );

	m_pNpcDialogueSet = pNpcDialogueSet;
	m_pTalkControl = pTalkControl;
}

BOOL CNpcTalkDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
    CComboBox* pComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_TYPE );
	for ( int i = 0; i < SNpcTalk::EM_TALK_SIZE; i++ )
	{
		int nIndex = pComboBox->InsertString ( i, SNpcTalk::szActionType[i] );
		pComboBox->SetItemData ( nIndex, i );
	}

	if ( !m_bModify )
	{
		m_Talk.Init ();
		m_Talk.m_NativeID = m_pTalkControl->MakeNativeID ();

		CString strTemp;

		strTemp.Format ( "%d", m_Talk.m_NativeID );		 
		GetDlgItem ( IDC_EDIT_TALKID )->SetWindowText ( strTemp );

		pComboBox->SetCurSel ( 0 );
	}
	else
	{
		CString strTemp;

		strTemp.Format ( "%d", m_Talk.m_NativeID );		 
		GetDlgItem ( IDC_EDIT_TALKID )->SetWindowText ( strTemp );

		GetDlgItem ( IDC_EDIT_TALK )->SetWindowText ( m_Talk.m_szTalk );		

		pComboBox->SetCurSel ( m_Talk.m_emActionType );

		//	콤보박스도 자동으로 선택되어있도록 하기 위함임
		DWORD BackupActionID = m_Talk.m_ActionID;	//	OnCbnSelchangeComboActionType를 돌면서, m_Talk.m_ActionID의 값을 바꾸기 때문에..
		OnCbnSelchangeComboActionType ();
		CComboBox* pDataComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_DATA );
		m_Talk.m_ActionID = BackupActionID;
		pDataComboBox->SetCurSel ( m_Talk.m_ActionID );
	}
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CNpcTalkDialog::OnCbnSelchangeComboActionType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CComboBox* pDataComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_DATA );
	pDataComboBox->ResetContent ();

	CComboBox* pTypeComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_TYPE );
	int nIndex = pTypeComboBox->GetCurSel ();
	m_Talk.m_emActionType = (int)pTypeComboBox->GetItemData ( nIndex );
    
	switch ( nIndex )
	{
	case SNpcTalk::EM_DO_NOTHING:
		{
		}
		break;
	case SNpcTalk::EM_PAGE_MOVE:
		{
			CNpcDialogueSet::DIALOGMAP* pMap = m_pNpcDialogueSet->GetDialogueMap ();
			if ( pMap->size() )
			{
				CNpcDialogueSet::DIALOGMAP_IT iter = pMap->begin();
				CNpcDialogueSet::DIALOGMAP_IT iter_end = pMap->end();	
				for ( ; iter!=iter_end; ++iter )
				{
					CNpcDialogue* pDialogue = (*iter).second;
					CNpcDialogueCase* pPostiveCase = pDialogue->GetDialogueCase ( TRUE );

					CString strTemp;
					strTemp.Format ( "[%03d] %s", pDialogue->GetNativeID(), pPostiveCase->GetBasicTalk () );
					int nItem = pDataComboBox->AddString ( strTemp );
					pDataComboBox->SetItemData ( nItem, pDialogue->GetNativeID() );					
				}
				pDataComboBox->SetCurSel ( 0 );

				OnCbnSelchangeComboActionData ();
			}
		}
		break;
	case SNpcTalk::EM_BASIC:
		{
			CComboBox* pComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_DATA );
			for ( int i = 0; i < SNpcTalk::EM_BASIC_SIZE; i++ )
			{
				int nIndex = pComboBox->InsertString ( i, SNpcTalk::szBasicDesc[i] );
				pComboBox->SetItemData ( nIndex, i );
			}
			pDataComboBox->SetCurSel ( 0 );

			OnCbnSelchangeComboActionData ();
		}
		break;
	case SNpcTalk::EM_QUEST:
		{
		}
		break;
	}
}

void CNpcTalkDialog::OnCbnSelchangeComboActionData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CComboBox* pDataComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_DATA );
	int nIndex = pDataComboBox->GetCurSel ();
	m_Talk.m_ActionID = (DWORD)pDataComboBox->GetItemData ( nIndex );
}

void CNpcTalkDialog::SetModifyData ( BOOL bModify, SNpcTalk* pTalk )
{
	m_bModify = bModify;
	m_Talk = *pTalk;
}
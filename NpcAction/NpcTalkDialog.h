#pragma once

// CNpcTalkDialog 대화 상자입니다.
#include "NpcTalk.h"

class	CNpcTalkControl;
class	CNpcDialogueSet;

class CNpcTalkDialog : public CDialog
{
	DECLARE_DYNAMIC(CNpcTalkDialog)

public:
	CNpcTalkDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CNpcTalkDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NPCTALKDIALOG };

private:
	BOOL		m_bModify;
	SNpcTalk	m_Talk;

public:
	void	SetTalkData ( CNpcDialogueSet* pNpcDialogueSet, CNpcTalkControl* pTalkControl );
	void	SetModifyData ( BOOL bModify, SNpcTalk* pTalk );

private:
	CNpcTalkControl*	m_pTalkControl;
	CNpcDialogueSet*	m_pNpcDialogueSet;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboActionType();
	afx_msg void OnCbnSelchangeComboActionData();
};

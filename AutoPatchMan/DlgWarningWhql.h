#pragma once
#include "afxwin.h"

#include "HyperlinkStatic.h"

// CDlgWarningWhql 대화 상자입니다.
class CDlgWarningWhql : public CDialog
{
	DECLARE_DYNAMIC(CDlgWarningWhql)

public:
	BOOL m_bCheckWhql;

public:
	CDlgWarningWhql(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgWarningWhql();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_WARNING_WHQL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CHyperlinkStatic m_ctrlNVidiaLink;
	CHyperlinkStatic m_ctrlATILink;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};

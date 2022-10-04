#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CDlgNewFolder 대화 상자입니다.

class CDlgNewFolder : public CDialog
{
	DECLARE_DYNAMIC(CDlgNewFolder)

public:
	CDlgNewFolder(CString strTemp, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgNewFolder();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_NEW_FOLDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strFolder;
	CEdit m_EditFolder;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};

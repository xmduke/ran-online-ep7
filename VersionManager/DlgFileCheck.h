#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgFileCheck 대화 상자입니다.

class CDlgFileCheck : public CDialog
{
	DECLARE_DYNAMIC(CDlgFileCheck)

public:
	CDlgFileCheck(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgFileCheck();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_FILECHECK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    void    StartFileCheck();
    BOOL    IsExist(CString strFullPath);
    afx_msg void OnTimer(UINT nIDEvent);
protected:
    CProgressCtrl m_ProgressCheck;
public:
    CStatic m_StaticCheck;
    afx_msg void OnEnMaxtextEditCheck();
};

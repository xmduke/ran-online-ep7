#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgFTP 대화 상자입니다.

class CDlgFTP : public CDialog
{
	DECLARE_DYNAMIC(CDlgFTP)

public:
	CDlgFTP(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgFTP();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_FTP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

    void UploadStart();

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    CProgressCtrl m_ProgressFTP;
    afx_msg void OnTimer(UINT nIDEvent);
    CStatic m_StaticFTP;
    afx_msg void OnBnClickedStop();
    BOOL m_bStop;
    HANDLE m_hThread;
    afx_msg void OnBnClickedStart();
    int StartUploadThread();
    int UploadThreadProc();
    afx_msg void OnEnMaxtextEditFtp();
};

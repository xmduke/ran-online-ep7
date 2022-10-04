#pragma once


// CNewFolderDlg 대화 상자입니다.

class CNewFolderDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewFolderDlg)

public:
	CNewFolderDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CNewFolderDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NEWFOLDER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strFolderName;
};

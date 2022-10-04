#pragma once


// CFile2ListDialog 대화 상자입니다.

class CFile2ListDialog : public CDialog
{
	DECLARE_DYNAMIC(CFile2ListDialog)

public:
	CFile2ListDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CFile2ListDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_FILE2LISTDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBegin();
	afx_msg void OnBnClickedButtonFile();
	afx_msg void OnBnClickedButtonEnd();
};

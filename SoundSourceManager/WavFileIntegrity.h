#pragma once


// CWavFileIntegrity 대화 상자입니다.

class CWavFileIntegrity : public CDialog
{
	DECLARE_DYNAMIC(CWavFileIntegrity)

public:
	CWavFileIntegrity(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CWavFileIntegrity();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_WAVFILEINTEGRITY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBegin();
	afx_msg void OnBnClickedButtonEnd();
	afx_msg void OnBnClickedButtonFile();
};

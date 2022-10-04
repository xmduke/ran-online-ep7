#pragma once


// CChatMsgDlg 대화 상자입니다.

class CChatMsgDlg : public CDialog
{
	DECLARE_DYNAMIC(CChatMsgDlg)

public:
	CChatMsgDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CChatMsgDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MESSAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strMessage;
};

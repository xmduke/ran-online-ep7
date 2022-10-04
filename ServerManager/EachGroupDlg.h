#pragma once
#include "afxwin.h"


// CEachGroupDlg 대화 상자입니다.

class CEachGroupDlg : public CDialog
{
	DECLARE_DYNAMIC(CEachGroupDlg)

public:
	CEachGroupDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CEachGroupDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_EACHGROUP };

	void SetEachGroupList( std::vector<int> vecEachGroup );
	void GetSelectEachGroupList();
	std::vector<int> GetSelectEachGroupList() { return m_vecCheckServ; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	std::vector<int> m_vecEachGroup;
	std::vector<int> m_vecCheckServ;
public:
	virtual BOOL OnInitDialog();
	CButton m_SvrGroupCheck[8];
};

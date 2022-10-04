#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <vector>
#include "DatabaseTable.h"
#include "MinEdit.h"

// CFormLeft 폼 뷰입니다.
class CFormLeft : public CFormView
{
	DECLARE_DYNCREATE(CFormLeft)

protected:
	CFormLeft();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormLeft();

public:
	enum { IDD = IDD_FORMLEFT };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CMinEdit m_Edit;
	BOOL m_bInit;
	int nSelectedItem;		
	CListCtrl m_Folder;		
	std::vector<FolderList> m_vFolder;

	void InitListHead();
	void InitListData();
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();		
	afx_msg void OnSize(UINT nType, int cx, int cy);	
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickList1(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnMakeCmd();
	afx_msg void OnDeleteCmd();
    afx_msg void OnTimer(UINT nIDEvent);
};



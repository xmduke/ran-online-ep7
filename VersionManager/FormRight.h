#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <vector>
#include "DatabaseTable.h"
#include "MinEdit.h"

// CFormRight 폼 뷰입니다.

class CFormRight : public CFormView
{
	DECLARE_DYNCREATE(CFormRight)

protected:
	CFormRight();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormRight();

public:
	enum { IDD = IDD_FORMRIGHT };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();

	void InitListHead(); // 리스트 컨트롤 헤더 초기화
	void InitListData(); // 리스트 컨트롤 Data 초기화
	void FillData(int nFolder);

	CListCtrl m_File;
	std::vector<FileList> m_vFile;
	CMinEdit m_Edit;
	int	m_nDir;
	BOOL m_bInit;
	int nSelectedItem;
	afx_msg void OnSize(UINT nType, int cx, int cy);	
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRightAdd();
	afx_msg void OnRightEdit();
	afx_msg void OnRightDelete();
};



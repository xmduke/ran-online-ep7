#if !defined(AFX_PROGRESSLISTCTRL_H__BDB8B31B_5EA5_11D5_A5DD_00A0CCD221FF__INCLUDED_)
#define AFX_PROGRESSLISTCTRL_H__BDB8B31B_5EA5_11D5_A5DD_00A0CCD221FF__INCLUDED_

#include "AdvHeaderCtrl.h"	// Added by ClassView
#include <afxtempl.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgressListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProgressListCtrl window

#define COLOR_RED		0
#define COLOR_BLUE		1
#define COLOR_GREEN		2
#define COLOR_BLACK		3
#define COLOR_YELLOW	4
#define COLOR_WHITE		5

#define PROGRESS_INDEX	6

class CProgressListCtrl : public CListCtrl
{
// Construction
public:
	CProgressListCtrl();

// Attributes
public:
	CAdvHeaderCtrl m_AdvHdr;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	void InitHdr();
	void ResizeProg();
	void SetProgress(int Index, int prog);
	
	void InsertServer(int nState, 
					 const char* szType, 
					 const char* szName,
					 const char* szIP,
					 int nPort,
					 int nCurClient, 
					 int nMaxClient, 
					 int nColor);

	void CreateProgress(int Index);
	void CreateColumns();
	virtual ~CProgressListCtrl();

	// Generated message map functions
protected:

	class CProgressEntry 
	{
	public:
		CProgressEntry(int Index, int SubIndex);
		virtual ~CProgressEntry();
	
		int m_Index;
		int m_SubIndex;
  	
		CProgressCtrl m_Prog;
	};

	CMap < int, int, CProgressEntry*, CProgressEntry*& > m_ProgEntries;

	// CAdvHeaderCtrl m_AdvHdr;
	//{{AFX_MSG(CProgressListCtrl)
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	
protected:

public:
	afx_msg void OnHdnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESSLISTCTRL_H__BDB8B31B_5EA5_11D5_A5DD_00A0CCD221FF__INCLUDED_)

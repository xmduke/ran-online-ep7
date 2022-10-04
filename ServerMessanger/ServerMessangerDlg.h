// ServerMessangerDlg.h : header file
//

#pragma once

#include "Resource.h"
#include "s_CNetCtrl.h"

// CServerMessangerDlg dialog
class CServerMessangerDlg : public CDialog
{
// Construction
public:
	CServerMessangerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SERVERMESSANGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support	

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	void MsgProcess(NET_MSG_GENERIC* nmg);
};

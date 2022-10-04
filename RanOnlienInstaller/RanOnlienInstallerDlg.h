// RanOnlienInstallerDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class	CHttpPatch;
class	CLPatchThread;

// CRanOnlienInstallerDlg 대화 상자
class CRanOnlienInstallerDlg : public CDialog
{
// 생성
public:
	CRanOnlienInstallerDlg(CWnd* pParent = NULL);	// 표준 생성자

	void UpdateControls();

	void BEGIN_DOWNLOAD();
	void END_DOWNLOAD();
	void EXECUTE_INSTALL();
	BOOL IsVista();


	void OnKillThread();

// 대화 상자 데이터
	enum { IDD = IDD_DOWNLOADDLG };

	enum 
	{
		E_START_PATCH	= 1,
		E_FORCE_EXIT	= 2,
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

public:
	CString m_DownloadURL;
	CString m_strFileName;
	CString m_strSavePath;

	BOOL m_bForceTerminate;

	CHttpPatch*	m_pHttpPatch;
	CLPatchThread * m_pThread; // 패치 스레드

	ULONGLONG m_ULBefore;
	float m_dwBps;
	DWORD m_dwBeforeTime;
	DWORD m_dwCntTime;

// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedExecuteButton();
	afx_msg void OnBnClickedStartButton();
	afx_msg void OnBnClickedCloseButton();
	afx_msg void OnTimer(UINT nIDEvent);
	
	CButton m_ctrlExecute;
	CButton m_ctrlStart;
	CProgressCtrl m_ctrlProgressAll;
protected:
	virtual void PostNcDestroy();
public:
	BOOL m_bAutoExecute;
};

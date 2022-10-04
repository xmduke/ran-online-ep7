// PatchPrimeManDlg.h : 헤더 파일
//

#pragma once
#include "afxcmn.h"

class	CHttpPatch;
class	CPatch;
class	CLPatchThread;

class CPatchPrimeManDlg : public CDialog
{
// 생성
public:
	CPatchPrimeManDlg(CWnd* pParent = NULL);	// 표준 생성자
// 대화 상자 데이터
	enum { IDD = IDD_PATCHPRIMEMAN_DIALOG };

private:

	//	타이머 ID
	enum
	{
		//E_LOGIN_CONNECT = 1,
		//E_CHECK_VERSION = 2,
		E_FTP_CONNECT	= 3,
		E_START_PATCH	= 4,
		E_FORCE_EXIT	= 5,
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

protected:
	void	SetAppPath ();
	void	WaitForLauncherClose();

private:
	void	DeleteInvalidFile ();

private:	
	//CPatch*		m_pFtpPatch;
	CHttpPatch*	m_pHttpPatch;
	bool		m_bUseHttp;

	CLPatchThread * m_pThread; // 패치 스레드

private:
	int		m_nFtpTry;			//	FTP 시도
	BOOL	m_bForceTerminate;	//	강제 종료 시도

private:
	int		m_sPatchVer;
	int		m_sGameVer;

private:
	void	UpdateControls();
//	void	UpdateFtpConnect ();

//private:
//	void	BEGIN_FTP_THREAD ();

private:
	void	BEGIN_PATCH ();
	void	END_PATCH ();
	
//private:	
	//BOOL	ConnectServer ( CString strFtpAddress );
	//BOOL	DisconnectServer ();

private:
	BOOL CREATEPROCESS();
	void OnKillThread();

public:
	void ListAddString( const CString & strLog );
	void ListAddString( UINT nIDS );

// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CProgressCtrl m_ctrlProgressAll;
	afx_msg void OnTimer(UINT nIDEvent);	
	afx_msg void OnBnClickedButtonExit();

protected:
	virtual void PostNcDestroy();

public:
	CListBox m_ctrlListBox;
};

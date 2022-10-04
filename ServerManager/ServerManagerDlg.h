// ServerManagerDlg.h : 헤더 파일
//
#ifndef S_SERVERMANAGERDLG_H_
#define S_SERVERMANAGERDLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afxcmn.h"
#include "afxwin.h"
#include "Resource.h"
#include "afxtempl.h"

#include "s_CNetCtrl.h"
#include "Led.h" // LED Control
#include "ListCtrlEx.h"
#include "ServerViewDlg.h"
#include "JobManagerDlg.h"

#include "MatrixStatic.h"
#include "AlexfStaticHistogram.h"
#include "CLogFile.h"
#include "CManagerCfg.h"
#include "COdbcManager.h"

#if defined ( TW_PARAM ) || defined(HK_PARAM) 
//----------code added for S2ACOM by chming ----
#include "S2ACOM_i.cpp"	// this is a server file
#include "S2ACOM.h"		// this is a server file
#include "Comdef.h"		// for usage of BSTR
#include "Atlbase.h"	// for usage of CComBSTR
#endif // TW_PARAM

#include <atlbase.h>
//#import "UdsPPurioXComm.tlb" no_namespace

struct G_SERVER_INFO_DLG
{
	G_SERVER_INFO gsi;
	CServerViewDlg* pDlg;	// 상태창 포인터
	BOOL bOpen;				// 상태창이 열렸는지 닫혔는지 체크
	BOOL bCheck;			// 리스트를 새로받았을때 업데이트
	int  nRow;				// 리스트컨트롤 Row 넘버

	G_SERVER_INFO_DLG()
	{
		pDlg = NULL;
		bOpen = FALSE;
		bCheck = FALSE;
		nRow = -1;
	};

	friend bool operator==(const G_SERVER_INFO_DLG& a, const G_SERVER_INFO_DLG& b)
	{
		if ((strcmp(a.gsi.szServerIP, b.gsi.szServerIP) == 0) &&
			(a.gsi.nServicePort == b.gsi.nServicePort)) 
			return true;
		else
			return false;
	};
};

// CTracingDialog 대화 상자입니다.


struct G_SVR_SIMPLE
{
	char	szServerIP[MAX_IP_LENGTH+1];
	int		nServicePort;	
};

// CServerManagerDlg 대화 상자
class CServerManagerDlg : public CDialog
{
// 생성
public:
	CServerManagerDlg(CWnd* pParent = NULL);	// 표준 생성자
    virtual ~CServerManagerDlg();

// 대화 상자 데이터
	enum { IDD = IDD_SERVERMANAGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원	
	
// 구현
protected:
	HICON m_hIcon;
	CMatrixStatic m_MaxUser;
	CMatrixStatic m_CrtUser;
	CAlexfStaticHistogram m_Histogram;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:	
	CListCtrl			m_ListServer;
	CListCtrl			m_TracingList;
	CEdit				m_EditTracingID;
	CIPAddressCtrl		m_IPAddress;
	CEdit				m_Port;
	CNetCtrl*			m_pNetClient;
	CLed				m_Led1;	
	UINT*				m_nTimer;
	CEdit				m_EditRefreshRate;
	int					m_nMaxUser;
	CJobManagerDlg*		m_pJobDlg;
	CLogFile*			m_pLogFile;
	bool				m_bFirstRcvTracing;
	int					m_SendMsgNum;
	int					m_nPhoneSendCount;

	std::vector<STracingData> m_vecTracingData;
	
//	IUdsPPurioXPtr      m_PPurioX; // Korea SMS Module

#if defined ( TW_PARAM ) || defined(HK_PARAM) 
	IS2Air*             m_pSmsTaiwan; // Taiwan SMS Module
#endif

	CTypedPtrList<CPtrList, G_SERVER_INFO_DLG*> m_DlgList;
	std::vector<G_SERVER_INFO_DLG*>				m_vecServerList;

	CString m_strNoticeMsg;

public:	
	void	MsgProcess(NET_MSG_GENERIC* nmg);
	void	MsgAllSvrInfo(NET_MSG_GENERIC* nmg);
    void    SMS_ServerDown(NET_MSG_GENERIC* nmg);
	void    SMS_ServerCloseClient(NET_MSG_GENERIC* nmg);
	void	MsgUpdateTracingChar(NET_MSG_GENERIC* pMsg, bool bAll);

    void	InitListHead();
    
	void	AddListCtrl(G_SERVER_INFO_DLG* gsid);
	void	UpdateListCtrl(G_SERVER_INFO& gsi);
	void	UpdateSvrListStart();
	void	UpdateSvrListEnd();
	void	UpdateTracingList();

	void	RemoveDlgList(CServerViewDlg* pDlg);
	void	AddDlgList(CServerViewDlg* pDlg);
	void	RefreshDlgList();
	G_SVR_SIMPLE GetSvr();
	void	OnStopTimer();
	CString	GetAppPath();
    
    void    SendSMS(CString strHP, CString strText);
    void    SendAllSMS(CString strText);

	void    SetLedRed();
	void    SetLedGreen();

	void    PlayServerDownSound();
	void    PlayTracingUserLogOnOff();

	void	SendTracingList();

	afx_msg void OnBnClickedBtnConnect();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnNMDblclkListServer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickListServer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextmenuOpen();
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnContextmenuMessage();
	afx_msg void OnBnClickedBtnIdsearch();
	afx_msg void OnBnClickedBtnJobSchedule();
	afx_msg void OnContextmenuStop();
	afx_msg void OnContextmenuStop5();
	afx_msg void OnEnMaxtextEditConsole();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();    
    afx_msg void OnBnClickedBtnsmstest();
    afx_msg void OnBnClickedButton4();
    afx_msg void OnBnClickedButton2();    
    afx_msg void OnBnClickedBtnMsgSnd();

protected:
	virtual void OnOK();
	virtual void OnCancel();

public:
	afx_msg void OnClickMenuExit();
	afx_msg void OnBnClickedSendType1();
	afx_msg void OnBnClickedSendType2();
	afx_msg void OnJobAdd();
	afx_msg void OnJobEdit();
	afx_msg void OnJobDelete();
	CEdit m_EditMsg[20];

	CEdit m_EditConsole;
	afx_msg void OnBnClickedView10Button();
	afx_msg void OnBnClickedView20Button();
	afx_msg void OnBnClickedView1Button();
	afx_msg void OnBnClickedSvrstateButton();
	afx_msg void OnBnClickedTracingButton();
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnBnClickedAddButton();
	afx_msg void OnBnClickedDelButton();
	afx_msg void OnBnClickedAddTypeCheck();
	afx_msg void OnBnClickedBtnMsgClear();
};

#endif // S_SERVERMANAGERDLG_H_

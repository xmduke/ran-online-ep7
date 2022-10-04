// MMPCDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"


// CMMPCDlg 대화 상자
class CMMPCDlg : public CDialog
{
// 생성
public:
	CMMPCDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_MMPC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원


// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();

protected:
	// 맵의 실제 좌표
	int m_nLX;
	int m_nTY;
	int m_nRX;
	int m_nBY;

	// 계산된 결과 값
	int m_nSizeX;
	int m_nSizeY;
	int m_nCalcLX;
	int m_nCalcBY;

	CString m_strDDSFileName;

public:
	CStatic m_csFileName;
	afx_msg void OnBnClickedOk();

protected:
	RECT GetBitmapPosition();
	void MiniMapPositionCalc( RECT & rect);
	BOOL MiniMapPositionSave( CString & strPathName, CString & strBUFFER );

	void InitEditView();
	void HideEditView();
	void ShowEditView( CString & strBUFFER = CString() );

public:
	afx_msg void OnBnClickedHide();
	CButton m_ctrlButtonCancel;
	CEdit m_ctrlEditView;
	CButton m_ctrlButtonHide;
	afx_msg void OnBnClickedButton2();
	CStatic m_csDDSFileName;
};

// VerManDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"

const int nFILENAME = 64;
const int nSUBPATH = 128;

struct	SFILENODE
{
	char	FileName[nFILENAME];
	char	SubPath[nSUBPATH];	
	int		Ver;

public:
	SFILENODE()
	{
		memset ( FileName, 0, sizeof ( char ) * nFILENAME );
		memset ( SubPath, 0, sizeof ( char ) * nSUBPATH );
		Ver = 0;
	}
};

// CVerManDlg 대화 상자
class CVerManDlg : public CDialog
{
// 생성
public:
	CVerManDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_VERMAN_DIALOG };

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
	CString m_valPatch;
	CString m_valClient;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonDummy();
};

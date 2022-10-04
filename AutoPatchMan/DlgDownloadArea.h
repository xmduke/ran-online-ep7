#pragma once

#include "afxwin.h"
#include "RANPARAM.h"

// CDlgDownloadArea 대화 상자입니다.

class CDlgDownloadArea : public CDialog
{
	DECLARE_DYNAMIC(CDlgDownloadArea)

public:
	CDlgDownloadArea(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgDownloadArea();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_DOWNLOAD_AREA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_nDownloadArea;

	CString m_strAreaName[RANPARAM::MAX_CHINA_REGION];
	virtual BOOL OnInitDialog();
};

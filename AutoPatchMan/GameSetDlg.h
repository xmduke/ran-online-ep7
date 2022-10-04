#pragma once

#include "afxcmn.h"
#include "d3dapp.h"
#include "afxwin.h"
#include "RANPARAM.h"

// CGameSetDlg 대화 상자입니다.

class CGameSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CGameSetDlg)

public:
	CGameSetDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CGameSetDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_GAMESETDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

private:
	long	m_MapVolume;
	long	m_SfxVolume;
	long	m_BgmVolume;

	std::string szCharShadow[5];
	std::string szCharDetail[3];
	std::string sz3DAlgorithm[3];
	std::string szFogRange[3];
	std::string szDefaultOption[4];

private:
	DWORD			m_dwScrWidth;
	DWORD			m_dwScrHeight;
	EMSCREEN_FORMAT	m_emScrFormat;
	UINT			m_uScrRefreshHz;

private:
	CD3DApplication	m_D3DApp;

private:
	BOOL	InitCtrls	();
	BOOL	UpdateCtrls ();
	BOOL	InverseUpdateCtrls ();
	void	InitString	();
	void	InitDlgText ();

	void	SetHighLevel();
	void	SetMediumLevel();
	void	SetLowLevel();

	DECLARE_MESSAGE_MAP()
public:	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonCompatible();
	afx_msg void OnCbnSelchangeComboResolution();
	afx_msg void OnCbnSelchangeComboHz();
	CButton m_buttonWindowMode;

	int m_nDownloadArea;
	CString m_strAreaName[RANPARAM::MAX_CHINA_REGION];
	afx_msg void OnCbnSelchangeComboDefaultoption();
	afx_msg void OnBnClickedCheckButton();
	afx_msg void OnCbnSelchangeCombo();
};

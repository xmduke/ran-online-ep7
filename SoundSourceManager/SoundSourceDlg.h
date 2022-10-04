////////////////////////////////////////////////////////////////////////////////
//	이름	:	CSoundSourceDlg
//	목적	:	각 레코드의 값을 만드는 다이얼로그임. 
//	부가설명:	내부적으로 데이타를	생성해서, 그것을 Doc로 넘겨 자료를 저장함
//	작성자	:	성기엽
//	eMail	:	kysung@mincoms.co.kr
//	WWW		:	http://www.ran-online.co.kr
//	
//	로그
//	$2002-10-22T21:07	최초 작성;
//
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "afxwin.h"

enum	EDlgType
{
	eNew,
	eModify
};

// CSoundSourceDlg 대화 상자입니다.
class CSoundSourceDlg : public CDialog
{
	DECLARE_DYNAMIC(CSoundSourceDlg)

public:
	CSoundSourceDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSoundSourceDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SOUNDSOURCEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

protected:
	EDlgType		m_eDlgType;
	WORD			m_ID;
	BOOL			m_bChangeFile;

public:
	void	SetState ( EDlgType DlgType, WORD ID = 0 );

protected:
	BOOL	ValidateWaveFile( const TCHAR* strFileName );


	DECLARE_MESSAGE_MAP()
public:
	CString m_valPathFile;
	CString	m_OriginFileName;
	CComboBox m_ctrlType;
	CString m_valComment;
	CString m_valStaticGroup;
	CButton m_ctrlButtonFindFile;
	afx_msg void OnBnClickedButtonFindfile();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	DWORD m_valBufferCount;
};

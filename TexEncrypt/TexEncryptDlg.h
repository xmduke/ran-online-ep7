// TexEncryptDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"


// CTexEncryptDlg 대화 상자
class CTexEncryptDlg : public CDialog
{
// 생성
public:
	CTexEncryptDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_TEXENCRYPT_DIALOG };

	enum 
	{
		ENCRYPT_FILE,
		ENCRYPT_FORDER,
		DECRYPT_FILE,
		DECRYPT_FORDER,
		OPTION_SIZE,
	};

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원


// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cCombo;
	CEdit m_cEdit;

	afx_msg void OnBnClickedOpenButton();
	afx_msg void OnBnClickedRunButton();


public:
	CString m_strAppPath;

	void SetAppPath();
	void Init();
	void OpenEncryptFile();
	void OpenDecryptFile();
	void OpenSelectForder();
	afx_msg void OnCbnSelchangeOptionCombo();

	BOOL RunEncryptFile( const CString& strInput, const CString& strOutput );
	BOOL RunEncryptForder( const CString& strInput, const CString& strOutput );
	BOOL RunDecryptFile( const CString& strInput, const CString& strOutput );
	BOOL RunDecryptForder( const CString& strInput, const CString& strOutput );

	void AddEditString( const char* szInfo );
};

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM dwData);



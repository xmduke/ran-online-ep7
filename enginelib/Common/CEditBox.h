#pragma once
#include "edit_ime.h"

class CEditBox
{
protected:
	BOOL		m_IsInput;

	BOOL		m_bComp;
	BOOL		m_bCompSet;
	int			m_nCompCount;
	RECT		m_rectEdit;
	CEdit_IME*	m_pEditBox;

public:
	BOOL Create ( CWnd *pWnd );
	void Destroy ();

	void SetWndRect ( RECT rtMove );

	BOOL IsOn ()					{ return m_IsInput; }
	void OnInput ();				// 문자열 입력 ON
	void OffInput ();				// 문자열 입력 OFF

	BOOL IsCOMPOSITION ();

	CString GetString ()
	{
		CString strText;
		if ( m_pEditBox )	m_pEditBox->GetWindowText(strText);
		return strText;
	}

	char* GetCompleteString ()		{ return ""; }
	char* GetCompsitionString ()	{ return ""; }
	void SetString ( const char* pStr = NULL ); // 초기 문자열 지정하기

	HRESULT FrameMove ( float fTime, float fElapsedTime );

public:
	CEditBox(void);
	~CEditBox(void);
};

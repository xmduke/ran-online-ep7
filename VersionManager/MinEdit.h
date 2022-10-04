#pragma once
#include "afxwin.h"

class CMinEdit : public CEdit
{
public:
	CMinEdit(void);
	~CMinEdit(void);
	DECLARE_MESSAGE_MAP()

public:
	long m_clrText;
	long m_clrBkgnd;
	CBrush m_brBkgnd;
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};

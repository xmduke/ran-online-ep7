#include "StdAfx.h"
#include ".\minedit.h"

CMinEdit::CMinEdit(void)
{
	m_clrText	= RGB(255, 255,   0); // Text color
	m_clrBkgnd	= RGB(  0,   0,   0); // Background color
	m_brBkgnd.CreateSolidBrush(m_clrBkgnd);
}

CMinEdit::~CMinEdit(void)
{
}

BEGIN_MESSAGE_MAP(CMinEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

HBRUSH CMinEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// TODO:  여기서 DC의 특성을 변경합니다.	
	pDC->SetTextColor(m_clrText);   // text
	pDC->SetBkColor  (m_clrBkgnd);  // text bkgnd
	return m_brBkgnd;  

	// TODO:  부모 처리기가 호출되지 않을 경우 Null이 아닌 브러시를 반환합니다.
	return NULL;
}

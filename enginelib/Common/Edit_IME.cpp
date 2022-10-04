#include "stdafx.h"
#include "edit_ime.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEdit_IME::CEdit_IME(void) :
	m_bStat(FALSE)
{
}

CEdit_IME::~CEdit_IME(void)
{
}

BEGIN_MESSAGE_MAP(CEdit_IME, CEdit)
END_MESSAGE_MAP()

LRESULT CEdit_IME::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch (message)
	{
		case WM_IME_STARTCOMPOSITION:
			//CDebugSet::ToView ( "WM_IME_STARTCOMPOSITION" );
			m_bStat = TRUE;
			break;

		case WM_IME_ENDCOMPOSITION:
			//CDebugSet::ToView ( "WM_IME_ENDCOMPOSITION" );
			m_bStat = FALSE;
			break;
	};

	return CEdit::WindowProc(message, wParam, lParam);
}
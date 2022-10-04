#include "StdAfx.h"

#include "CEditBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEditBox::CEditBox(void) :
	m_pEditBox(NULL),
	m_IsInput(FALSE),
	m_bComp(FALSE),
	m_bCompSet(FALSE),
	m_nCompCount(0)
{
	ZeroMemory ( &m_rectEdit, sizeof(RECT) );
	m_rectEdit.left = 0;
	m_rectEdit.top = 0;
	m_rectEdit.right = 200;
	m_rectEdit.bottom = 40;
}

CEditBox::~CEditBox(void)
{
	SAFE_DELETE(m_pEditBox);
}


BOOL CEditBox::Create ( CWnd *pWndParent )
{
	SAFE_DELETE(m_pEditBox);

	m_pEditBox = new CEdit_IME;
	m_pEditBox->Create ( ES_LEFT|ES_AUTOHSCROLL, m_rectEdit, pWndParent, 11234 );

	OffInput ();

	return TRUE;
}

void CEditBox::Destroy ()
{
	delete m_pEditBox;
	m_pEditBox = NULL;
}

void CEditBox::SetWndRect ( RECT rtMove )
{
	if ( m_pEditBox )
	{
		m_rectEdit = rtMove;
		m_pEditBox->MoveWindow ( &m_rectEdit );
	}
}

void CEditBox::OnInput ()
{
	m_IsInput = TRUE;
	m_pEditBox->SetFocus ();
	m_pEditBox->EnableWindow ( TRUE );
}

void CEditBox::OffInput ()
{
	m_IsInput = FALSE;
	m_pEditBox->EnableWindow ( FALSE );
	m_pEditBox->GetParent()->SetFocus();
}

BOOL CEditBox::IsCOMPOSITION ()
{
	return m_bCompSet;
}

HRESULT CEditBox::FrameMove ( float fTime, float fElapsedTime )
{
	if ( m_IsInput )
	{
		m_pEditBox->SetFocus ();
	}

	if ( m_pEditBox )
	{
		m_bComp = m_pEditBox->IsCOMPOSITION();
		if (m_bComp )
		{
			m_bCompSet = TRUE;
			m_nCompCount = 40;
		}

		if ( m_bCompSet && (m_nCompCount--) < 0 )
		{
			m_bCompSet = FALSE;
		}

		//CString strText;
		//m_pEditBox->GetWindowText(strText);
		//CDebugSet::ToView ( 19, strText.GetBuffer() );
		//CDebugSet::ToView ( 20, "%d", m_nCompCount );
	}

	return S_OK;
}

void CEditBox::SetString ( const char* pStr )
{
	if ( m_pEditBox )
	{
		if ( !pStr )	m_pEditBox->SetWindowText ( "" );
		else
		{
			m_pEditBox->SetWindowText ( pStr );

			m_pEditBox->SendMessage ( EM_SETSEL , ( WPARAM ) strlen(pStr), ( LPARAM ) strlen(pStr) );
		}
	}
}







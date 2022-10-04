#include "stdafx.h"
#include ".\uieditboxime_dx9.h"
#include "DxResponseMan.h"

CUIEditBoxIME_Dx9::CUIEditBoxIME_Dx9(void)
	: m_fElapsedTime(0)
{
}

CUIEditBoxIME_Dx9::~CUIEditBoxIME_Dx9(void)
{
}

HRESULT CUIEditBoxIME_Dx9::FrameMove(LPDIRECT3DDEVICEQ pd3dDevice,float fElapsedTime)
{
	m_fElapsedTime = fElapsedTime;
	return S_OK;
}

HRESULT CUIEditBoxIME_Dx9::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;

	//V_RETURN( CUIControl::Render( pd3dDevice ) );
	V_RETURN( m_EditUI.OnRender( pd3dDevice, m_fElapsedTime ) );

	return S_OK;
}

LRESULT CUIEditBoxIME_Dx9::MsgProc( MSG* pMsg )
{
	//CUIControl::MsgProc( pMsg );
	m_EditUI.MsgProc( pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam );

	return 0L;
}

HRESULT CUIEditBoxIME_Dx9::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hr;

	V_RETURN( CUIControl::RestoreDeviceObjects( pd3dDevice ) ); 

	// 에디터 박스의 위치를 조정해준다.
	m_EditUI.SetLocation( 0, 0 );
	m_EditUI.SetSize( DxResponseMan::GetInstance().GetD3DSurfaceDesc()->Width, 
						DxResponseMan::GetInstance().GetD3DSurfaceDesc()->Height );

	m_EditUI.GetControl( IDC_EDITBOX )->SetLocation( 400, 500 );
	m_EditUI.GetControl( IDC_EDITBOX )->SetSize( 200, 36 );

	return S_OK;
}

void CUIEditBoxIME_Dx9::InitEditBox( CDXUTDialogResourceManager * pDialogResourceManager, PCALLBACKDXUTGUIEVENT pCallback )
{
	m_EditUI.Init( pDialogResourceManager );
	m_EditUI.SetCallback( pCallback );
}

HRESULT CUIEditBoxIME_Dx9::SetFont( UINT index, LPCWSTR strFaceName, LONG height, LONG weight )
{
	HRESULT hr;

	//V_RETURN( m_EditUI.SetFont( index, strFaceName, height, weight ) );
	m_EditUI.SetFont( 1, L"돋움체", 12, FW_NORMAL );
	m_EditUI.SetFont( 2, L"Courier New", 16, FW_NORMAL );

	return S_OK;
}

HRESULT CUIEditBoxIME_Dx9::AddEditBox( INT nID, LPCWSTR strText, D3DCOLOR dwColor )
{
	HRESULT hr;

	CDXUTIMEEditBox *pIMEEdit;
	if( SUCCEEDED( m_EditUI.AddIMEEditBox( nID, strText, 400, 500, 200, 36, false, &pIMEEdit ) ) )
	{
		pIMEEdit->GetElement( 0 )->iFont = 1;
		pIMEEdit->GetElement( 1 )->iFont = 1;
		pIMEEdit->GetElement( 9 )->iFont = 1;
		pIMEEdit->GetElement( 0 )->TextureColor.Init( dwColor );  // Transparent center
		pIMEEdit->SetBorderWidth( 7 );
		pIMEEdit->SetTextColor( D3DCOLOR_ARGB( 255, 64, 64, 64 ) );
		pIMEEdit->SetCaretColor( D3DCOLOR_ARGB( 255, 64, 64, 64 ) );
		pIMEEdit->SetSelectedTextColor( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
		pIMEEdit->SetSelectedBackColor( D3DCOLOR_ARGB( 255, 40, 72, 72 ) );
	}
	else
	{
		return E_FAIL;
	}
}
#include "stdafx.h"
#include ".\uieditbox_dx9.h"
#include "DxResponseMan.h"

CUIEditBox_Dx9::CUIEditBox_Dx9(void)
	: m_fElapsedTime(0)
{
}

CUIEditBox_Dx9::~CUIEditBox_Dx9(void)
{
}

HRESULT CUIEditBox_Dx9::FrameMove(LPDIRECT3DDEVICEQ pd3dDevice,float fElapsedTime)
{
	m_fElapsedTime = fElapsedTime;
	return S_OK;
}

HRESULT CUIEditBox_Dx9::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;

	//V_RETURN( CUIControl::Render( pd3dDevice ) );
	V_RETURN( m_EditUI.OnRender( pd3dDevice, m_fElapsedTime ) );

	return S_OK;
}

LRESULT CUIEditBox_Dx9::MsgProc( MSG* pMsg )
{
	//CUIControl::MsgProc( pMsg );
	m_EditUI.MsgProc( pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam );

	return 0L;
}

HRESULT CUIEditBox_Dx9::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hr;

	V_RETURN( CUIControl::RestoreDeviceObjects( pd3dDevice ) ); 

	// 에디터 박스의 위치를 조정해준다.
	m_EditUI.SetLocation( 0, 0 );
	m_EditUI.SetSize( DxResponseMan::GetInstance().GetD3DSurfaceDesc()->Width, 
						DxResponseMan::GetInstance().GetD3DSurfaceDesc()->Height );

	m_EditUI.GetControl( IDC_EDITBOX )->SetLocation( 400, 400 );
	m_EditUI.GetControl( IDC_EDITBOX )->SetSize( 200, 36 );

	return S_OK;
}

void CUIEditBox_Dx9::InitEditBox( CDXUTDialogResourceManager * pDialogResourceManager, PCALLBACKDXUTGUIEVENT pCallback )
{
	m_EditUI.Init( pDialogResourceManager );
	m_EditUI.SetCallback( pCallback );
}

HRESULT CUIEditBox_Dx9::SetFont( UINT index, LPCWSTR strFaceName, LONG height, LONG weight )
{
	HRESULT hr;

	//V_RETURN( m_EditUI.SetFont( index, strFaceName, height, weight ) );
	m_EditUI.SetFont( 1, L"돋움체", 24, FW_NORMAL );
	m_EditUI.SetFont( 2, L"Courier New", 16, FW_NORMAL );

	return S_OK;
}

HRESULT CUIEditBox_Dx9::AddEditBox( INT nID, LPCWSTR strText, D3DCOLOR dwColor )
{
	HRESULT hr;

	V_RETURN( m_EditUI.AddEditBox( nID, strText, 400, 400, 200, 36 ) );
	m_EditUI.GetEditBox( IDC_EDITBOX )->SetTextColor( dwColor );

	return S_OK;
}
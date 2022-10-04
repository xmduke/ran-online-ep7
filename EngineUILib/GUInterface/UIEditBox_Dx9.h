#pragma once

#include "UIControl.h"
#include "UITextControl.h"

class CUIEditBox_Dx9 : public CUIControl
{
public:
	enum
	{
		IDC_EDITBOX
	};

protected:
	CDXUTDialog m_EditUI;
	float m_fElapsedTime;

public:
	CUIEditBox_Dx9();
	virtual ~CUIEditBox_Dx9();

	void InitEditBox( CDXUTDialogResourceManager * pDialogResourceManager, PCALLBACKDXUTGUIEVENT pCallback = NULL );
	HRESULT SetFont( UINT index, LPCWSTR strFaceName, LONG height, LONG weight );

	HRESULT AddEditBox( INT nID, LPCWSTR strText = L"", D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE );

public:
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT FrameMove(LPDIRECT3DDEVICEQ pd3dDevice,float fElapsedTime);
	virtual LRESULT MsgProc( MSG* pMsg );
};

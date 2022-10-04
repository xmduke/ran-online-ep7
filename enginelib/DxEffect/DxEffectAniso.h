// DxEffectAniso.h: interface for the DxEffectAniso class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXEFFECTANISO_H__4F8FB352_601D_42CE_80B3_BE3521C0E102__INCLUDED_)
#define AFX_DXEFFECTANISO_H__4F8FB352_601D_42CE_80B3_BE3521C0E102__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DxEffectFrame.h"

struct DxFrame;

struct ANISO_PROPERTY
{
	char	m_szTexture[MAX_PATH];
};

class DxEffectAniso : public DxEffectBase
{
	//	Note : 이펙트 타입 정의.
	//
public:
	const static DWORD	TYPEID;
	const static DWORD	VERSION;
	const static DWORD	FLAG;
	const static char	NAME[];

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return FLAG; }
	virtual const char* GetName ()	{ return NAME; }

	virtual void GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer );
	virtual void SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer );

	//	Note : 쉐이더.
	//
protected:	
	static DWORD		m_dwEffect;
	static DWORD		dwAnisoVertexShader[];

	static DWORD		m_dwSavedStateBlock;
	static DWORD		m_dwEffectStateBlock;

	//	Note : 속성.
	//
protected:
	union
	{
		struct
		{
			ANISO_PROPERTY		m_Property;
		};

		struct
		{
			char	m_szTexture[MAX_PATH];
		};
	};

public:
	void SetProperty ( ANISO_PROPERTY& Property )
	{
		m_Property = Property;
	}
	ANISO_PROPERTY& GetProperty () { return m_Property; }

protected:
	LPDIRECT3DTEXTURE8	m_pddsTexture;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICE8 pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICE8 pd3dDevice );

public:
	HRESULT AdaptToDxFrame ( DxFrame *pframeCur, LPDIRECT3DDEVICE8 pd3dDevice );

	//	Note : 
	//
public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );	
	HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICE8 pd3dDevice );

public:
	DxEffectAniso();
	virtual ~DxEffectAniso();
};

#endif // !defined(AFX_DXEFFECTANISO_H__4F8FB352_601D_42CE_80B3_BE3521C0E102__INCLUDED_)

// DxEffectBlurC.h: interface for the DxEffectBlurC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXEFFECTBLURC_H__INCLUDED_)
#define AFX_DXEFFECTBLURC_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DxEffectFrame.h"

struct DxFrame;

struct BLURCURVE_PROPERTY
{
	bool			m_bUse;
	bool			m_bTestPlay;
	DWORD			m_dwFaces;
	D3DCOLOR		m_vColorUP;
	D3DCOLOR		m_vColorDOWN;
	int				m_iBlurAdd;				// ´õÇØÁö´Â ÀÜ»ó °¹¼ö
	int				m_iColorUP;
	int				m_iColorDOWN;
	D3DXVECTOR3		m_vBlurStart;
	D3DXVECTOR3		m_vBlurEnd;
	bool			m_bTex;
	char			m_szTexture[MAX_PATH];
};


class DxEffectBlurC : public DxEffectBase
{
	//	Note : ÀÌÆåÆ® Å¸ÀÔ Á¤ÀÇ.
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

	//	Note : ½¦ÀÌ´õ.
	//
protected:	
	static DWORD	m_dwSavedStateBlock;
	static DWORD	m_dwEffectStateBlock;

	//	Note : ¼Ó¼º.
	//
protected:
	union
	{
		struct
		{
			BLURCURVE_PROPERTY		m_Property;
		};

		struct
		{
			bool			m_bUse;
			bool			m_bTestPlay;
			DWORD			m_dwFaces;
			D3DCOLOR		m_vColorUP;
			D3DCOLOR		m_vColorDOWN;
			int				m_iBlurAdd;		// ´õÇØÁö´Â ÀÜ»ó °¹¼ö
			int				m_iColorUP;
			int				m_iColorDOWN;
			D3DXVECTOR3		m_vBlurStart;
			D3DXVECTOR3		m_vBlurEnd;
			bool			m_bTex;
			char			m_szTexture[MAX_PATH];
		};
	};

public:
	void SetProperty ( BLURCURVE_PROPERTY& Property )
	{
		m_Property = Property;
	}
	BLURCURVE_PROPERTY& GetProperty () { return m_Property; }

protected:
	float	m_fTime;
	DWORD	m_dwCount;

	struct BLURTEX
	{
		D3DXVECTOR3	p;
		D3DCOLOR    d;
		D3DXVECTOR2	t;
		const static DWORD FVF;
	};

	LPDIRECT3DVERTEXBUFFER8		m_pVB;

	DxFrame		*m_pframeCur;
	BLURTEX		*m_vBlurTexNow;

	BLURTEX		m_pFirstBlur[4];
	float		m_fTestTime;

	int			m_iBlurNum;		// »Ñ·Á¾ß ÇÒ ÀÜ»ó °¹¼ö

	int			m_iAdd;			// ´õÇØÁö´Â ÀÜ»ó °¹¼ö

	D3DXMATRIX	matTestWorld;

	LPDIRECT3DTEXTURE8	m_pddsTexture;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICE8 pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICE8 pd3dDevice );

protected:
	HRESULT AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICE8 pd3dDevice );

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
	HRESULT		MakeEffBlurCMeshs( LPDIRECT3DDEVICE8 pd3dDevice );
	HRESULT		SetInitMesh( );
	D3DXVECTOR3 GetPoint ( int UpDown );

public:
	DxEffectBlurC();
	virtual ~DxEffectBlurC();
};

#endif // !defined(AFX_DXEFFECTBLURC_H__INCLUDED_)


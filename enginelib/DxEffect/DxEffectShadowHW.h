// DxEffectShadowHW.h: interface for the DxEffectShadowHW class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXEFFECTSHADOWHW_H__INCLUDED_)
#define AFX_DXEFFECTSHADOWHW_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DxEffectFrame.h"

class DxSkinChar;

class DxEffectShadowHW
{
protected:
	static		LPDIRECT3DVERTEXBUFFER8		m_pShadowVB;
	const static	float					m_fRange;

protected:	
	static DWORD	m_dwSavedStateBlock;
	static DWORD	m_dwEffectStateBlock;

protected:
	float				m_fDistanceRate;
	float				m_fHeight;

	D3DXVECTOR3			m_vShadowPos;

	struct	VERTEX
	{
		D3DXVECTOR3			p;
		D3DXVECTOR2			t;
		static const DWORD	FVF;
	};

public:
	DxEffectShadowHW();
	virtual ~DxEffectShadowHW();
};

#endif // !defined(AFX_DXEFFECTSHADOWHW_H__INCLUDED_)

// DxEffectSprite.h: interface for the DxEffectBlurC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXEFFECTSPRITE_H__INCLUDED_)
#define AFX_DXEFFECTSPRITE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DxEffectFrame.h"

struct DxOcMeshes;
struct DxFrame;

struct SPRITE_PROPERTY
{
	D3DXVECTOR3		m_vCenter;
	float			m_fWidth;	// 위쪽과 아래쪽을 분리 해야 한다.
	float			m_fHeight;
	float			m_fDistance;
	DWORD			m_dwColor;
	int				m_iCol;
	int				m_iRow;
	float			m_fAniTime;
	char			m_szTexture[MAX_PATH];
};


class DxEffectSprite : public DxEffectBase
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
	static DWORD	m_dwSavedStateBlock;
	static DWORD	m_dwEffectStateBlock;

	//	Note : 속성.
	//
protected:
	union
	{
		struct
		{
			SPRITE_PROPERTY		m_Property;
		};

		struct
		{
			D3DXVECTOR3		m_vCenter;
			float			m_fWidth;
			float			m_fHeight;
			float			m_fDistance;
			DWORD			m_dwColor;
			int				m_iCol;
			int				m_iRow;
			float			m_fAniTime;
			char			m_szTexture[MAX_PATH];
		};
	};

public:
	void SetProperty ( SPRITE_PROPERTY& Property )
	{
		m_Property = Property;
	}
	SPRITE_PROPERTY& GetProperty () { return m_Property; }

protected:
	LPDIRECT3DTEXTURE8	m_pddsTexture;

	float	m_fTime;

	struct QUAD
	{
		D3DXVECTOR3	p;
		D3DCOLOR	d;
		D3DXVECTOR2 t;
	};

	LPD3DXMESH	m_pSpriteMesh;

	int			iNowSpriteNum;

protected:

	struct VERTEX
	{
		D3DXVECTOR3 p;       // vertex position
		D3DXVECTOR3 n;       // vertex normal
		D3DXVECTOR2 t;
		const static DWORD	FVF;
	};

protected:
	DxOcMeshes*		m_pOcMesh;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICE8 pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICE8 pd3dDevice );

protected:
	HRESULT AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICE8 pd3dDevice );
	
public:
	HRESULT AdaptToDxFrame ( DxFrame *pframeCur, LPDIRECT3DDEVICE8 pd3dDevice );

	HRESULT AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICE8 pd3dDevice );

	//	Note : 
	//
public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );	
	HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICE8 pd3dDevice );

private:
	HRESULT MakeEffSprite ( LPDIRECT3DDEVICE8 pd3dDevice, LPD3DXMESH pLocalMesh );
	D3DMATRIX BillboardLookAt ( D3DXVECTOR3 BillboardPos, D3DXVECTOR3 CarmeraPos );
	D3DXVECTOR3 MatrixVertex ( D3DMATRIX *matWorld, D3DXVECTOR3 *vPos );
	D3DXVECTOR3 Split ( D3DXPLANE *vPlane, D3DXVECTOR3 *vPos1, D3DXVECTOR3 *vPos2 );
	
public:
	DxEffectSprite();
	virtual ~DxEffectSprite();

protected:
	virtual void SaveBuffer ( CSerialFile &SFile );
	virtual void LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICE8 pd3dDevice );
	virtual void LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICE8 pd3dDevice );

	//	Note : DxFrameMesh 의 효과를 DxLandMan 에 삽입 할 때 대량의 버퍼 데이타를
	//		복제하기 위해서 사용한다.
public:
	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICE8 pd3dDevice );
};

#endif // !defined(AFX_DXEFFECTSPRITE_H__INCLUDED_)


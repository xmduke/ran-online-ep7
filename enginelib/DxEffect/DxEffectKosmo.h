// DxEffectKosmo.h: interface for the DxEffectKosmo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXEFFECTKOSMO_H__INCLUDED_)
#define AFX_DXEFFECTKOSMO_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DxEffectFrame.h"

struct DxFrame;

struct KOSMO_PROPERTY
{
	int			m_iTemp;

	D3DXVECTOR3	m_vPos;

	D3DXVECTOR3	LineStart;
	D3DXVECTOR3	LineEnd;
	float		m_fScale;

	float		m_fWidth;
	float		m_fHeight;

	char	m_szTexture[MAX_PATH];
};


class DxEffectKosmo : public DxEffectBase
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

public:

	struct KOSMOFACE
	{
		D3DXVECTOR3	Pos1;
		D3DXVECTOR3	Pos2;
		D3DXVECTOR4	Temp1;
		D3DXVECTOR3	Temp2;
		float		Diff;
	};

	struct KOSMOSTRIP
	{
		D3DXVECTOR3	p;
		D3DCOLOR    d;
		D3DXVECTOR2	t;
		const static DWORD FVF;
	};

	struct VERTEX
	{
		D3DXVECTOR3	Pos;
		float		Diff;
		float		fSize;
	};

	//	Note : 쉐이더.
	//
protected:	
	static DWORD			m_dwEffect;
	static char				m_strEffect[];

	static DWORD			m_dwSavedStateBlock;
	static DWORD			m_dwEffectStateBlock;

	//	Note : 속성.
	//
protected:
	union
	{
		struct
		{
			KOSMO_PROPERTY	m_Property;
		};

		struct
		{
			int			m_iTemp;

			D3DXVECTOR3	m_vPos;

			D3DXVECTOR3	LineStart;
			D3DXVECTOR3	LineEnd;
			float		m_fScale;

			float		m_fWidth;
			float		m_fHeight;

			char	m_szTexture[MAX_PATH];
		};
	};

public:
	void SetProperty ( KOSMO_PROPERTY& Property )
	{
		m_Property = Property;
	}
	KOSMO_PROPERTY& GetProperty () { return m_Property; }

protected:
	LPDIRECT3DTEXTURE8			m_pddsTexture;

	LPDIRECT3DVERTEXBUFFER8		m_pVB;
	LPDIRECT3DINDEXBUFFER8		m_pIB;

	DWORD		m_dwVertex;


	DWORD		m_dwFace;
	VERTEX*		m_pBaseVertex;

	int			m_iScene;
	float		m_fTime;
	float		m_fElapsedTime;

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
	HRESULT AddKosmoVertex ( LPDIRECT3DDEVICE8 pd3dDevice, D3DXVECTOR3 Pos, float fSize );
	HRESULT AddKosmoMesh ( LPDIRECT3DDEVICE8 pd3dDevice, DWORD Faces );				// 다시 만들 때도 사용 한다.
	HRESULT AddMeshStrip ( LPDIRECT3DDEVICE8 pd3dDevice, DWORD Faces );

	HRESULT MakeMagLine ( LPDIRECT3DDEVICE8 pd3dDevice, D3DXVECTOR3 v1, D3DXVECTOR3 v2 );
	HRESULT LineMove ( KOSMOFACE* pVertices, VERTEX* pBaseVertices, DWORD Faces );

	HRESULT MakeMagCircle ( LPDIRECT3DDEVICE8 pd3dDevice, D3DXVECTOR3 Pos );
	HRESULT	CircleMove ( KOSMOFACE* pVertices, VERTEX* pBaseVertices, DWORD Faces );
	void	LineDuffuse	( KOSMOFACE* &pBuff, D3DXVECTOR3 v1, D3DXVECTOR3 v2 );

	HRESULT MakeMagCyclone ( LPDIRECT3DDEVICE8 pd3dDevice, D3DXVECTOR3 v1, D3DXVECTOR3 v2 );
	void	PushLine2		( KOSMOFACE* &pBuff, D3DXVECTOR3 inFromPos, D3DXVECTOR3 inToPos, float inFromSize, float inToSize, float Color1, float Color2 );

	HRESULT MakeMagLighting ( LPDIRECT3DDEVICE8 pd3dDevice, D3DXVECTOR3 v1, D3DXVECTOR3 v2 );
	void	PushStrip	( KOSMOSTRIP* &pBuff, D3DXVECTOR3 Pos, float Size, float Color );


	D3DXVECTOR3	GetThisVector ( DWORD Data );
	float		GetThisScale ( DWORD Data );

	HRESULT		GetThisVector ( DWORD Data, D3DXVECTOR3 vVector );
	HRESULT		GetThisScale ( DWORD Data, float fScale );

protected:
	HRESULT AddVertexBuffer ( KOSMOFACE* pVertices, VERTEX* pBaseVertices, DWORD Faces );

	HRESULT CreateMeshKosmo ( LPDIRECT3DDEVICE8 pd3dDevice );

	HRESULT FillVertexBuffer ( void );
	void	PushLine	( KOSMOFACE* &pBuff, D3DXVECTOR3 inFromPos, D3DXVECTOR3 inToPos, float inFromSize, float inToSize );

public:
	DxEffectKosmo();
	virtual ~DxEffectKosmo();
};

#endif // !defined(AFX_DXEFFECTKOSMO_H__INCLUDED_)

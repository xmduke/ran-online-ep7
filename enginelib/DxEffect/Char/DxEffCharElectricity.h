#pragma once

#include "DxEffChar.h"

#include "TextureManager.h"
#include "DxMethods.h"

#define	USEIN			0x0001	// 안 사용
#define	USEOUT			0x0002	// 밖 사용
#define	USESTART		0x0010	// 시작 모임
#define	USEEND			0x0020	// 끝 모임

struct EFFCHAR_PROPERTY_ELECTRICITY : public EFFCHAR_PROPERTY
{
	BOOL			m_bUse;
	int				m_nBlend;
	char			m_szTraceUP[STRACE_NSIZE];		//	추적 포인터 인덱스.
	char			m_szTraceDOWN[STRACE_NSIZE];	//	추적 포인터 인덱스.

	DWORD			m_dwFlag;

	DWORD			m_dwLightning;	// 전기 갯수

	DWORD			m_dwDivision;	// 분할 갯수.

	float			m_fWidth_In;
	float			m_fWidth_Out;

	DWORD			m_dwMaxLenth;	// Out 휘도록
	DWORD			m_dwVelocity;	// In 휘도록

	float			m_fAlphaRate1;
	float			m_fAlphaRate2;

	float			m_fAlphaStart;
	float			m_fAlphaMid1;
	float			m_fAlphaMid2;
	float			m_fAlphaEnd;

	D3DXCOLOR		m_cColorStart;
	D3DXCOLOR		m_cColorEnd;

	char			m_szTexture_In[256];
	char			m_szTexture_Out[256];

	EFFCHAR_PROPERTY_ELECTRICITY () :
		m_bUse(TRUE),
		m_nBlend(1),

		m_dwDivision(7),		// 분할.!

		m_dwLightning(2),

		m_fWidth_In(1.f),
		m_fWidth_Out(0.2f),

		m_dwMaxLenth(3),		// 각각.. 휘도록
		m_dwVelocity(2),		// 전체.. 휘도록

		m_fAlphaRate1(25.0f),
		m_fAlphaRate2(75.0f),

		m_fAlphaStart(0.0f),
		m_fAlphaMid1(1.0f),
		m_fAlphaMid2(1.0f),
		m_fAlphaEnd(0.0f),

		m_cColorStart(1.0f,0.8f,0.5f,0.5f),
		m_cColorEnd(0.4f,0.5f,1.0f,0.0f)
	{
		m_dwFlag = USEIN | USEOUT;
		strcpy( m_szTexture_In, "_Eff_flare03.tga" );
		strcpy( m_szTexture_Out, "_Eff_flare03.tga" );

		strcpy(m_szTraceUP,STRACE_IMAGE01);
		strcpy(m_szTraceDOWN,STRACE_IMAGE02);
	}
};

class DxEffCharElectricity : public DxEffChar
{
public:
	static DWORD		TYPEID;
	static DWORD		VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return NULL; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_ELECTRICITY m_Property;
		};
		
		struct
		{
			BOOL			m_bUse;
			int				m_nBlend;
			char			m_szTraceUP[STRACE_NSIZE];		//	추적 포인터 인덱스.
			char			m_szTraceDOWN[STRACE_NSIZE];	//	추적 포인터 인덱스.

			DWORD			m_dwFlag;

			DWORD			m_dwLightning;	// 전기 갯수

			DWORD			m_dwDivision;	// 분할 갯수.

			float			m_fWidth_In;
			float			m_fWidth_Out;

			DWORD			m_dwMaxLenth;	// Out 휘도록
			DWORD			m_dwVelocity;	// In 휘도록

			float			m_fAlphaRate1;
			float			m_fAlphaRate2;

			float			m_fAlphaStart;
			float			m_fAlphaMid1;
			float			m_fAlphaMid2;
			float			m_fAlphaEnd;

			D3DXCOLOR		m_cColorStart;
			D3DXCOLOR		m_cColorEnd;

			char			m_szTexture_In[256];
			char			m_szTexture_Out[256];
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_ELECTRICITY*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:	
	static DWORD	m_dwSavedStateBlock;
	static DWORD	m_dwEffectStateBlock;

	LPDIRECT3DDEVICE8 m_pd3dDevice;

protected:
	D3DXVECTOR3			m_vTransUP;				//	위치.
	D3DXVECTOR3			m_vTransDOWN;			//	위치.
	D3DXVECTOR3			m_vPrevUP;				//	위치.
	D3DXVECTOR3			m_vPrevDOWN;			//	위치.

protected:
	struct D3DVERTEX
	{
		D3DXVECTOR3		vPos;
		D3DCOLOR		Diffuse;
		D3DXVECTOR2		vTex;
		
		static const DWORD FVF;
	};

	D3DVERTEX*			m_pVertex;
	D3DVERTEX*			m_pVertex_Out;

	//struct POSITION
	//{
	//	D3DXVECTOR3		vPrevPos;
	//	D3DXVECTOR3		vNextPos;
	//};

	//POSITION*			m_pTargetPos;
	//int					m_nDivide;		// 분할 갯수 - 기본 1 , 2면 속도 1/2, 5면 속도 1/5	/// 저장 삽입에 필요할꺼 같다.
	//int					m_nRoop;

	struct D3DROOT
	{
		D3DXVECTOR3		vPos;
		D3DXVECTOR3		vLookatCross;
	};
	D3DROOT*			m_pRootVertex;

protected:
	D3DXVECTOR3		DirectSum ( D3DROOT* pPos, int i, int End );
	void			UpdatePos ();
	void			UpdatePos_Out ();
	HRESULT			CreateBlurVB( LPDIRECT3DDEVICE8 pd3dDevice );

public:
	LPDIRECT3DTEXTURE8		m_pTexture_In;
	LPDIRECT3DTEXTURE8		m_pTexture_Out;

public:
	int						m_nVertexNum;
	int						m_dwOriginNum;

	float					m_fLifeTime;

	float					m_fAlpha;

	float					m_fAlphaDelta1;
	float					m_fAlphaDelta2;
	float					m_fAlphaDelta3;

	float					m_fAlphaTime1;
	float					m_fAlphaTime2;

	D3DXCOLOR				m_cColor;
	D3DXCOLOR				m_cColorDelta;

	float					m_fElapsedTime;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICE8 pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICE8 pd3dDevice );

public:
	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT	Render ( LPDIRECT3DDEVICE8 pd3dDevice );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICE8 pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICE8 pd3dDevice );
	virtual HRESULT SaveFile ( basestream &SFile );

public:
	DxEffCharElectricity(void);
	~DxEffCharElectricity(void);
};
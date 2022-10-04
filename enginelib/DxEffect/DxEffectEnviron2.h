// DxEffectEnviron2t.h: interface for the DxEffectShadowHW class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXEFFECTENVIRON2_H__INCLUDED_)
#define AFX_DXEFFECTENVIRON2_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DxEffectFrame.h"

struct DxFrame;

struct SHADOW_PROPERTY
{
	WORD			m_wEnvCase;					// 상태

	float			m_fLoop;					// 구름 효과
	float			m_fLoopAdd;
	float			m_fDistance;
	float			m_fMultiply;
	char			m_szTexture[MAX_PATH];

	float			m_fDotPower;
	char			m_szTexDot3[MAX_PATH];		// 내적 블렌딩

	float			m_fWidth;					// 새로 시작
	float			m_fHeight;
	char			m_szTexDetail[MAX_PATH];

	DWORD			m_dwBlendAlpha;				// 반사
	char			m_szTexture1[MAX_PATH];
	char			m_szTexture2[MAX_PATH];
	char			m_szTexture3[MAX_PATH];
	char			m_szTexture4[MAX_PATH];
	char			m_szTexture5[MAX_PATH];
	char			m_szTexture6[MAX_PATH];
};


class DxEffectEnviron2 : public DxEffectBase
{
	//	Note : 이펙트 타입 정의.
	//
public:
	const static DWORD	TYPEID;
	const static char	NAME[];

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual const char* GetName ()	{ return NAME; }

	//	Note : 쉐이더
	//
protected:	
	static DWORD	m_dwEffReflect;
	static DWORD	m_dwEffCloudReflect;

	static char		m_strEffReflect[];
	static char		m_strEffCloudReflect[];

	static DWORD	m_dwSavedStateBlock;
	static DWORD	m_dwEffectStateBlock;


	//	Note : 속성.
	//
protected:
	union
	{
		SHADOW_PROPERTY	m_Property;

		struct
		{
			WORD			m_wEnvCase;					// 상태

			float			m_fLoop;					// 구름 효과
			float			m_fLoopAdd;
			float			m_fDistance;
			float			m_fMultiply;
			char			m_szTexture[MAX_PATH];

			float			m_fDotPower;
			char			m_szTexDot3[MAX_PATH];		// 내적 블렌딩

			float			m_fWidth;					// 새로 시작
			float			m_fHeight;
			char			m_szTexDetail[MAX_PATH];

			DWORD			m_dwBlendAlpha;				// 반사
			char			m_szTexture1[MAX_PATH];
			char			m_szTexture2[MAX_PATH];
			char			m_szTexture3[MAX_PATH];
			char			m_szTexture4[MAX_PATH];
			char			m_szTexture5[MAX_PATH];
			char			m_szTexture6[MAX_PATH];
		};
	};

public:
	void SetProperty ( SHADOW_PROPERTY& Property )
	{
		m_Property = Property;
	}
	SHADOW_PROPERTY& GetProperty () { return m_Property; }

protected:
	float				m_fTime;

	LPDIRECT3DTEXTURE8		m_pCloudTexture;	// 구름 그림자	 // 구름 텍스쳐
	LPDIRECT3DTEXTURE8		m_pDot3Texture;		// 내적 블렌딩	 // Dot3 텍스쳐
	LPDIRECT3DTEXTURE8		m_pDetailTexture;		// 정체 불명 블렌딩	 // Dot3 텍스쳐
	LPDIRECT3DCUBETEXTURE8	m_pCubeTexture;		// 반사			// 큐브 텍스쳐

//	WORD			m_wEnvCase;

protected:	

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
	DxEffectEnviron2();
	virtual ~DxEffectEnviron2();
};

#endif // !defined(AFX_DXEFFECTENVIRON2_H__INCLUDED_)

// Terrain0.h: interface for the DxEffectWater class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__DxEffectTile__INCLUDED__)
#define __DxEffectTile__INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DxEffectFrame.h"
#include "Collision.h"

#include <list>
#include <hash_map>
#include <map>
#include <algorithm>

struct DxMeshes;
struct DXMATERIAL;

struct TILE_PROPERTY					// Ver.102
{
	DWORD			m_dwFlag;

	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;

	DWORD			m_dwVertices;
	DWORD			m_dwFaces;

	char			m_szDetailTex[MAX_PATH];

	D3DXMATRIX		m_matFrameComb;

	TILE_PROPERTY () :
		m_vMax(0.f,0.f,0.f),
		m_vMin(0.f,0.f,0.f),
		m_dwVertices(0),
		m_dwFaces(0)
	{
		//m_dwFlag = USE_TEX_DRAW | USE_COLOR_DRAW;
	};
};

class DxEffectTile : public DxEffectBase
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
	void SetProperty ( TILE_PROPERTY& Property )
	{
		m_Property = Property;
	}
	TILE_PROPERTY& GetProperty () { return m_Property; }

	//	Note : 속성.
	//
protected:
	union
	{
		struct
		{
			TILE_PROPERTY		m_Property;
		};

		struct
		{
			DWORD			m_dwFlag;

			D3DXVECTOR3		m_vMax;
			D3DXVECTOR3		m_vMin;

			DWORD			m_dwVertices;
			DWORD			m_dwFaces;

			char			m_szDetailTex[MAX_PATH];

			D3DXMATRIX		m_matFrameComb;
		};
	};

	//	Note : 랜더링 데이터
	//
protected:
	static	DWORD			m_dwSavedSB_Detail;
	static	DWORD			m_dwDrawSB_Detail;

protected:
	struct VERTEX
	{
		D3DXVECTOR3			vPos;
		D3DXVECTOR3			vNor;
		D3DXVECTOR2			vTex;
		const static DWORD	FVF;
	};

	struct VERTEX_TEX2
	{
		D3DXVECTOR3			vPos;
		D3DXVECTOR3			vNor;
		D3DXVECTOR2			vTex1;
		D3DXVECTOR2			vTex2;
		const static DWORD	FVF;
	};

protected:
	enum	DEFAULT
	{	
		EM_TEXSIZE		= 128,						// 텍스쳐 사이즈
		EM_LENGTH_S		= 10,						// 작은 타일 길이
		EM_LENGTH		= 40,						// 디테일 타일 길이
		EM_GRID			= 30,							// 격자 수
		EM_QURD			= EM_GRID*EM_GRID,				// 격자 수
		EM_FACE			= EM_QURD*2,					// 격자 수
		EM_VERTEX_PART	= 32,								// 버텍스 수
		EM_VERTEX_ALL	= EM_VERTEX_PART*EM_VERTEX_PART,	// 버텍스 수 가로세로 전체
		EM_QURD_VERTEX	= EM_QURD*4,							// 사각형으로 따졌을 때의 정점 갯수
		EM_FULL_LENGTH	= EM_LENGTH*EM_GRID,					// 최대 길이
	};

	char		m_cBaseName[100];
	std::string	m_strBaseName;
	std::string	m_strExtName;

protected:
	struct TILE
	{
		DWORD		dwTile;		// 타일 번호
		VERTEX_TEX2	sVertex[4];	// 정점 정보

		TILE () :
			dwTile(0)
		{
		}
	};

	struct BASE
	{
		D3DXVECTOR3	vMax;				// Clip 하기 위한 값
		D3DXVECTOR3	vMin;				// Clip 하기 위한 값

		LPDIRECT3DVERTEXBUFFER8	pVB;		// Vertex Buffer
		LPDIRECT3DINDEXBUFFER8	pIB;		// Index Buffer
		LPDIRECT3DTEXTURE8		pTexture;	// BaseTex

		DWORD		sColor[EM_TEXSIZE][EM_TEXSIZE];
		TILE		sTile[EM_GRID][EM_GRID];			// 타일 정보

		BASE() :
			vMax(0.f,0.f,0.f),
			vMin(0.f,0.f,0.f),
			pTexture(NULL),
			pVB(NULL),
			pIB(NULL)
		{
			for ( DWORD y=0; y<EM_TEXSIZE; ++y )
			{
				for ( DWORD x=0; x<EM_TEXSIZE; ++x )
				{
					sColor[x][y] = 0xffffffff;
				}
			}
		};
		~BASE();
	};

	//typedef stdext::hash_map<DWORD,BASE*>		BASEMAP;
	typedef std::map<DWORD,BASE*>			BASEMAP;		// 타일의 X,Y 번호
	typedef BASEMAP::iterator				BASEMAP_ITER;

	BASEMAP		m_mapBase;

protected:
	VERTEX		m_sVertex[EM_VERTEX_PART][EM_VERTEX_PART];	// 데이터의 축적

	struct FACE
	{
		WORD wIndex[3];
	};

	typedef std::multimap<float,FACE>	FACEMAP;		// x 축 좌표로만 찾아내 보자.
	typedef FACEMAP::iterator			FACEMAP_ITER;

	FACEMAP		m_mapFace;

protected:
	DWORD		m_dwCol;		// 열 갯수
	DWORD		m_dwRow;		// 행 갯수
	LPDIRECT3DTEXTURE8		m_pTexture;	// BaseTex

protected:
	struct SEPARATE_PROPERTY
	{
		D3DXVECTOR3		vMax;
		D3DXVECTOR3		vMin;
		VERTEX_TEX2		vVertex[3];
	};

	class SEPARATEOBJ : public OBJAABB
	{
	public:										// 저장하는것
		union
		{
			struct
			{
				SEPARATE_PROPERTY	sSeparateProp;
			};
			struct
			{
				D3DXVECTOR3		vMax;
				D3DXVECTOR3		vMin;
				VERTEX_TEX2		vVertex[3];
			};
		};

		DWORD				dwPointEx[3];

		SEPARATEOBJ*		pNext;

		SEPARATEOBJ () :
			vMax(0,0,0),
			vMin(0,0,0),
			pNext(0)
		{
			for ( DWORD i=0; i<3; ++i )
			{
				dwPointEx[0] = 0;

				vVertex[i].vPos = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
				vVertex[i].vNor = D3DXVECTOR3 ( 0.f, 1.f, 0.f );
				vVertex[i].vTex1 = D3DXVECTOR2 ( 0.f, 0.f );
				vVertex[i].vTex2 = D3DXVECTOR2 ( 0.f, 0.f );
			}
		}

		~SEPARATEOBJ ()
		{
		}

		virtual void GetAABBSize ( D3DXVECTOR3 &_vMax, D3DXVECTOR3 &_vMin )
		{
			_vMax = vMax; _vMin = vMin;
		}

		virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
		{
			return COLLISION::IsWithInPoint ( vDivMax, vDivMin, (vMax+vMin)/2 );
		}

		virtual OBJAABB* GetNext () { return pNext; }

		virtual HRESULT Render ( LPDIRECT3DDEVICE8 pd3dDevice, CLIPVOLUME *pCV );

	public:
		void		SaveFile ( CSerialFile &SFile );
		void		LoadFile ( CSerialFile &SFile );
	};
	typedef SEPARATEOBJ*	PSEPARATEOBJ;

protected:
	LPDIRECT3DTEXTURE8		m_pDetailTex;

protected:
	virtual HRESULT AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICE8 pd3dDevice );
public:
	virtual HRESULT AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICE8 pd3dDevice );
	virtual HRESULT AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICE8 pd3dDevice );

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICE8 pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICE8 pd3dDevice );

public:
	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice );

	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICE8 pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );
 
protected:
	//	생	성	부	분
	void	SetBaseQuad ( LPDIRECT3DDEVICE8 pd3dDevice, DxMeshes *pmsMeshs );
	void	SortTriangle ( VERTEX* pSrcVertices, WORD* pSrcIndices );
	void	SetBaseInit ( LPDIRECT3DDEVICE8 pd3dDevice, VERTEX* pSrcVertices, WORD* pSrcIndices );
	void	SetBaseMap ( int i, int j, VERTEX* pSrcVertices, WORD* pSrcIndices );
	void	SetBase_VB_IB_TEX ( LPDIRECT3DDEVICE8 pd3dDevice );


	BOOL		Picking ( VERTEX* pVertices, VERTEX& vPos );
	void		ModifyNormal ( DWORD x, DWORD y );
	D3DXVECTOR3	ModifyNormalPart ( int x, int y, int x1, int y1, int x2, int y2 );

	//	렌	더	부	분
	void	RenderTiles ( LPDIRECT3DDEVICE8 pd3dDevice );
public:
	DxEffectTile ();
	virtual ~DxEffectTile ();

	//	Note : OBJAABB
	//
public:
	virtual void GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );

protected:
	virtual void SaveBuffer ( CSerialFile &SFile );
	virtual void LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICE8 pd3dDevice );
	virtual void LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICE8 pd3dDevice );

	//	Note : DxFrameMesh 의 효과를 DxLandMan 에 삽입 할 때 대량의 버퍼 데이타를
	//		복제하기 위해서 사용한다.
public:
	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICE8 pd3dDevice );
};

#endif // !defined(__DxEffectTile__INCLUDED__)

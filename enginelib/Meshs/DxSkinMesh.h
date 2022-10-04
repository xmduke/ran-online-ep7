// DxSkinMesh.h: interface for the DxSkinMesh class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "SMeshContainer.h"

#define __FRAMEROOT ("__ROOT")

struct SFrame
{
	SMeshContainer	*pmcMesh;		//	메쉬 콘테이너 트리.

	D3DXMATRIXA16	matRot;			//	에니메이션 트랜스폼 메트릭스.
	D3DXMATRIXA16	matRotOrig;		//	원본 트랜스폼 메트릭스.
	D3DXMATRIXA16	matCombined;	//	계층 트랜스폼 메트릭스 합성된 것.

	SFrame			*pframeSibling;		//	형제 노드들.
	SFrame			*pframeFirstChild;	//	자식 노드들.

	char			*szName;			//	오브젝트 이름.

	SFrame ()
		:
			pmcMesh(NULL),
			pframeSibling(NULL),
			pframeFirstChild(NULL),
			szName(NULL)
	{
		D3DXMatrixIdentity(&matRot);
		D3DXMatrixIdentity(&matRotOrig);
	}

	~SFrame();

	SFrame *FindFrame ( char *szFrame )
	{
		SFrame *pframe;

		if ( (szName != NULL) && (strcmp(szName, szFrame) == 0) )
			return this;

		if ( pframeFirstChild != NULL )
		{
			pframe = pframeFirstChild->FindFrame ( szFrame );
			if ( pframe != NULL )	return pframe;
        }

		if ( pframeSibling != NULL )
		{
			pframe = pframeSibling->FindFrame ( szFrame );
			if ( pframe != NULL )		return pframe;
		}

		return NULL;
	}

	SFrame* FindParentFrame ( SFrame* pFindFrame )
	{
		SFrame *pframeChild = pframeFirstChild;
		while ( pframeChild != NULL )
		{
			if ( pframeChild == pFindFrame )	return this;

			SFrame *pframe = pframeChild->FindParentFrame ( pFindFrame );
			if ( pframe != NULL ) return pframe;

			pframeChild = pframeChild->pframeSibling;
		}

		return NULL;
	}

	void ResetMatrix ()
	{
		matRot = matRotOrig;

		if ( pframeFirstChild != NULL )
		{
			pframeFirstChild->ResetMatrix ();
		}

		if ( pframeSibling != NULL )
		{
			pframeSibling->ResetMatrix ();
		}
	}

	void AddFrame ( SFrame *pframe )
	{
		if ( pframeFirstChild == NULL )
		{
			pframeFirstChild = pframe;
		}
		else
		{
			pframe->pframeSibling = pframeFirstChild->pframeSibling;
			pframeFirstChild->pframeSibling = pframe;
		}
	}

	void AddMesh ( SMeshContainer *pmc )
	{
		pmc->m_pmcNext = pmcMesh;
		pmcMesh = pmc;
	}
};

struct DxSkeleton;
struct SMATERIAL_PIECE;

class DxSkinMesh
{
public:
	static METHOD		m_method;						//	스키닝 에니메이션 종류


protected:
	LPDIRECT3DDEVICE8	m_pd3dDevice;
	DWORD				m_dwD3dMaxVertexBlendMatrices;
	DWORD				m_dwFVF;						//	FVF 설정.

protected:
	TCHAR				m_szName[MAX_PATH];				//	형상 xFile 이름.
	TCHAR				m_szSkeleton[MAX_PATH];			//	Skeleton xFile 이름.

	//	Note : 형상 정보.
	//
	SFrame*				pframeRoot;						//	프레임 트리.
	DxSkeleton*			pSkeleton;
	D3DXVECTOR3			vCenter;						//	물체 중심.
	float				fRadius;						//	물체 반경.

	SMeshContainer*		pMeshGlobalHead;				//	메쉬 콘테이너의 전체 리스트.

	//	Note : SoftWare 용 임시 배열.
	//
	LPD3DXMATRIX		m_pBoneMatrices;				//	본의 메트릭스 주소.
	DWORD				m_maxBones;						//	본의 갯수.

//protected:
//	static DWORD		dwSkinmesh1VertexShader[];
//	static DWORD		dwSkinmesh2VertexShader[];
//	static DWORD		dwSkinmesh3VertexShader[];
//	static DWORD		dwSkinmesh4VertexShader[];
//	static DWORD		dwSkinmeshPixelShader[];
//	static DWORD		m_dwIndexedVS[4];				// Shader 용	범프(DOT3)
//	static DWORD		m_dwIndexedPS;					// PS
//
//	static DWORD		m_dwTestVS[4];					// Test 용
//	static DWORD		m_dwTestPS;						// Test 용
//	LPDIRECT3DTEXTURE8	m_pTestTex;						// Test 용

public:
	DWORD				dwRefCount;

protected:
	BOOL		m_bOriginDraw;
	BOOL		m_bAlpha;
	BOOL		m_bShadow;
	BOOL		m_bAlphaTex;
	BOOL		m_bLevel;
	float		m_fHeight;

public:
	D3DXMATRIX	m_matWorld;

public:
	SFrame* GetFrameRoot ()					{ return pframeRoot; }
	TCHAR* GetSkinFileName ()				{ return m_szName; }

	D3DXVECTOR3 GetCenter ()				{ return vCenter; }
	float GetRadius ()						{ return fRadius; }

	SMeshContainer* GetMeshClobalList ()	{ return pMeshGlobalHead; }
	
public:
	SFrame* FindFrame ( char *szName )
	{
		if ( pframeRoot == NULL )	return NULL;
		else						return pframeRoot->FindFrame ( szName );
	}

	SFrame* FindParentFrame ( SFrame* pFrame )
	{
		if ( pframeRoot == NULL || pFrame == NULL )	return NULL;
		else
		{
			return pframeRoot->FindParentFrame ( pFrame );
		}
	}

	SMeshContainer* FindMeshContainer ( char *szName );

public:
	void AddMeshGlobal ( SMeshContainer *pmc )
	{
		pmc->m_pmcGlobalNext = pMeshGlobalHead;
		pMeshGlobalHead = pmc;
	}

public:
	void ResetTransform () { pframeRoot->ResetMatrix (); }

public:
	HRESULT LoadMeshHierarchy ( char *szFile, char *szSkeletonFile, LPDIRECT3DDEVICE8 pd3dDevice, const BOOL bThread );
	HRESULT ClearMeshHierarchy ();

protected:
	HRESULT FindBones ( SFrame *pframeCur );
	HRESULT LoadFrames ( LPDIRECTXFILEDATA pxofobjCur,
					DWORD options, DWORD fvf, LPDIRECT3DDEVICE8 pD3DDevice,
					SFrame *pframeParent, const BOOL bThread );
	HRESULT LoadMesh ( LPDIRECTXFILEDATA pxofobjCur,
					DWORD options, DWORD fvf, LPDIRECT3DDEVICE8 pD3DDevice,
					SFrame *pframeParent, const BOOL bThread );
public:
	void	Init_Detail_Texture ();

	void	SetDrawState ( BOOL bOriginDraw=TRUE, BOOL bAlpha=FALSE, BOOL bShadow=FALSE, BOOL bAlphaTex=TRUE, BOOL bLevel=FALSE, float fHeight=0.f );

	HRESULT DrawMeshContainer ( LPDIRECT3DDEVICE8 pd3dDevice, SMeshContainer *pmcMesh, SMATERIAL_PIECE *pmtrlPiece=NULL, DXMATERIAL_CHAR_EFF* pmtrlSpecular=NULL );

protected:
	HRESULT RenderDefault ( LPDIRECT3DDEVICE8 pd3dDevice, SMeshContainer *pmcMesh, int ipattr, DWORD dwThisAttribId, 
							SMATERIAL_PIECE *pmtrlPiece, DXMATERIAL_CHAR_EFF* pmtrlSpecular );
	HRESULT RenderVS ( LPDIRECT3DDEVICE8 pd3dDevice, SMeshContainer *pmcMesh, LPD3DXBONECOMBINATION pBoneComb, int ipattr, 
						DWORD dwThisAttribId, SMATERIAL_PIECE *pmtrlPiece, DXMATERIAL_CHAR_EFF* pmtrlSpecular );

public:
	HRESULT UpdateFrames ( SFrame *pframeCur, D3DXMATRIX &matCur );
	HRESULT DrawFrames ( LPDIRECT3DDEVICE8 pd3dDevice, SFrame *pframeCur, UINT &cTriangles );
	HRESULT DrawMeshClobal ( LPDIRECT3DDEVICE8 pd3dDevice, UINT &cTriangles );

public:
	HRESULT UpdateEffMesh ( LPDIRECT3DDEVICE8 pd3dDevice, SMeshContainer* pmcMesh );

public:
	HRESULT CalculateBoundingSphere ();
	HRESULT CalculateBoundingBox ( LPDIRECT3DDEVICE8 pd3dDevice, SMeshContainer* pmcMesh, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice );
	HRESULT InvalidateDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice );
	HRESULT DeleteDeviceObjects();

	HRESULT FrameMove ( float fElapsedTime );
	HRESULT Render ( D3DXMATRIX matRot, LPDIRECT3DDEVICE8 pd3dDevice );

public:
	DxSkinMesh ();
	virtual ~DxSkinMesh ();
};

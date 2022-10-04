// Terrain0.cpp: implementation of the DxEffectTiling class.
//
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "TextureManager.h"
#include "EditMeshs.h"
#include "DxMaterial.h"
#include "DxFrameMesh.h"
#include "DxEffectDefine.h"

#include "SerialFile.h"
#include "Collision.h"

#include "DxInputDevice.h"

#include "DxEffectMan.h"

#include "DxEffectTile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW
//	---------------------------------------------------------------------------------------------------
//										이 함수의 초기화와 삭제 부분
//	---------------------------------------------------------------------------------------------------
DxEffectTile::BASE::~BASE()
{
	SAFE_RELEASE ( pTexture );

	SAFE_RELEASE ( pVB );
	SAFE_RELEASE ( pIB );
}

//	---------------------------------------------------------------------------------------------------
//											생	성	부	분
//	---------------------------------------------------------------------------------------------------
void	DxEffectTile::SetBaseQuad ( LPDIRECT3DDEVICE8 pd3dDevice, DxMeshes *pmsMeshs )
{
	m_pTexture = pmsMeshs->pTextures[0];

	//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
	//
	if ( !pmsMeshs->m_pLocalMesh )	return;

	m_dwVertices	= pmsMeshs->m_pLocalMesh->GetNumVertices ( );
	m_dwFaces		= pmsMeshs->m_pLocalMesh->GetNumFaces ( );

	VERTEX*			pSrcVertices;
	WORD*			pSrcIndices;

	pmsMeshs->m_pLocalMesh->LockVertexBuffer ( 0L, (BYTE**)&pSrcVertices );
	pmsMeshs->m_pLocalMesh->LockIndexBuffer ( 0L, (BYTE**)&pSrcIndices );

	// 삼각형을 정렬한다.
	SortTriangle ( pSrcVertices, pSrcIndices );
	
	// 먼저 기본 틀을 만든다.
	SetBaseInit ( pd3dDevice, pSrcVertices, pSrcIndices );

	pmsMeshs->m_pLocalMesh->UnlockVertexBuffer ();
	pmsMeshs->m_pLocalMesh->UnlockIndexBuffer ();
}

void	DxEffectTile::SortTriangle ( VERTEX* pSrcVertices, WORD* pSrcIndices )
{
	FACE		sFace;
	VERTEX		vVertex[3];
	D3DXVECTOR3	vMin;
	for ( DWORD i=0; i<m_dwFaces; ++i )
	{
		vVertex[0] = pSrcVertices [ pSrcIndices[(i*3)+0] ];
		vVertex[1] = pSrcVertices [ pSrcIndices[(i*3)+1] ];
		vVertex[2] = pSrcVertices [ pSrcIndices[(i*3)+2] ];

		vMin = vVertex[0].vPos;	// 초기값 셋팅
		vMin.x = (vVertex[1].vPos.x < vMin.x) ? vVertex[1].vPos.x : vMin.x ;
		vMin.x = (vVertex[2].vPos.x < vMin.x) ? vVertex[2].vPos.x : vMin.x ;

		sFace.wIndex[0] = pSrcIndices[(i*3)+0];
		sFace.wIndex[1] = pSrcIndices[(i*3)+1];
		sFace.wIndex[2] = pSrcIndices[(i*3)+2];

		m_mapFace.insert ( std::make_pair(vMin.x,sFace) );
	}
}

void	DxEffectTile::SetBaseInit ( LPDIRECT3DDEVICE8 pd3dDevice, VERTEX* pSrcVertices, WORD* pSrcIndices )
{
	m_vMax = m_vMin = pSrcVertices[0].vPos;	// 초기값 셋팅

	for ( DWORD i=0; i<m_dwVertices; i++ )
	{
		m_vMax.x = (pSrcVertices[i].vPos.x > m_vMax.x) ? pSrcVertices[i].vPos.x : m_vMax.x ;
		m_vMax.z = (pSrcVertices[i].vPos.z > m_vMax.z) ? pSrcVertices[i].vPos.z : m_vMax.z ;

		m_vMin.x = (pSrcVertices[i].vPos.x < m_vMin.x) ? pSrcVertices[i].vPos.x : m_vMin.x ;
		m_vMin.z = (pSrcVertices[i].vPos.z < m_vMin.z) ? pSrcVertices[i].vPos.z : m_vMin.z ;
	}
	
	D3DXMATRIX	matWorld;
	D3DXMatrixMultiply ( &matWorld, m_pmatLocal, &m_matFrameComb );
	D3DXVec3TransformCoord ( &m_vMax, &m_vMax, &matWorld );			// 최종 데이터 m_vMax
	D3DXVec3TransformCoord ( &m_vMin, &m_vMin, &matWorld );			// 최종 데이터 m_vMin

	//	Col 과 Row 갯수를 찾는다.
	m_dwCol = 0;
	m_dwRow = 0;
	float fLength = m_vMax.x - m_vMin.x;
	while ( fLength > 0.f )
	{
		++m_dwCol;
		fLength -= EM_FULL_LENGTH;
	}

	fLength = m_vMax.z - m_vMin.z;
	while ( fLength > 0.f )
	{
		++m_dwRow;
		fLength -= EM_FULL_LENGTH;
	}

	//	드디어 만든다. 하하하.^^
	for ( DWORD i=0; i<m_dwCol; ++i )
	{
		for ( DWORD j=0; j<m_dwRow; ++j )
		{
			SetBaseMap ( i, j, pSrcVertices, pSrcIndices );
		}
	}

	//	위치 정보 재 수정

	//	VB, IB, TEXTURE
	SetBase_VB_IB_TEX ( pd3dDevice );
};

void	DxEffectTile::SetBaseMap ( int nCol, int nRow, VERTEX* pSrcVertices, WORD* pSrcIndices )
{
	BASE* pBase = new BASE;

	pBase->vMin.x = m_vMin.x + (EM_FULL_LENGTH*nCol);
	pBase->vMin.z = m_vMin.z + (EM_FULL_LENGTH*nRow);
	pBase->vMax.x = pBase->vMin.x + EM_FULL_LENGTH;
	pBase->vMax.z = pBase->vMin.z + EM_FULL_LENGTH;

	// 기본 Vertex 데이터 삽입, Picking으로 높이 조절
	for ( DWORD y=0; y<EM_VERTEX_PART; ++y )
	{
		for ( DWORD x=0; x<EM_VERTEX_PART; ++x )
		{
			m_sVertex[x][y].vPos.x	= pBase->vMin.x + (x*EM_LENGTH);
			m_sVertex[x][y].vPos.y	= 0.f;
			m_sVertex[x][y].vPos.z	= pBase->vMin.z + (y*EM_LENGTH);
			m_sVertex[x][y].vNor	= D3DXVECTOR3 ( 0.f, 1.f, 0.f );
			m_sVertex[x][y].vTex	= D3DXVECTOR2 ( 0.f, 0.f );

			Picking ( pSrcVertices, m_sVertex[x][y] );
		}
	}

	// Normal 값 수정
	for ( DWORD y=0; y<EM_VERTEX_PART; ++y )
	{
		for ( DWORD x=0; x<EM_VERTEX_PART; ++x )
		{
			ModifyNormal ( x, y );
		}
	}

	// Last
	for ( DWORD y=0; y<EM_GRID; ++y )
	{
		for ( DWORD x=0; x<EM_GRID; ++x )
		{
			pBase->sTile[x][y].dwTile = 0;

			float fRandX = RANDOM_POS;			// TEST
			float fRandY = RANDOM_POS;			// TEST

			pBase->sTile[x][y].sVertex[0].vPos	= m_sVertex[x][y].vPos;
			pBase->sTile[x][y].sVertex[0].vNor	= m_sVertex[x][y].vNor;
			pBase->sTile[x][y].sVertex[0].vTex1 = m_sVertex[x][y].vTex;
			pBase->sTile[x][y].sVertex[0].vTex2	= D3DXVECTOR2 ( fRandX, fRandY );

			pBase->sTile[x][y].sVertex[1].vPos	= m_sVertex[x+1][y].vPos;
			pBase->sTile[x][y].sVertex[1].vNor	= m_sVertex[x+1][y].vNor;
			pBase->sTile[x][y].sVertex[1].vTex1 = m_sVertex[x+1][y].vTex;
			pBase->sTile[x][y].sVertex[1].vTex2	= D3DXVECTOR2 ( fRandX+0.5f, fRandY );

			pBase->sTile[x][y].sVertex[2].vPos	= m_sVertex[x][y+1].vPos;
			pBase->sTile[x][y].sVertex[2].vNor	= m_sVertex[x][y+1].vNor;
			pBase->sTile[x][y].sVertex[2].vTex1 = m_sVertex[x][y+1].vTex;
			pBase->sTile[x][y].sVertex[2].vTex2	= D3DXVECTOR2 ( fRandX, fRandY+0.5f );

			pBase->sTile[x][y].sVertex[3].vPos	= m_sVertex[x+1][y+1].vPos;
			pBase->sTile[x][y].sVertex[3].vNor	= m_sVertex[x+1][y+1].vNor;
			pBase->sTile[x][y].sVertex[3].vTex1 = m_sVertex[x+1][y+1].vTex;
			pBase->sTile[x][y].sVertex[3].vTex2	= D3DXVECTOR2 ( fRandX+0.5f, fRandY+0.5f );
		}
	}

	DWORD	dwGrid = (nCol*10000) + nRow;
	m_mapBase.insert ( std::make_pair(dwGrid,pBase) );
}

BOOL	DxEffectTile::Picking ( VERTEX* pVertices, VERTEX& vVertex )
{
	D3DXVECTOR3 v0, v1, v2;
	FLOAT fBary1, fBary2;
	FLOAT fDist;

	D3DXVECTOR3 vPickRayDir		= D3DXVECTOR3 ( 0.f, 100000.f, 0.f );
	D3DXVECTOR3 vPickRayOrig	= vVertex.vPos - (vPickRayDir*0.5f);
	
	FACEMAP_ITER iter = m_mapFace.lower_bound ( vVertex.vPos.x-EM_LENGTH );
	for ( ; iter!=m_mapFace.end(); ++iter )
	{
		v0 = pVertices [ (*iter).second.wIndex[0] ].vPos;
		v1 = pVertices [ (*iter).second.wIndex[1] ].vPos;
		v2 = pVertices [ (*iter).second.wIndex[2] ].vPos;

		if( DxEffectMan::Instance.IntersectTriangle( vPickRayOrig, vPickRayDir, v0, v1, v2,
								&fDist, &fBary1, &fBary2 ) )
		{
			vVertex.vPos = v0 + ((v1-v0)*fBary1) + ((v2-v0)*fBary2);		// Position 수정

			D3DXVECTOR2 vT0, vT1, vT2;
			vT0 = pVertices [ (*iter).second.wIndex[0] ].vTex;
			vT1 = pVertices [ (*iter).second.wIndex[1] ].vTex;
			vT2 = pVertices [ (*iter).second.wIndex[2] ].vTex;
			vVertex.vTex = vT0 + ((vT1-vT0)*fBary1) + ((vT2-vT0)*fBary2);	// UV 좌표 수정

			return TRUE;
		}
		else if ( v0.x > (vVertex.vPos.x + EM_LENGTH) )
		{
			return FALSE;
		}
	}

	return FALSE;
}

void	DxEffectTile::ModifyNormal ( DWORD x, DWORD y )
{
	m_sVertex[x][y].vNor = D3DXVECTOR3 ( 0.f, 0.f, 0.f );

	m_sVertex[x][y].vNor += ModifyNormalPart ( x,y, 1,-1,	0,-1 );		// 1
	m_sVertex[x][y].vNor += ModifyNormalPart ( x,y, 1,0,	1,-1 );		// 2
	m_sVertex[x][y].vNor += ModifyNormalPart ( x,y, 1,1,	1,0 );		// 3
	m_sVertex[x][y].vNor += ModifyNormalPart ( x,y, 0,1,	1,1 );		// 4
	m_sVertex[x][y].vNor += ModifyNormalPart ( x,y, -1,1,	0,1 );		// 5
	m_sVertex[x][y].vNor += ModifyNormalPart ( x,y, -1,0,	-1,1 );		// 6
	m_sVertex[x][y].vNor += ModifyNormalPart ( x,y, -1,-1,	-1,0 );		// 7
	m_sVertex[x][y].vNor += ModifyNormalPart ( x,y, 0,-1,	-1,-1 );	// 8

	D3DXVec3Normalize ( &m_sVertex[x][y].vNor, &m_sVertex[x][y].vNor );
}

D3DXVECTOR3	DxEffectTile::ModifyNormalPart ( int x, int y, int x1, int y1, int x2, int y2 )
{
	D3DXVECTOR3 vNormal (0.f, 0.f, 0.f);
	D3DXVECTOR3	vLine[2];

	int nX, nY;
	nX = x+x1;
	nY = y+y1;
	if ( (nX<=-1) || (nX>=EM_VERTEX_PART) || (nY<=-1) || (nY>=EM_VERTEX_PART) )	return vNormal;
	vLine[0] = m_sVertex[nX][nY].vPos - m_sVertex[x][y].vPos;

	nX = x+x2;
	nY = y+y2;
	if ( (nX<=-1) || (nX>=EM_VERTEX_PART) || (nY<=-1) || (nY>=EM_VERTEX_PART) )	return vNormal;
	vLine[1] = m_sVertex[nX][nY].vPos - m_sVertex[x][y].vPos;

	D3DXVec3Cross ( &vNormal, &vLine[0], &vLine[1] );
	D3DXVec3Normalize ( &vNormal, &vNormal );

	if ( vNormal.y < 0.f )	vNormal *= -1.f;	// 밑을 바라본다면 반대로 방향을 바꾼다.

	return vNormal;
}

void	DxEffectTile::SetBase_VB_IB_TEX ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	DWORD			dwCol;
	DWORD			dwRow;
	VERTEX_TEX2*	pVertices;
	WORD*			pIndices;
	BASEMAP_ITER iter = m_mapBase.begin();
	for ( ; iter!=m_mapBase.end(); ++iter )
	{
		SAFE_RELEASE ( (*iter).second->pVB );
		SAFE_RELEASE ( (*iter).second->pIB );
		SAFE_RELEASE ( (*iter).second->pTexture );

		pd3dDevice->CreateVertexBuffer ( 4*EM_GRID*EM_GRID*sizeof(VERTEX_TEX2), D3DUSAGE_WRITEONLY, VERTEX_TEX2::FVF, D3DPOOL_MANAGED, &(*iter).second->pVB );
		(*iter).second->pVB->Lock ( 0, 0, (BYTE**)&pVertices, 0 );
		for ( DWORD y=0; y<EM_GRID; ++y )
		{
			for ( DWORD x=0; x<EM_GRID; ++x )
			{
				for ( DWORD i=0; i<4; ++i )
				{
					pVertices[(y*EM_GRID*4)+(x*4)+i].vPos	= (*iter).second->sTile[x][y].sVertex[i].vPos;
					pVertices[(y*EM_GRID*4)+(x*4)+i].vNor	= (*iter).second->sTile[x][y].sVertex[i].vNor;
					pVertices[(y*EM_GRID*4)+(x*4)+i].vTex1	= (*iter).second->sTile[x][y].sVertex[i].vTex1;
					pVertices[(y*EM_GRID*4)+(x*4)+i].vTex2	= (*iter).second->sTile[x][y].sVertex[i].vTex2;
				}
			}
		}
		(*iter).second->pVB->Unlock ();

		dwCol = 0;
		dwRow = 0;
		pd3dDevice->CreateIndexBuffer ( 3*EM_FACE*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &(*iter).second->pIB );
		(*iter).second->pIB->Lock ( 0, 0, (BYTE**)&pIndices, 0 );
		DWORD	dwAdd;
		for ( DWORD y=0; y<EM_GRID; ++y )
		{
			for ( DWORD x=0; x<EM_GRID; ++x )
			{
				dwAdd = (x*4) + (y*EM_GRID*4);
				*pIndices++ = (WORD)(2 + dwAdd);
				*pIndices++ = (WORD)(3 + dwAdd);
				*pIndices++ = (WORD)(0 + dwAdd);

				*pIndices++ = (WORD)(3 + dwAdd);
				*pIndices++ = (WORD)(1 + dwAdd);
				*pIndices++ = (WORD)(0 + dwAdd);
			}
		}
		(*iter).second->pIB->Unlock ();

		//D3DXCreateTexture ( pd3dDevice, EM_TEXSIZE, EM_TEXSIZE, 1, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &((*iter).second->pTexture) );

		//DWORD			dwBPP = 32;
		//D3DLOCKED_RECT	lrDst;
		//RECT			pRect;
		//pRect.left		= 0;
		//pRect.top		= 0; 
		//pRect.right		= EM_TEXSIZE-1;
		//pRect.bottom	= EM_TEXSIZE-1;
		//(*iter).second->pTexture->LockRect ( 0, &lrDst, &pRect, 0 );
		//DWORD* pDst       = (DWORD*)lrDst.pBits;

		//for( DWORD y=0; y<EM_TEXSIZE; y++ )
		//{
		//	for( DWORD x=0; x<EM_TEXSIZE; x++ )
		//	{
		//		float fRand = RANDOM_POS;			// TEST
		//		if ( fRand > 0.5f )
		//		{
		//			DWORD dwColor = 70+(DWORD)(100*fRand);
		//			pDst[x] = 0x00330033;
		//			pDst[x] += dwColor<<8;
		//		}
		//		else
		//		{
		//			DWORD dwColor = 80+(DWORD)(127*fRand);
		//			pDst[x] = (dwColor<<16) + (dwColor<<8) + (dwColor);
		//		}

		//		//pDst[x] = (*iter).second->sColor[x][y];
		//	}
		//	pDst = pDst+(lrDst.Pitch/(dwBPP/8));
		//}

		//(*iter).second->pTexture->UnlockRect(0);
	}
}

//	---------------------------------------------------------------------------------------------------
//									에	디	트	부	분			( Interface )
//	---------------------------------------------------------------------------------------------------


//	---------------------------------------------------------------------------------------------------
//									에	디	트	부	분			( Engine )
//	---------------------------------------------------------------------------------------------------


//	---------------------------------------------------------------------------------------------------
//									삼	각	형	생	성	부	분
//	---------------------------------------------------------------------------------------------------


//	---------------------------------------------------------------------------------------------------
//										렌	더	부	분
//	---------------------------------------------------------------------------------------------------
void DxEffectTile::RenderTiles ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	D3DMATERIAL8 sMaterial;
	sMaterial.Diffuse.r = 1.f;
	sMaterial.Diffuse.g = 1.f;
	sMaterial.Diffuse.b = 1.f;
	sMaterial.Diffuse.a = 1.f;
	sMaterial.Ambient.r = 1.f;
	sMaterial.Ambient.g = 1.f;
	sMaterial.Ambient.b = 1.f;
	sMaterial.Ambient.a = 1.f;
	sMaterial.Emissive.r = 0.f;
	sMaterial.Emissive.g = 0.f;
	sMaterial.Emissive.b = 0.f;
	sMaterial.Emissive.a = 0.f;
	sMaterial.Specular.r = 0.f;
	sMaterial.Specular.g = 0.f;
	sMaterial.Specular.b = 0.f;
	sMaterial.Specular.a = 0.f;
	pd3dDevice->SetMaterial ( &sMaterial );

	pd3dDevice->CaptureStateBlock ( m_dwSavedSB_Detail );
	pd3dDevice->ApplyStateBlock ( m_dwDrawSB_Detail );

	pd3dDevice->SetTexture ( 1, m_pDetailTex );

	BASEMAP_ITER iter = m_mapBase.begin();
	for ( ; iter!=m_mapBase.end(); ++iter )
	{
		pd3dDevice->SetVertexShader ( VERTEX_TEX2::FVF );
		pd3dDevice->SetStreamSource ( 0, (*iter).second->pVB, sizeof(VERTEX_TEX2) );
		pd3dDevice->SetIndices ( (*iter).second->pIB, 0 );

		//pd3dDevice->SetTexture ( 0, (*iter).second->pTexture );
		pd3dDevice->SetTexture ( 0, m_pTexture );

		pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, EM_VERTEX_ALL, 0, EM_FACE );

		pd3dDevice->SetTexture ( 0, NULL );

		pd3dDevice->SetStreamSource ( 0, NULL, 0 );
		pd3dDevice->SetIndices ( NULL, 0 );
	}

	pd3dDevice->SetTexture ( 1, NULL );

	pd3dDevice->ApplyStateBlock ( m_dwSavedSB_Detail );
}

//	---------------------------------------------------------------------------------------------------
//							(	S	A	V	E	)	&	(	L	O	A	D	)
//	---------------------------------------------------------------------------------------------------

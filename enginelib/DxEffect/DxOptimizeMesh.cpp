#include "stdafx.h"
#include "DxViewPort.h"
#include "SerialFile.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

#include "TextureManager.h"
#include "StlFunctions.h"

#include "NsOCTree.h"

#include "DxFrameMesh.h"
#include "DxOptimizeMesh.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x		O	p	t	i	m	i	z	e		M	e	s	h
// -----------------------------------------------------------------------------------------------------------------------------------------
DxOptimizeMesh::~DxOptimizeMesh()
{
	m_dwFace = 0L;
	SAFE_DELETE_ARRAY ( m_pVertex );

	if ( m_pTexture )	
	{
		TextureManager::ReleaseTexture ( m_pTexture );
		m_pTexture = NULL;
	}

	SAFE_DELETE ( m_pOcTree );

	SAFE_DELETE ( m_pNext );		// 계속 없앤다.
}

void DxOptimizeMesh::Create ( LPDIRECT3DDEVICE8 pd3dDevice, const char* szName, DWORD dwFaceNUM, VERTEX* pVertices )
{
	if ( m_pTexture )
	{
		TextureManager::ReleaseTexture ( m_pTexture );
		m_pTexture = NULL;
	}
	if ( strcmp(szName,"") )	TextureManager::LoadTexture ( szName, pd3dDevice, m_pTexture, 0L, 0L );

	if ( !pVertices )	return;

	m_dwFace = dwFaceNUM;
	m_pVertex = new VERTEX[m_dwFace*3];
	memcpy ( m_pVertex, pVertices, sizeof(VERTEX)*m_dwFace*3 );

	for ( DWORD i=0; i<m_dwFace*3; ++i )
	{
		if ( m_vMax.x < pVertices[i].vPos.x )	m_vMax.x = pVertices[i].vPos.x;
		if ( m_vMax.y < pVertices[i].vPos.y )	m_vMax.y = pVertices[i].vPos.y;
		if ( m_vMax.z < pVertices[i].vPos.z )	m_vMax.z = pVertices[i].vPos.z;

		if ( m_vMin.x > pVertices[i].vPos.x )	m_vMin.x = pVertices[i].vPos.x;
		if ( m_vMin.y > pVertices[i].vPos.y )	m_vMin.y = pVertices[i].vPos.y;
		if ( m_vMin.z > pVertices[i].vPos.z )	m_vMin.z = pVertices[i].vPos.z;
	}
}

void DxOptimizeMesh::MakeOptimizeMesh ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	NSOPTIMIZEMESH::MakeOctreeMesh ( pd3dDevice, m_pVertex, m_dwFace, m_pOcTree );
}

HRESULT DxOptimizeMesh::Render ( LPDIRECT3DDEVICE8 pd3dDevice, CLIPVOLUME *pCV )
{
	pd3dDevice->SetTexture ( 0, m_pTexture );
	NSOCTREE::RenderOctree ( pd3dDevice, pCV, m_pOcTree );

	return S_OK;
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//										N	S		O	P	T	I	M	I	Z	E		M	E	S	H
// -----------------------------------------------------------------------------------------------------------------------------------------
namespace NSOPTIMIZEMESH
{
	struct VERTEX_S
	{
		DWORD		dwParent;
		D3DXVECTOR3 vPos; 
		D3DXVECTOR3 vNor; 
		D3DXVECTOR2 vTex;
	};

	struct VERTEX_P
	{
		DWORD		dwNum;
		D3DXVECTOR3 vPos; 
		D3DXVECTOR3 vNor; 
		D3DXVECTOR2 vTex;

		BOOL operator==(const VERTEX_S& sVertex)  const;
		friend BOOL operator==(const VERTEX_S& sVertex, const VERTEX_P& sVertex_P);
	};

	BOOL VERTEX_P::operator==(const VERTEX_S& sVertexS) const
	{
		if ( vPos.x+0.01f < sVertexS.vPos.x )	return FALSE;
		if ( vPos.y+0.01f < sVertexS.vPos.y )	return FALSE;
		if ( vPos.z+0.01f < sVertexS.vPos.z )	return FALSE;
		if ( vPos.x > sVertexS.vPos.x+0.01f )	return FALSE;
		if ( vPos.y > sVertexS.vPos.y+0.01f )	return FALSE;
		if ( vPos.z > sVertexS.vPos.z+0.01f )	return FALSE;

		if ( vNor.x+0.01f < sVertexS.vNor.x )	return FALSE;
		if ( vNor.y+0.01f < sVertexS.vNor.y )	return FALSE;
		if ( vNor.z+0.01f < sVertexS.vNor.z )	return FALSE;
		if ( vNor.x > sVertexS.vNor.x+0.01f )	return FALSE;
		if ( vNor.y > sVertexS.vNor.y+0.01f )	return FALSE;
		if ( vNor.z > sVertexS.vNor.z+0.01f )	return FALSE;

		if ( vTex.x+0.01f < sVertexS.vTex.x )	return FALSE;
		if ( vTex.y+0.01f < sVertexS.vTex.y )	return FALSE;
		if ( vTex.x > sVertexS.vTex.x+0.01f )	return FALSE;
		if ( vTex.y > sVertexS.vTex.y+0.01f )	return FALSE;

		return TRUE;
	}

	BOOL operator==(const VERTEX_S& sVertex, const VERTEX_P& sVertex_P)
	{
		return (sVertex_P==sVertex);
	}

	typedef	std::multimap<float,VERTEX_P>			MAPVERTEX;
	typedef	std::multimap<float,VERTEX_P>::iterator	MAPVERTEX_ITER;

	void SetPoint ( MAPVERTEX& mapVertex, VERTEX_S& sVertexS, DWORD& dwIndex )
	{
		MAPVERTEX_ITER iter = mapVertex.lower_bound(sVertexS.vPos.x-0.01f);
		for ( ; iter!=mapVertex.end(); ++iter )
		{
			if ( (*iter).second == sVertexS )
			{
				sVertexS.dwParent  = (*iter).second.dwNum;	// Set Parent
				dwIndex = sVertexS.dwParent;
				return;
			}
			else if ( (*iter).second.vPos.x > (sVertexS.vPos.x+0.01f) )		break;
		}

		VERTEX_P sVertexP;
		sVertexP.dwNum = mapVertex.size();
		sVertexP.vPos = sVertexS.vPos;
		sVertexP.vNor = sVertexS.vNor;
		sVertexP.vTex = sVertexS.vTex;

		sVertexS.dwParent  = sVertexP.dwNum;		// Set Parent
		dwIndex = sVertexS.dwParent;

		mapVertex.insert ( std::make_pair(sVertexP.vPos.x,sVertexP) );
	}

	void MakeOptimizeVertex ( PVERTEX& pVertices, DWORD* pIndices, DWORD& dwVertNUM, const DWORD dwFaceNUM )
	{
		MAPVERTEX	mapVertex;
		MAPVERTEX_ITER iter;

		VERTEX_S* pVertexS = new VERTEX_S[dwVertNUM];
		
		for ( DWORD i=0; i<dwVertNUM; ++i )
		{
			pVertexS[i].dwParent = 0;
			pVertexS[i].vPos = pVertices[i].vPos;
			pVertexS[i].vNor = pVertices[i].vNor;
			pVertexS[i].vTex = pVertices[i].vTex;
		}

		//	Note : 최적화된 Vertex와 Index를 얻는다.
		//
		for ( DWORD i=0; i<dwFaceNUM; ++i )
		{
			SetPoint ( mapVertex, pVertexS[pIndices[i*3+0]], pIndices[i*3+0] );
			SetPoint ( mapVertex, pVertexS[pIndices[i*3+1]], pIndices[i*3+1] );
			SetPoint ( mapVertex, pVertexS[pIndices[i*3+2]], pIndices[i*3+2] );
		}

		// Note : Vertex 데이터를 수정한다.
		dwVertNUM = mapVertex.size();
		SAFE_DELETE_ARRAY ( pVertices );
		pVertices	= new VERTEX[dwVertNUM];
		for ( iter=mapVertex.begin(); iter!=mapVertex.end(); ++iter )
		{
			pVertices [ (*iter).second.dwNum ].vPos = (*iter).second.vPos;
			pVertices [ (*iter).second.dwNum ].vNor = (*iter).second.vNor;
			pVertices [ (*iter).second.dwNum ].vTex = (*iter).second.vTex;
		}

		SAFE_DELETE_ARRAY ( pVertexS );
	}

	void MakeOptimizeMesh ( LPDIRECT3DDEVICE8 pd3dDevice, LPD3DXMESH& pMesh )
	{
		DWORD dwVertNUM = pMesh->GetNumVertices();
		DWORD dwFaceNUM = pMesh->GetNumFaces();
		VERTEX* pNewVertex	= new VERTEX[dwVertNUM];
		DWORD*	pNewIndex	= new DWORD[dwFaceNUM*3];

		// Note : Clone
		VERTEX* pVert;
		pMesh->LockVertexBuffer(0L,(BYTE**)&pVert);	
		memcpy ( pNewVertex, pVert, sizeof(VERTEX)*dwVertNUM );
		pMesh->UnlockVertexBuffer();

		if ( pMesh->GetOptions() & D3DXMESH_32BIT )
		{
			DWORD*	pIndex;
			pMesh->LockIndexBuffer(0L,(BYTE**)&pIndex);
			memcpy ( pNewIndex, pIndex, sizeof(DWORD)*dwFaceNUM*3 );
			pMesh->UnlockIndexBuffer();
		}
		else
		{
			WORD*	pIndex;
			pMesh->LockIndexBuffer(0L,(BYTE**)&pIndex);
			memcpy ( pNewIndex, pIndex, sizeof(WORD)*dwFaceNUM*3 );
			for ( DWORD i=0; i<dwFaceNUM*3; ++i )
			{
				pNewIndex[i] = (DWORD)pIndex[i];
			}
			pMesh->UnlockIndexBuffer();
		}

		// Note : 정보 갱신
		MakeOptimizeVertex( pNewVertex, pNewIndex, dwVertNUM, dwFaceNUM );

		// Note : Mesh 새로 생성
		SAFE_RELEASE ( pMesh );
		if ( dwVertNUM > 65000 )
		{
			D3DXCreateMeshFVF ( dwFaceNUM, dwVertNUM, D3DXMESH_32BIT | D3DXMESH_MANAGED, VERTEX::FVF, pd3dDevice, &pMesh );

			DWORD* pIndices;
			pMesh->LockIndexBuffer ( 0L, (BYTE**)&pIndices );
			memcpy ( pIndices, pNewIndex, sizeof(DWORD)*dwFaceNUM*3 );
			pMesh->UnlockIndexBuffer();
		}
		else
		{
			D3DXCreateMeshFVF ( dwFaceNUM, dwVertNUM, D3DXMESH_MANAGED, VERTEX::FVF, pd3dDevice, &pMesh );

			WORD* pIndices;
			pMesh->LockIndexBuffer ( 0L, (BYTE**)&pIndices );
			for ( DWORD i=0; i<dwFaceNUM*3; ++i )
			{
				pIndices[i] = (WORD)pNewIndex[i];
			}
			pMesh->UnlockIndexBuffer();
		}

		VERTEX* pVertices;
		pMesh->LockVertexBuffer ( 0L, (BYTE**)&pVertices );
		memcpy ( pVertices, pNewVertex, sizeof(VERTEX)*dwVertNUM );
		pMesh->UnlockVertexBuffer();

		SAFE_DELETE_ARRAY ( pNewVertex );
		SAFE_DELETE_ARRAY ( pNewIndex );

		// Note : 최적화 완료
		DWORD* pAdjacency = new DWORD[3*dwFaceNUM];
		pMesh->GenerateAdjacency ( 0.01f, pAdjacency );
		pMesh->OptimizeInplace ( D3DXMESHOPT_VERTEXCACHE, pAdjacency, NULL, NULL, NULL );
		SAFE_DELETE_ARRAY ( pAdjacency );
	}

	void MakeOctreeMesh ( LPDIRECT3DDEVICE8 pd3dDevice, VERTEX* pVertex, DWORD dwFaceNUM, POBJOCTREE &m_pOcTree )
	{
		// Note : Octree 형식으로 분할
		NSOCTREE::MakeOctree ( m_pOcTree, pVertex, dwFaceNUM );

		// Note : OcMesh 만들기
		NSOCTREE::MakeOcMesh ( pd3dDevice, m_pOcTree );

		// Note : OcMesh 최적화
		NSOCTREE::MakeOptimizeMesh ( pd3dDevice, m_pOcTree );
	}
};
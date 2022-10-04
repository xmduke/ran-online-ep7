#include "stdafx.h"

#include <algorithm>
#include "StlFunctions.h"

#include "DxRenderStates.h"

#include "DxLandMan.h"
#include "DxStaticMeshCreate.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x		S	t	a	t	i	c		M	e	s	h
// -----------------------------------------------------------------------------------------------------------------------------------------
DxStaticMesh::DxStaticMesh() :
	m_pMeshList(NULL),
	m_pMeshTree(NULL),
	m_pMeshAlphaList(NULL),
	m_pMeshAlphaTree(NULL)
{
}

DxStaticMesh::~DxStaticMesh()
{
	CleanUp();
}

void DxStaticMesh::CleanUp()
{
	SAFE_DELETE ( m_pMeshTree );
	SAFE_DELETE ( m_pMeshList );

	SAFE_DELETE ( m_pMeshAlphaTree );
	SAFE_DELETE ( m_pMeshAlphaList );
}

void DxStaticMesh::FrameMove ( float fTime, float fElapsedTime )
{
}

void DxStaticMesh::Render( LPDIRECT3DDEVICE8 pd3dDevice, CLIPVOLUME *pCV )
{
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );

	COLLISION::RenderAABBTree ( pd3dDevice, pCV, m_pMeshTree, FALSE );
}

void DxStaticMesh::RenderAlpha( LPDIRECT3DDEVICE8 pd3dDevice, CLIPVOLUME *pCV )
{
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );

	DxRenderStates::Instance.SetOnAlphaMap ( pd3dDevice );
	COLLISION::RenderAABBTree ( pd3dDevice, pCV, m_pMeshAlphaTree, FALSE );
	DxRenderStates::Instance.ReSetOnAlphaMap ( pd3dDevice );
}

void DxStaticMesh::MakeAABBOCTree ( LPDIRECT3DDEVICE8 pd3dDevice, DxFrameMesh* pFrameMesh )
{
	CleanUp();

	if ( pFrameMesh )
	{
		NSSTATICMESH::ExportMaterialList ( pd3dDevice, pFrameMesh, this );

		COLLISION::MakeAABBTree ( m_pMeshTree, m_pMeshList );
		COLLISION::MakeAABBTree ( m_pMeshAlphaTree, m_pMeshAlphaList );
	}
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//										N	S		S	T	A	T	I	C		M	E	S	H
// -----------------------------------------------------------------------------------------------------------------------------------------
namespace NSSTATICMESH
{
	MAPSTATICDATA	mapMeshData;
	MAPSTATICDATA	mapMeshAlpha;

	void MakeStaticMesh3 ( char* szName, MAPSTATICDATA& mapMesh, VERTEX*	pVertices, DWORD* pIndices, DWORD dwFaceStart, DWORD dwFaceCount, D3DXMATRIX& matWorld )
	{
		MAPSTATICDATA_ITER iter = mapMesh.find(szName);
		if ( iter==mapMesh.end() )	// New
		{
			STATICDATA* pData = new STATICDATA;
			pData->dwFaceNUM = dwFaceCount;
			pData->pVertices = new VERTEX[dwFaceCount*3];

			DWORD dwCount=0;

			for ( DWORD i=dwFaceStart; i<dwFaceStart+dwFaceCount; ++i )
			{
				pData->pVertices[dwCount] = pVertices [pIndices[(i*3)+0]];
				D3DXVec3TransformCoord ( &pData->pVertices[dwCount].vPos, &pData->pVertices[dwCount].vPos, &matWorld );
				D3DXVec3TransformNormal ( &pData->pVertices[dwCount].vNor, &pData->pVertices[dwCount].vNor, &matWorld );
				++dwCount;

				pData->pVertices[dwCount] = pVertices [pIndices[(i*3)+1]];
				D3DXVec3TransformCoord ( &pData->pVertices[dwCount].vPos, &pData->pVertices[dwCount].vPos, &matWorld );
				D3DXVec3TransformNormal ( &pData->pVertices[dwCount].vNor, &pData->pVertices[dwCount].vNor, &matWorld );
				++dwCount;

				pData->pVertices[dwCount] = pVertices [pIndices[(i*3)+2]];
				D3DXVec3TransformCoord ( &pData->pVertices[dwCount].vPos, &pData->pVertices[dwCount].vPos, &matWorld );
				D3DXVec3TransformNormal ( &pData->pVertices[dwCount].vNor, &pData->pVertices[dwCount].vNor, &matWorld );
				++dwCount;
			}

			mapMesh.insert ( std::make_pair(szName,pData) );
		}
		else								// Add
		{
			STATICDATA* pData = (STATICDATA*)(*iter).second;

			// 기존 데이터 백업
			STATICDATA OldData;
			OldData.dwFaceNUM = pData->dwFaceNUM;
			OldData.pVertices = new VERTEX[pData->dwFaceNUM*3];
			memcpy ( OldData.pVertices, pData->pVertices, sizeof(VERTEX)*OldData.dwFaceNUM*3 );

			// 데이터 추가 삽입
			pData->dwFaceNUM += dwFaceCount;
			SAFE_DELETE_ARRAY ( pData->pVertices );
			pData->pVertices = new VERTEX[pData->dwFaceNUM*3];
			memcpy ( pData->pVertices, OldData.pVertices, sizeof(VERTEX)*OldData.dwFaceNUM*3 );
			SAFE_DELETE_ARRAY ( OldData.pVertices );

			DWORD dwCount=OldData.dwFaceNUM*3;

			for ( DWORD i=dwFaceStart; i<dwFaceStart+dwFaceCount; ++i )
			{
				pData->pVertices[dwCount] = pVertices [pIndices[(i*3)+0]];
				D3DXVec3TransformCoord ( &pData->pVertices[dwCount].vPos, &pData->pVertices[dwCount].vPos, &matWorld );
				D3DXVec3TransformNormal ( &pData->pVertices[dwCount].vNor, &pData->pVertices[dwCount].vNor, &matWorld );
				++dwCount;

				pData->pVertices[dwCount] = pVertices [pIndices[(i*3)+1]];
				D3DXVec3TransformCoord ( &pData->pVertices[dwCount].vPos, &pData->pVertices[dwCount].vPos, &matWorld );
				D3DXVec3TransformNormal ( &pData->pVertices[dwCount].vNor, &pData->pVertices[dwCount].vNor, &matWorld );
				++dwCount;

				pData->pVertices[dwCount] = pVertices [pIndices[(i*3)+2]];
				D3DXVec3TransformCoord ( &pData->pVertices[dwCount].vPos, &pData->pVertices[dwCount].vPos, &matWorld );
				D3DXVec3TransformNormal ( &pData->pVertices[dwCount].vNor, &pData->pVertices[dwCount].vNor, &matWorld );
				++dwCount;
			}
		}
	}

	void MakeStaticMesh2 ( DxMeshes* pMesh, D3DXMATRIX& matWorld )
	{
		VERTEX*	pVertices;

		DWORD dwVertices = pMesh->m_pLocalMesh->GetNumVertices();
		DWORD dwFaces	= pMesh->m_pLocalMesh->GetNumFaces();

		DWORD*	pIndices = new DWORD[dwFaces*3];
		if ( pMesh->m_pLocalMesh->GetOptions()&D3DXMESH_32BIT )
		{
			DWORD*	pIndex32;
			pMesh->m_pLocalMesh->LockIndexBuffer ( 0L, (BYTE**)&pIndex32 );
			memcpy ( pIndices, pIndex32, sizeof(DWORD)*dwFaces*3 );
			pMesh->m_pLocalMesh->UnlockIndexBuffer ();
		}
		else
		{
			WORD*	pIndex16;
			pMesh->m_pLocalMesh->LockIndexBuffer ( 0L, (BYTE**)&pIndex16 );
			for ( DWORD i=0; i<dwFaces*3; ++i )
			{
				pIndices[i] = (DWORD)pIndex16[i];
			}
			pMesh->m_pLocalMesh->UnlockIndexBuffer ();
		}


		pMesh->m_pLocalMesh->LockVertexBuffer ( 0L, (BYTE**)&pVertices );

		DWORD dwAttibTableSize;
		pMesh->m_pLocalMesh->GetAttributeTable ( NULL, &dwAttibTableSize );

		LPD3DXATTRIBUTERANGE pAttribTable = NULL;
		pAttribTable = new D3DXATTRIBUTERANGE [ dwAttibTableSize ];
		pMesh->m_pLocalMesh->GetAttributeTable ( pAttribTable, &dwAttibTableSize );

		for ( DWORD i=0; i<dwAttibTableSize; ++i )
		{
			char szName[256];
			DWORD dwAttribID = pAttribTable[i].AttribId;
			strcpy ( szName, pMesh->strTextureFiles[dwAttribID].GetBuffer(256) );

			D3DSURFACE_DESC sDesc;
			sDesc.Format = D3DFMT_UNKNOWN;
			if ( pMesh->pTextures[dwAttribID] )	pMesh->pTextures[dwAttribID]->GetLevelDesc( 0, &sDesc );	// 텍스쳐가 없는 것들이 있을 수 있다.

			switch ( sDesc.Format )
			{
			case D3DFMT_A8R8G8B8:
			case D3DFMT_A1R5G5B5:
			case D3DFMT_A4R4G4B4:
			case D3DFMT_A8R3G3B2:
			case D3DFMT_A8:
			case D3DFMT_DXT2:
			case D3DFMT_DXT3:
			case D3DFMT_DXT4:
			case D3DFMT_DXT5:
				MakeStaticMesh3 ( szName, mapMeshAlpha, pVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld );
				break;
			case D3DFMT_X8R8G8B8:
			case D3DFMT_X1R5G5B5:
			case D3DFMT_X4R4G4B4:
			case D3DFMT_R8G8B8:
			case D3DFMT_R5G6B5:
			case D3DFMT_R3G3B2:
			case D3DFMT_DXT1:
			default:
				MakeStaticMesh3 ( szName, mapMeshData, pVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld );
				break;

			};
		}
		pMesh->m_pLocalMesh->UnlockVertexBuffer();

		SAFE_DELETE_ARRAY ( pAttribTable );
		SAFE_DELETE_ARRAY ( pIndices );
	}

	void MakeStaticMesh1 ( DxFrame *pframeCur )
	{		
		if ( pframeCur->IsReplace() || pframeCur->pframeFromAnimate )
		{
			// Note : 여기서는 아무것도 하면 안된다.
		}
		else
		{
			DxMeshes* pMesh = pframeCur->pmsMeshs;
			while ( pMesh )
			{
				MakeStaticMesh2 ( pMesh, pframeCur->matCombined );
				pMesh = pMesh->pMeshNext;
			}

			ExportProgress::CurPos++;
		}

		//	Note : 자식들을 재귀적으로 모두 탐색.
		DxFrame *pframeChild = pframeCur->pframeFirstChild;
		while (pframeChild != NULL)
		{
			MakeStaticMesh1 ( pframeChild );
			pframeChild = pframeChild->pframeSibling;
		}
	}

	void MakeStaticMesh ( LPDIRECT3DDEVICE8 pd3dDevice, DxStaticMesh* pStaticMesh )
	{
		MAPSTATICDATA_ITER iter;

		ExportProgress::SetStateString ( "Optimize Mesh로 생성 중...2/3" );
		ExportProgress::CurPos= 0;
		ExportProgress::EndPos = mapMeshData.size();
		for ( iter=mapMeshData.begin(); iter!=mapMeshData.end(); ++iter )
		{
			DxAABBOctree*	pOptimize = new DxAABBOctree;
			pOptimize->Create ( pd3dDevice, (*iter).first.c_str(), (*iter).second->dwFaceNUM, (*iter).second->pVertices );
			pOptimize->AddList ( pStaticMesh->m_pMeshList );

			ExportProgress::CurPos++;
		}

		ExportProgress::SetStateString ( "Optimize Mesh로 생성 중...3/3" );
		ExportProgress::CurPos= 0;
		ExportProgress::EndPos = mapMeshAlpha.size();
		for ( iter=mapMeshAlpha.begin(); iter!=mapMeshAlpha.end(); ++iter )
		{
			DxAABBOctree*	pOptimize = new DxAABBOctree;
			pOptimize->Create ( pd3dDevice, (*iter).first.c_str(), (*iter).second->dwFaceNUM, (*iter).second->pVertices );
			pOptimize->AddList ( pStaticMesh->m_pMeshAlphaList );

			ExportProgress::CurPos++;
		}
	}

	
	void FindNodeNUM ( DxFrame *pframeCur )
	{

		if ( pframeCur->IsReplace() || pframeCur->pframeFromAnimate )	{}
		else															ExportProgress::EndPos++;

		//	Note : 자식들을 재귀적으로 모두 탐색.
		DxFrame *pframeChild = pframeCur->pframeFirstChild;
		while (pframeChild != NULL)
		{
			FindNodeNUM ( pframeChild );
			pframeChild = pframeChild->pframeSibling;
		}
	}

	void ExportMaterialList ( LPDIRECT3DDEVICE8 pd3dDevice, DxFrameMesh* pFrameMesh, DxStaticMesh* pStaticMesh )
	{
		DxFrame* pFrameRoot = pFrameMesh->GetFrameRoot();
		if ( pFrameRoot )
		{
			std::for_each ( mapMeshData.begin(), mapMeshData.end(), std_afunc::DeleteMapObject() );
			mapMeshData.clear();
			std::for_each ( mapMeshAlpha.begin(), mapMeshAlpha.end(), std_afunc::DeleteMapObject() );
			mapMeshAlpha.clear();

			ExportProgress::SetStateString ( "Optimize Mesh로 생성 중...1/3" );
			ExportProgress::CurPos= 0;
			ExportProgress::EndPos = 0;
			FindNodeNUM ( pFrameRoot );

			MakeStaticMesh1 ( pFrameRoot );

			MakeStaticMesh ( pd3dDevice, pStaticMesh );

			std::for_each ( mapMeshData.begin(), mapMeshData.end(), std_afunc::DeleteMapObject() );
			mapMeshData.clear();
			std::for_each ( mapMeshAlpha.begin(), mapMeshAlpha.end(), std_afunc::DeleteMapObject() );
			mapMeshAlpha.clear();
		}
	}
};
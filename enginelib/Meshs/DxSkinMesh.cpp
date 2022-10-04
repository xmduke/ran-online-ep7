// DxSkinMesh.cpp: implementation of the DxSkinMesh class.
//
#include "stdafx.h"

#include "EDITMESHS.h"
#include "COLLISION.h"
#include "SerialFile.h"
#include "../DxCommon/TextureManager.h"

#include "DxLoadShader.h"

#include "DxSkinMesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

METHOD DxSkinMesh::m_method = D3DNONINDEXED;//D3DINDEXED;//D3DINDEXEDVS;//D3DNONINDEXED;

//--------------------------------------------------------------------------------[SFrame]
//
SFrame::~SFrame()
{
	SAFE_DELETE_ARRAY(szName);
	SAFE_DELETE(pmcMesh);
	SAFE_DELETE(pframeFirstChild);
	SAFE_DELETE(pframeSibling);

	// do NOT delete ppframeReplace
}

//--------------------------------------------------------------------------------[DxSkinMesh]
//
DxSkinMesh::DxSkinMesh() :
	m_pd3dDevice(NULL),
	m_dwD3dMaxVertexBlendMatrices(0),
	m_dwFVF(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1),
	m_pBoneMatrices(NULL),
	m_maxBones(0),
	pframeRoot(NULL),
	pMeshGlobalHead(NULL),
	vCenter(0,0,0),
	fRadius(0.0f),
	pSkeleton(NULL),
	dwRefCount(0),

	m_bOriginDraw(TRUE),
	m_bAlpha(FALSE),
	m_bShadow(FALSE),
	m_bAlphaTex(FALSE),
	m_bLevel(FALSE)
{
	memset( m_szName, 0, sizeof(TCHAR)*MAX_PATH );
	memset( m_szSkeleton, 0, sizeof(TCHAR)*MAX_PATH );

	D3DXMatrixIdentity ( &m_matWorld );

	Init_Detail_Texture ();
}

DxSkinMesh::~DxSkinMesh()
{
	ClearMeshHierarchy();
}

HRESULT DxSkinMesh::InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	
	D3DCAPS8 d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );
	m_dwD3dMaxVertexBlendMatrices = d3dCaps.MaxVertexBlendMatrices;

//	TextureManager::LoadTexture ( "test.tga", pd3dDevice, m_pTestTex, 0, 0 );

	return S_OK;
}

HRESULT DxSkinMesh::RestoreDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr = S_OK;
//
//	D3DCAPS8 d3dCaps;
//	pd3dDevice->GetDeviceCaps ( &d3dCaps );
//	
//	DWORD	m_dwUseSwShader = D3DUSAGE_SOFTWAREPROCESSING;
//
//	//	Note : Vertex Shader
//	//
//	if ( d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1) &&
//		d3dCaps.PixelShaderVersion >= D3DPS_VERSION(1, 1) )
//	{
//		m_method = D3DINDEXEDVS;	// 윽 안돼.
//		m_dwUseSwShader = 0;		// 이럴 땐 MIXED -> H/W 가속 이다.
//	}
//	else if ( d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1) )
//	{
//		m_method = D3DNONINDEXED;	// 윽.. 안돼.
//		m_dwUseSwShader = 0;		// 이럴 땐 MIXED -> H/W 가속 이다.
//	}
//	else
//	{
//		m_method = D3DNONINDEXED;							// 윽.. 안돼.
//		m_dwUseSwShader = D3DUSAGE_SOFTWAREPROCESSING;		// 이럴 땐      SW
//	}
//
////	m_method = D3DNONINDEXED;
//
//	for ( int i=0; i<4; i++ )
//	{
//		pd3dDevice->DeleteVertexShader ( m_dwIndexedVS[i] );
//	}
//	pd3dDevice->DeleteVertexShader ( m_dwIndexedPS );
//
//
//
//	hr = pd3dDevice->CreatePixelShader ( (DWORD*)dwSkinmeshPixelShader, &m_dwIndexedPS );
//	if( FAILED(hr) )
//	{
//		CDebugSet::ToListView ( "[ERROR] PS _ Skin Mesh FAILED" );
//		return E_FAIL;
//	}
//
//	//dwSkinMeshVS[];
//	//static DWORD		m_dwSkinMeshVS;	
//
//	// Create vertex shader for the indexed skinning
//    DWORD dwIndexedVertexDecl1[] =
//    {
//        D3DVSD_STREAM( 0 ),
//        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // Position of first mesh
//        D3DVSD_REG( 2, D3DVSDT_D3DCOLOR ), // Blend indices
////        D3DVSD_REG( 2, D3DVSDT_UBYTE4 ), // Blend indices
//        D3DVSD_REG( 3, D3DVSDT_FLOAT3 ), // Normal
//        D3DVSD_REG( 4, D3DVSDT_FLOAT2 ), // Tex coords
//		D3DVSD_REG( 5, D3DVSDT_FLOAT3 ), // U
//		D3DVSD_REG( 7, D3DVSDT_FLOAT3 ), // V
//        D3DVSD_END()
//    };
//
//    DWORD dwIndexedVertexDecl2[] =
//    {
//        D3DVSD_STREAM( 0 ),
//        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // Position of first mesh
//        D3DVSD_REG( 1, D3DVSDT_FLOAT1 ), // Blend weights
//        D3DVSD_REG( 2, D3DVSDT_D3DCOLOR ), // Blend indices
////        D3DVSD_REG( 2, D3DVSDT_UBYTE4 ), // Blend indices
//        D3DVSD_REG( 3, D3DVSDT_FLOAT3 ), // Normal
//        D3DVSD_REG( 4, D3DVSDT_FLOAT2 ), // Tex coords
//		D3DVSD_REG( 5, D3DVSDT_FLOAT3 ), // U
//		D3DVSD_REG( 7, D3DVSDT_FLOAT3 ), // V
//        D3DVSD_END()
//    };
//
//    DWORD dwIndexedVertexDecl3[] =
//    {
//        D3DVSD_STREAM( 0 ),
//        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // Position of first mesh
//        D3DVSD_REG( 1, D3DVSDT_FLOAT2 ), // Blend weights
//        D3DVSD_REG( 2, D3DVSDT_D3DCOLOR ), // Blend indices
////        D3DVSD_REG( 2, D3DVSDT_UBYTE4 ), // Blend indices
//        D3DVSD_REG( 3, D3DVSDT_FLOAT3 ), // Normal
//        D3DVSD_REG( 4, D3DVSDT_FLOAT2 ), // Tex coords
//		D3DVSD_REG( 5, D3DVSDT_FLOAT3 ), // U
//		D3DVSD_REG( 7, D3DVSDT_FLOAT3 ), // V
//        D3DVSD_END()
//    };
//
//    DWORD dwIndexedVertexDecl4[] =
//    {
//        D3DVSD_STREAM( 0 ),
//        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // Position of first mesh
//        D3DVSD_REG( 1, D3DVSDT_FLOAT3 ), // Blend weights
//        D3DVSD_REG( 2, D3DVSDT_D3DCOLOR ), // Blend indices
////        D3DVSD_REG( 2, D3DVSDT_UBYTE4 ), // Blend indices
//        D3DVSD_REG( 3, D3DVSDT_FLOAT3 ), // Normal
//        D3DVSD_REG( 4, D3DVSDT_FLOAT2 ), // Tex coords
//		D3DVSD_REG( 5, D3DVSDT_FLOAT3 ), // U
//		D3DVSD_REG( 7, D3DVSDT_FLOAT3 ), // V
//        D3DVSD_END()
//    };
//
//	DWORD* dwIndexedVertexDecl[] = {dwIndexedVertexDecl1, dwIndexedVertexDecl2, dwIndexedVertexDecl3, dwIndexedVertexDecl4};
//
//	for ( DWORD i=0; i<4; ++i )
//	{
//		DWORD* dwSkinmeshVS[] = {dwSkinmesh1VertexShader, dwSkinmesh2VertexShader, dwSkinmesh3VertexShader, dwSkinmesh4VertexShader};
//
//		hr = pd3dDevice->CreateVertexShader ( dwIndexedVertexDecl[i], 
//											(DWORD*)dwSkinmeshVS[i], 
//											&m_dwIndexedVS[i], 
//											m_dwUseSwShader );
//		if( FAILED(hr) )
//		{
//			CDebugSet::ToListView ( "[ERROR] VS _ Skin Mesh FAILED" );
//			return E_FAIL;
//		}
//
//	}
//
//	// Create vertex shader
//    {
//        LPD3DXBUFFER pCode;
//
//		std::string strFilePath = DXShaderMan::GetInstance().GetPath();
//		strFilePath += "Test_1.vsh";
//        // Assemble the vertex shader from the file
//		if( FAILED( hr = D3DXAssembleShaderFromFile( strFilePath.c_str(), 
//                                                     0, NULL, &pCode, NULL ) ) )
//			return hr;
//
//        // Create the vertex shader
//        hr = pd3dDevice->CreateVertexShader( dwIndexedVertexDecl1, (DWORD*)pCode->GetBufferPointer(), &m_dwTestVS[0], 0 );
//        pCode->Release();
//        if( FAILED(hr) )
//            return hr;
//    }
//
//	// Create vertex shader
//    {
//        LPD3DXBUFFER pCode;
//
//		std::string strFilePath = DXShaderMan::GetInstance().GetPath();
//		strFilePath += "Test_2.vsh";
//        // Assemble the vertex shader from the file
//        if( FAILED( hr = D3DXAssembleShaderFromFile( strFilePath.c_str(), 
//                                                     0, NULL, &pCode, NULL ) ) )
//			return hr;
//
//        // Create the vertex shader
//        hr = pd3dDevice->CreateVertexShader( dwIndexedVertexDecl2, (DWORD*)pCode->GetBufferPointer(), &m_dwTestVS[1], 0 );
//        pCode->Release();
//        if( FAILED(hr) )
//            return hr;
//    }
//
//	// Create vertex shader
//    {
//        LPD3DXBUFFER pCode;
//
//		std::string strFilePath = DXShaderMan::GetInstance().GetPath();
//		strFilePath += "Test_3.vsh";
//        // Assemble the vertex shader from the file
//        if( FAILED( hr = D3DXAssembleShaderFromFile( strFilePath.c_str(), 
//                                                     0, NULL, &pCode, NULL ) ) )
//			return hr;
//
//        // Create the vertex shader
//        hr = pd3dDevice->CreateVertexShader( dwIndexedVertexDecl3, (DWORD*)pCode->GetBufferPointer(), &m_dwTestVS[2], 0 );
//        pCode->Release();
//        if( FAILED(hr) )
//            return hr;
//    }
//
//	// Create vertex shader
//    {
//        LPD3DXBUFFER pCode;
//
//		std::string strFilePath = DXShaderMan::GetInstance().GetPath();
//		strFilePath += "Test_4.vsh";
//        // Assemble the vertex shader from the file
//        if( FAILED( hr = D3DXAssembleShaderFromFile( strFilePath.c_str(), 
//                                                     0, NULL, &pCode, NULL ) ) )
//			return hr;
//
//        // Create the vertex shader
//        hr = pd3dDevice->CreateVertexShader( dwIndexedVertexDecl4, (DWORD*)pCode->GetBufferPointer(), &m_dwTestVS[3], 0 );
//        pCode->Release();
//        if( FAILED(hr) )
//            return hr;
//    }
//
//	// Create pixel shader
//    {
//        LPD3DXBUFFER pCode;
//
//		std::string strFilePath = DXShaderMan::GetInstance().GetPath();
//		strFilePath += "Test.psh";
//        // Assemble the pixel shader from the file
//        if( FAILED( hr = D3DXAssembleShaderFromFile( strFilePath.c_str(), 
//                                                     0, NULL, &pCode, NULL ) ) )
//            return hr;
//
//        // Create the pixel shader
//        hr = pd3dDevice->CreatePixelShader( (DWORD*)pCode->GetBufferPointer(), &m_dwTestPS);
//        pCode->Release();
//        if( FAILED(hr) )
//            return hr;
//    }

	return S_OK;
}

void DxSkinMesh::Init_Detail_Texture ()
{
	m_bOriginDraw	= TRUE;
	m_bAlpha		= FALSE;
	m_bShadow		= FALSE;
	m_bAlphaTex		= TRUE;
	m_bLevel		= FALSE;
	m_fHeight		= 0.f;
}

//	Note : 만억 pMesh 가 MANAGED, SYSTEMMEM 속성을 가진 개체가 아닐 경우
//		Invalidate 호출시에 제거하고 새로 Mesh가 만들어져야 한다. 그때
//		사용된다.
//
void ReleaseDeviceDependentMeshes(SFrame* pframe)
{
	if ( pframe->pmcMesh != NULL )
	{
		for ( SMeshContainer* pmcCurr = pframe->pmcMesh; pmcCurr != NULL; pmcCurr = pmcCurr->m_pmcNext )
		{
			if ( pmcCurr->m_pSkinMesh != NULL )
			{
				GXRELEASE(pmcCurr->m_pMesh);

				pmcCurr->m_Method = NONE;
			}
		}
	}

	if ( pframe->pframeFirstChild != NULL )
		ReleaseDeviceDependentMeshes ( pframe->pframeFirstChild );

	if ( pframe->pframeSibling != NULL )
		ReleaseDeviceDependentMeshes ( pframe->pframeSibling );
}


HRESULT DxSkinMesh::InvalidateDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	//	Note : 만억 pMesh 가 MANAGED, SYSTEMMEM 속성을 가진 개체가 아닐 경우
	//		Invalidate 호출시에 제거하고 새로 Mesh가 만들어져야 한다. 그때
	//		사용된다.
	//
	//if ( pframeRoot )
	//	ReleaseDeviceDependentMeshes ( pframeRoot );


	//for ( int i=0; i<4; i++ )
	//{
	//	pd3dDevice->DeleteVertexShader ( m_dwIndexedVS[i] );

	//	pd3dDevice->DeleteVertexShader ( m_dwTestVS[i] );
	//}
	//pd3dDevice->DeleteVertexShader ( m_dwIndexedPS );

	//pd3dDevice->DeleteVertexShader ( m_dwTestPS );

	return S_OK;
}


HRESULT DxSkinMesh::DeleteDeviceObjects()
{
	SAFE_DELETE(pframeRoot);
	SAFE_DELETE_ARRAY(m_pBoneMatrices);

/*	if ( m_pTestTex )
		TextureManager::ReleaseTexture ( m_pTestTex );
	m_pTestTex = NULL;	*/	

	return S_OK;
}

HRESULT DxSkinMesh::ClearMeshHierarchy()
{
	SAFE_DELETE(pframeRoot);
	SAFE_DELETE_ARRAY(m_pBoneMatrices);

	return S_OK;
}

SMeshContainer* DxSkinMesh::FindMeshContainer ( char *szName )
{
	//	Note : 메쉬 찾기.
	//
	SMeshContainer *pmcMesh = pMeshGlobalHead;
	while ( pmcMesh != NULL )
	{
		if ( !strcmp(pmcMesh->m_szName,szName) )	return pmcMesh;

		pmcMesh = pmcMesh->m_pmcGlobalNext;
	}

	return NULL;
}


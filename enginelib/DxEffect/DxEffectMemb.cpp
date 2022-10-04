// DxEffectMemb.cpp: implementation of the DxEffectMemb class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DxFrameMesh.h"
#include "DxEffectMan.h"

#include "DxEffectMemb.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW


const DWORD DxEffectMemb::TYPEID = DEF_EFFECT_MEMBRANE;
const DWORD	DxEffectMemb::VERSION = 0x00000100;
const char DxEffectMemb::NAME[] = "[ 얇은 막 효과 ]";
const DWORD DxEffectMemb::FLAG = NULL;

void DxEffectMemb::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(MEMBRANE_PROPERTY);
	dwVer = VERSION;
}

void DxEffectMemb::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(MEMBRANE_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxEffectMemb::DxEffectMemb() :
	m_pddsTexture(NULL),
	m_fTime(0.0f),
	m_fLoop(0.0f),
	m_bBase(TRUE),
	m_bMemb(TRUE),
	m_fSpeed(0.002f)		// 돌아가는 속도 !!
{
	strcpy ( m_szTexture, "Test_Day.dds" );
}

DxEffectMemb::~DxEffectMemb()
{
	SAFE_DELETE_ARRAY(m_szAdaptFrame);
}

HRESULT DxEffectMemb::CreateDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr;

	DWORD dwDecl[] =
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG(0, D3DVSDT_FLOAT3 ),			//D3DVSDE_POSITION,  0  
		D3DVSD_REG(3, D3DVSDT_FLOAT3 ),		//D3DVSDE_DIFFUSE,   5  
		D3DVSD_REG(7, D3DVSDT_FLOAT2 ),		//D3DVSDE_DIFFUSE,   5  
		D3DVSD_END()
	};

	hr = pd3dDevice->CreateVertexShader ( dwDecl, (DWORD*)dwMemVertexShader, &m_dwEffect, 
											DxEffectMan::Instance.GetUseSwShader() );
	if( FAILED(hr) )
	{
		CDebugSet::ToListView ( "[ERROR] VS _ Memb FAILED" );
		return E_FAIL;
	}


	hr = pd3dDevice->CreateVertexShader ( dwDecl, (DWORD*)dwMem2VertexShader, &m_dwEffect2, 
											DxEffectMan::Instance.GetUseSwShader() );
	if( FAILED(hr) )
	{
		CDebugSet::ToListView ( "[ERROR] VS _ Memb FAILED" );
		return E_FAIL;
	}


	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		//
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,	D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,	D3DBLEND_ONE );

		//	Note : SetTextureStageState() 선언
		//
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );


		if( which==0 )	pd3dDevice->EndStateBlock( &m_dwSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_dwEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffectMemb::ReleaseDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	pd3dDevice->DeleteVertexShader ( m_dwEffect );
	pd3dDevice->DeleteVertexShader ( m_dwEffect2 );

	if ( pd3dDevice )
	{
		if ( m_dwSavedStateBlock )
			pd3dDevice->DeleteStateBlock( m_dwSavedStateBlock );
		if ( m_dwEffectStateBlock )
			pd3dDevice->DeleteStateBlock( m_dwEffectStateBlock );
	}

	m_dwSavedStateBlock		= NULL;
	m_dwEffectStateBlock	= NULL;

	return S_OK;
}

HRESULT DxEffectMemb::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICE8 pd3dDevice )
{
	if ( !pFrame ) return S_OK;

	this->pLocalFrameEffNext = pFrame->pEffect;
	pFrame->pEffect = this;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		m_szAdaptFrame = new char[strlen(pFrame->szName)+1];

		strcpy ( m_szAdaptFrame, pFrame->szName );
	}

	return S_OK;
}


HRESULT DxEffectMemb::InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	//	Note : 텍스쳐의 읽기 오류는 무시한다.
	//
	TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pddsTexture, 0, 0 );

	if ( !m_pddsTexture )
	{
		char szMsg[256];
		sprintf ( szMsg, "이미지 파일 '%s' 읽기에 실패", m_szTexture );
		MessageBox ( NULL, szMsg, "ERROR", MB_OK );
	}


	return S_OK;
}

HRESULT DxEffectMemb::DeleteDeviceObjects ()
{
	//	Note : 이전 텍스쳐가 로드되어 있을 경우 제거.
	//
	if ( m_pddsTexture )
		TextureManager::ReleaseTexture ( m_pddsTexture );
	m_pddsTexture = NULL;

	return S_OK;
}

HRESULT DxEffectMemb::FrameMove ( float fTime, float fElapsedTime )
{
	m_fTime += fElapsedTime;
	return S_OK;
}

HRESULT DxEffectMemb::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICE8 pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : Render 
	//
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : 메쉬 그리기.
		//
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh || pmsMeshs->m_pDxOctreeMesh )
			{
				DWORD dwFVFSize;
				DWORD dwVerts, dwFaces;
				LPDIRECT3DINDEXBUFFER8 pIB	= NULL;
				LPDIRECT3DVERTEXBUFFER8 pVB = NULL;

				//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
				//
				if ( pmsMeshs->m_pLocalMesh )
				{				
					pmsMeshs->m_pLocalMesh->GetIndexBuffer ( &pIB );
					pmsMeshs->m_pLocalMesh->GetVertexBuffer ( &pVB );
					
					dwFVFSize = pmsMeshs->m_pLocalMesh->GetFVF ();
					dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );

					dwVerts = pmsMeshs->m_pLocalMesh->GetNumVertices();
					dwFaces = pmsMeshs->m_pLocalMesh->GetNumFaces();
				}
				else
				{
					dwFVFSize = pmsMeshs->m_pDxOctreeMesh->m_dwFVF;
					dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );

					dwVerts = pmsMeshs->m_pDxOctreeMesh->m_dwNumVertices;
					dwFaces = pmsMeshs->m_pDxOctreeMesh->m_dwNumFaces;
				}


				if( m_fTime > 0.05f )
				{
					m_fLoop += m_fSpeed;

					if( m_fLoop > 1.0f )	m_fLoop = 0.0f;
				}


				//	Note : SetVertexShader에 World와 WorldViewProjection 행렬 집어 넣기
				//
				D3DXMATRIX	matWorld, matView, matProj, matWVP;

				matView = DxViewPort::Instance.GetMatView();
				matProj = DxViewPort::Instance.GetMatProj();

				D3DXMatrixTranspose( &matWorld, &(pframeCur->matCombined) );
				pd3dDevice->SetVertexShaderConstant ( VSC_MATWVP_01,			&matWorld,	4 );

				D3DXMatrixMultiply ( &matWVP, &(pframeCur->matCombined), &matView );
				D3DXMatrixMultiply ( &matWVP, &matWVP, &matProj );
				D3DXMatrixTranspose( &matWVP, &matWVP );
				pd3dDevice->SetVertexShaderConstant ( VSC_MATWVP_02, &matWVP,	4 );








				DWORD					AttribTableSize = 0;
				LPD3DXATTRIBUTERANGE	pSrcAttribTable = NULL;
				
				if ( pmsMeshs->m_pLocalMesh )
				{	
					pmsMeshs->m_pLocalMesh->GetAttributeTable ( NULL, &AttribTableSize );
				}
				else
				{
					AttribTableSize = pmsMeshs->m_pDxOctreeMesh->m_dwAttribTableSize;
				}
				
				LPD3DXATTRIBUTERANGE pAttribTable;
				pAttribTable = new D3DXATTRIBUTERANGE [ AttribTableSize ];

				if ( pmsMeshs->m_pLocalMesh )
				{
					pmsMeshs->m_pLocalMesh->GetAttributeTable( pAttribTable, &AttribTableSize );
				}
				else
				{
					memcpy ( pAttribTable, pmsMeshs->m_pDxOctreeMesh->m_pAttribTable, sizeof ( D3DXATTRIBUTERANGE ) * AttribTableSize );
				}

				//	Note : 불투명 메쉬 드로잉.
				//		pmsMeshs->cMaterials --> AttribTableSize
				//
				for ( DWORD i=0; i<AttribTableSize; i++ )
				{







				pd3dDevice->SetTexture ( 0, m_pddsTexture );

				pd3dDevice->SetVertexShaderConstant ( VSC_MATRIAL, &( pmsMeshs->rgMaterials[0].Diffuse ), 1 );

				//	Note : 이전 상태 백업.
				//
			//	pd3dDevice->CaptureStateBlock ( m_dwSavedStateBlock );

				//	Note : 랜더링 상태 조정.
				//
			//	pd3dDevice->ApplyStateBlock ( m_dwEffectStateBlock );



				//	Note : 소프트웨어 버텍스 프로세싱
				//
				if ( DxEffectMan::Instance.GetUseSwShader() )
					pd3dDevice->SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, TRUE );


				//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
				//
				if ( pmsMeshs->m_pLocalMesh )
				{
					pd3dDevice->SetIndices ( pIB, 0 );
					pd3dDevice->SetStreamSource ( 0, pVB, dwFVFSize );
					pd3dDevice->SetVertexShader ( pmsMeshs->m_pLocalMesh->GetFVF () );
				}
				else
				{
					pd3dDevice->SetIndices ( pmsMeshs->m_pDxOctreeMesh->m_pIB, 0 );
					pd3dDevice->SetStreamSource ( 0, pmsMeshs->m_pDxOctreeMesh->m_pVB, dwFVFSize );
					pd3dDevice->SetVertexShader ( pmsMeshs->m_pDxOctreeMesh->m_dwFVF );
				}


				if( m_bMemb && !m_bBase)
				{
					pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
					pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,	D3DBLEND_ONE );
					pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,	D3DBLEND_ONE );

					pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

					pd3dDevice->SetTexture ( 0, m_pddsTexture );
					pd3dDevice->SetVertexShader ( m_dwEffect );
				}
				else if( m_bMemb && m_bBase )
				{
					pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
					pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_ADD );

					pd3dDevice->SetTexture ( 0, pmsMeshs->pTextures[pAttribTable[i].AttribId] );
					pd3dDevice->SetTexture ( 1, m_pddsTexture );
					pd3dDevice->SetVertexShader ( m_dwEffect2 );
				}
				else
				{
					pd3dDevice->SetTransform ( D3DTS_WORLD, &(pframeCur->matCombined) );
					pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

					pd3dDevice->SetTexture ( 0, pmsMeshs->pTextures[pAttribTable[i].AttribId] );
				//	pd3dDevice->SetVertexShader ( pmsMeshs->m_pLocalMesh->GetFVF () );	// 전에 해줌
				}

				pd3dDevice->DrawIndexedPrimitive
					(
						D3DPT_TRIANGLELIST,
						pAttribTable[i].VertexStart,
						pAttribTable[i].VertexCount,
						pAttribTable[i].FaceStart * 3,
						pAttribTable[i].FaceCount
					);


				pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );
				pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
				pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_DISABLE );


				//	Note : 소프트웨어 버텍스 프로세싱
				//
				if ( DxEffectMan::Instance.GetUseSwShader() )
					pd3dDevice->SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, FALSE );

				pd3dDevice->SetIndices ( NULL, 0 );
				pd3dDevice->SetStreamSource ( 0, NULL, 0 );



				//	Note : 이전상태로 복원.
				//
			//	pd3dDevice->ApplyStateBlock ( m_dwSavedStateBlock );







				}

				SAFE_DELETE(pAttribTable);

				pd3dDevice->SetTexture ( 0, NULL );
				pd3dDevice->SetTexture ( 1, NULL );

				if ( pmsMeshs->m_pLocalMesh )
				{
					if(pIB) pIB->Release ();
					if(pVB) pVB->Release ();
				}
			}

			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	//	Note : 자식 프레임 그리기.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = Render ( pframeChild, pd3dDevice, pFrameMesh, pLandMan );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

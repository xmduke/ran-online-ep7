// DxEffectFishEye.cpp: implementation of the DxEffectFishEye class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DxFrameMesh.h"
#include "DxEffectMan.h"

#include "DxEffectFishEye.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW


const DWORD DxEffectFishEye::TYPEID = DEF_EFFECT_FISHEYE;
const DWORD	DxEffectFishEye::VERSION = 0x00000100;
const char DxEffectFishEye::NAME[] = "[18]_[ 볼록렌즈 ]";
const DWORD DxEffectFishEye::FLAG = NULL;

void DxEffectFishEye::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(FISHEYE_PROPERTY);
	dwVer = VERSION;
}

void DxEffectFishEye::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(FISHEYE_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxEffectFishEye::DxEffectFishEye() :
	m_fXZRatio(2.5f),
	m_fDistortion(2.0)
{
}

DxEffectFishEye::~DxEffectFishEye()
{
	SAFE_DELETE_ARRAY(m_szAdaptFrame);
}

HRESULT DxEffectFishEye::CreateDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr;

	DWORD dwDecl[] =
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG(0, D3DVSDT_FLOAT3 ),			//D3DVSDE_POSITION,  0  
		D3DVSD_REG(1, D3DVSDT_FLOAT3 ),		
		D3DVSD_REG(2, D3DVSDT_FLOAT2 ),	
		D3DVSD_END()
	};


	hr = pd3dDevice->CreateVertexShader ( dwDecl, (DWORD*)dwFisheyeVertexShader, &m_dwEffect, 
											DxEffectMan::Instance.GetUseSwShader() );
	if( FAILED(hr) )	return E_FAIL;


	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetTextureStageState() 선언
		//
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ADDRESSU,	D3DTADDRESS_MIRROR );


		if( which==0 )	pd3dDevice->EndStateBlock( &m_dwSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_dwEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffectFishEye::ReleaseDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	pd3dDevice->DeleteVertexShader ( m_dwEffect );

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

HRESULT DxEffectFishEye::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICE8 pd3dDevice )
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


HRESULT DxEffectFishEye::InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
{

	return S_OK;
}

HRESULT DxEffectFishEye::DeleteDeviceObjects ()
{

	return S_OK;
}

HRESULT DxEffectFishEye::FrameMove ( float fTime, float fElapsedTime )
{

	return S_OK;
}

HRESULT DxEffectFishEye::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICE8 pd3dDevice )
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


				//	Note : Shader 상수 선언
				//
				D3DXVECTOR4	m_vConstants(m_fDistortion + 1.0f, 0.0f, 0.5f, 0.8f);
				D3DXVECTOR4	m_vInfo(m_fXZRatio, m_fDistortion, 0.0f, 1.0f);

				pd3dDevice->SetVertexShaderConstant ( VSC_CUSTOM+2,		&m_vConstants,	1 );
				pd3dDevice->SetVertexShaderConstant ( VSC_CUSTOM+1,		&m_vInfo,		1 );

				//	Note : SetVertexShader에 World와 WorldViewProjection 행렬 집어 넣기
				//
				D3DXMATRIX	matWorld, matView, matProj, matWVP;

				matView = DxViewPort::Instance.GetMatView();
				matProj = DxViewPort::Instance.GetMatProj();

				D3DXMatrixTranspose( &matWorld, &(pframeCur->matCombined) );
				pd3dDevice->SetVertexShaderConstant ( VSC_MATWORLD,			&matWorld,	4 );

				D3DXMatrixMultiply ( &matWVP, &(pframeCur->matCombined), &matView );
				D3DXMatrixMultiply ( &matWVP, &matWVP, &matProj );
				D3DXMatrixTranspose( &matWVP, &matWVP );
				pd3dDevice->SetVertexShaderConstant ( VSC_MATWORLDVIEWPROJ, &matWVP,	4 );



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
					//	Note : 메터리얼, 텍스쳐 지정.
					pd3dDevice->SetVertexShaderConstant ( VSC_MATRIAL, &( pmsMeshs->rgMaterials[i].Diffuse ), 1 );
	//				pd3dDevice->SetVertexShaderConstant ( VSC_AMBIENT, &( pmsMeshs->rgMaterials[i].Ambient ), 1 );
					pd3dDevice->SetTexture ( 0, pmsMeshs->pTextures[pAttribTable[i].AttribId] );
				

					//	Note : 이전 상태 백업.
					//
					pd3dDevice->CaptureStateBlock ( m_dwSavedStateBlock );

					//	Note : 랜더링 상태 조정.
					//
					pd3dDevice->ApplyStateBlock ( m_dwEffectStateBlock );



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
					}
					else
					{
						pd3dDevice->SetIndices ( pmsMeshs->m_pDxOctreeMesh->m_pIB, 0 );
						pd3dDevice->SetStreamSource ( 0, pmsMeshs->m_pDxOctreeMesh->m_pVB, dwFVFSize );
					}		

					pd3dDevice->SetVertexShader ( m_dwEffect );


					pd3dDevice->DrawIndexedPrimitive
					(
						D3DPT_TRIANGLELIST,
						pAttribTable[i].VertexStart,
						pAttribTable[i].VertexCount,
						pAttribTable[i].FaceStart * 3,
						pAttribTable[i].FaceCount
					);


					//	Note : 소프트웨어 버텍스 프로세싱
					//
					if ( DxEffectMan::Instance.GetUseSwShader() )
						pd3dDevice->SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, FALSE );

					pd3dDevice->SetIndices ( NULL, 0 );
					pd3dDevice->SetStreamSource ( 0, NULL, 0 );



					//	Note : 이전상태로 복원.
					//
					pd3dDevice->ApplyStateBlock ( m_dwSavedStateBlock );


					pd3dDevice->SetTexture ( 0, NULL );
				}

				SAFE_DELETE(pAttribTable);

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
		hr = Render ( pframeChild, pd3dDevice );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

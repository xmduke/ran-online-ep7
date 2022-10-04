// DxEffectFire.cpp: implementation of the DxEffectFire class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DxFrameMesh.h"
#include "DxEffectMan.h"

#include "DxEffectFire.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW


const DWORD DxEffectFire::FIREVERTEX::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
const DWORD DxEffectFire::TYPEID = DEF_EFFECT_FIRE;
const DWORD	DxEffectFire::VERSION = 0x00000100;
const char DxEffectFire::NAME[] = "[3009]_[ 불 효과 ]";
const DWORD DxEffectFire::FLAG = NULL;

void DxEffectFire::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(FIRE_PROPERTY);
	dwVer = VERSION;
}

void DxEffectFire::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(FIRE_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
}

DWORD	DxEffectFire::m_dwSavedStateBlock(0x00000000);
DWORD	DxEffectFire::m_dwEffectStateBlock(0x00000000);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////6.f/32.f , 0.35f, 31.f/32.f, 0.4f

DxEffectFire::DxEffectFire() :
	m_fTime(0.0f),
	m_dwVertices(0),
	m_dwIndices(0),
	m_iNowSprite(0),
	m_pVB(NULL),
	m_pIB(NULL),
	m_pddsTexture(NULL),
	m_dwNumFaces(3),
	m_fPosX(0.0f),
	m_fPosY(0.0f),
	m_fPosZ(0.0f),
	m_iCol(10),
	m_iRow(1),
	m_fAniTime(0.1f),
	m_fUpLong(0.f),
	m_fDownLong(5.f),
	m_fHeight(20.f)
{
	strcpy ( m_szTexture, "_Spr_wall.jpg" );
}

DxEffectFire::~DxEffectFire()
{
	SAFE_DELETE_ARRAY(m_szAdaptFrame);
}

HRESULT DxEffectFire::CreateDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		//
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE, FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,	D3DBLEND_ONE );

		//	Note : SetTextureStageState() 선언
		//
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );


		if( which==0 )	pd3dDevice->EndStateBlock( &m_dwSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_dwEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffectFire::ReleaseDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
{
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

HRESULT DxEffectFire::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICE8 pd3dDevice )
{
	DxMeshes *pmsMeshs;

	if ( pframeCur->pmsMeshs != NULL )
	{
		pmsMeshs = pframeCur->pmsMeshs;

		if ( pmsMeshs->m_pLocalMesh )
		{
			CreateFireMesh ( pd3dDevice, pmsMeshs->m_pLocalMesh );
		}
	}

	return S_OK;
}

HRESULT DxEffectFire::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICE8 pd3dDevice )
{
	if ( !pFrame ) return S_OK;

	this->pLocalFrameEffNext = pFrame->pEffectNext;
	pFrame->pEffectNext = this;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		m_szAdaptFrame = new char[strlen(pFrame->szName)+1];

		strcpy ( m_szAdaptFrame, pFrame->szName );
	}

	return AdaptToDxFrameChild ( pFrame, pd3dDevice );
}


HRESULT DxEffectFire::InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
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

HRESULT DxEffectFire::DeleteDeviceObjects ()
{
	//	Note : 이전 텍스쳐가 로드되어 있을 경우 제거.
	//
	if ( m_pddsTexture )
		TextureManager::ReleaseTexture ( m_pddsTexture );
	m_pddsTexture = NULL;

	return S_OK;
}

HRESULT DxEffectFire::FrameMove ( float fTime, float fElapsedTime )
{
	m_fTime += fElapsedTime;

	return S_OK;
}

HRESULT DxEffectFire::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr = S_OK;

	//	Note : Render ( 잔상 )
	//
	if ( m_pVB )
	{
		//	Note : SetRenderState() 선언
		//
		D3DXMATRIX matIdentity, matWorld;
		D3DXMatrixIdentity( &matIdentity );

		pd3dDevice->GetTransform( D3DTS_WORLD, &matWorld );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );


		DWORD					dwFVFSize;
//		DWORD					dwVerts, dwFaces;

		
		dwFVFSize = FIREVERTEX::FVF;
		dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );

//		dwVerts = (m_dwNumFaces+1)*2;
//		dwFaces = m_dwNumFaces;


		//	Note : 스프라이트
		//
		int iSpriteNum = m_iCol * m_iRow;

		if ( m_fTime > m_fAniTime )
		{
			m_iNowSprite += 1;
			m_fTime = 0.0f;
		}

		if ( m_iNowSprite >= iSpriteNum )
		{
			m_iNowSprite = 0;
		}

		FIREVERTEX*				pVertex;

		m_pVB->Lock ( 0, 0, (BYTE**)&pVertex, 0 );

		for ( DWORD i=0; i<m_dwNumFaces; i++ )
		{
			pVertex->t.x = m_iNowSprite / (float)m_iCol;
			pVertex->t.y = (m_iNowSprite / m_iRow) / (float)m_iRow;
			pVertex++;
			pVertex->t.x = (m_iNowSprite+1) / (float)m_iCol;
			pVertex->t.y = (m_iNowSprite / m_iRow) / (float)m_iRow;
			pVertex++;
			pVertex->t.x = m_iNowSprite / (float)m_iCol;
			pVertex->t.y = ((m_iNowSprite+m_iRow) / m_iRow) / (float)m_iRow;
			pVertex++;
			pVertex->t.x = (m_iNowSprite+1) / (float)m_iCol;
			pVertex->t.y = ((m_iNowSprite+m_iRow) / m_iRow) / (float)m_iRow;
			pVertex++;
		}

		m_pVB->Unlock ();



		pd3dDevice->SetVertexShader ( FIREVERTEX::FVF );

		pd3dDevice->SetTexture ( 0, m_pddsTexture );

		//	Note : 이전 상태 백업.
		//
		pd3dDevice->CaptureStateBlock ( m_dwSavedStateBlock );

		//	Note : 랜더링 상태 조정.
		//
		pd3dDevice->ApplyStateBlock ( m_dwEffectStateBlock );

		pd3dDevice->SetIndices ( m_pIB, 0 );
		pd3dDevice->SetStreamSource ( 0, m_pVB, dwFVFSize );


		pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, m_dwVertices, 0, m_dwNumFaces*2 );

		

		pd3dDevice->SetIndices ( NULL, 0 );
		pd3dDevice->SetStreamSource ( 0, NULL, 0 );



		//	Note : 이전상태로 복원.
		//
		pd3dDevice->ApplyStateBlock ( m_dwSavedStateBlock );



		pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );

		pd3dDevice->SetTexture ( 0, NULL );
	}

	return S_OK;
}

HRESULT DxEffectFire::CreateFireMesh ( LPDIRECT3DDEVICE8 pd3dDevice, LPD3DXMESH m_pLocalMesh )
{
	m_dwVertices = m_dwNumFaces*4;
	m_dwIndices = m_dwNumFaces*6;
	pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(FIREVERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, 
										FIREVERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pVB );

	pd3dDevice->CreateIndexBuffer ( m_dwIndices*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB );

	FIREVERTEX*	pVertices;
	m_pVB->Lock ( 0, 0, (BYTE**)&pVertices, 0 );

	for ( DWORD i=0; i<m_dwNumFaces; i++ )
	{
		pVertices->p.x = m_fPosX + cosf( 2.f*D3DX_PI*( (float)i/(float)m_dwNumFaces ) )*m_fUpLong;
		pVertices->p.y = m_fPosY + m_fHeight;
		pVertices->p.z = m_fPosZ + sinf( 2.f*D3DX_PI*( (float)i/(float)m_dwNumFaces ) )*m_fUpLong;
		pVertices->d = D3DCOLOR_ARGB ( 100, 255, 255, 255 );
		pVertices->t = D3DXVECTOR2 ( 0.f, 0.f );
		pVertices++;

		pVertices->p.x = m_fPosX + cosf( 2.f*D3DX_PI*( (float)(i+1)/(float)m_dwNumFaces ) )*m_fUpLong;
		pVertices->p.y = m_fPosY + m_fHeight;
		pVertices->p.z = m_fPosZ + sinf( 2.f*D3DX_PI*( (float)(i+1)/(float)m_dwNumFaces ) )*m_fUpLong;
		pVertices->d = D3DCOLOR_ARGB ( 100, 255, 255, 255 );
		pVertices->t = D3DXVECTOR2 ( 1.f, 0.f );
		pVertices++;

		pVertices->p.x = m_fPosX + cosf( 2.f*D3DX_PI*( (float)i/(float)m_dwNumFaces ) )*m_fDownLong;
		pVertices->p.y = m_fPosY;
		pVertices->p.z = m_fPosZ + sinf( 2.f*D3DX_PI*( (float)i/(float)m_dwNumFaces ) )*m_fDownLong;
		pVertices->d = D3DCOLOR_ARGB ( 100, 255, 255, 255 );
		pVertices->t = D3DXVECTOR2 ( 0.f, 1.f );
		pVertices++;

		pVertices->p.x = m_fPosX + cosf( 2.f*D3DX_PI*( (float)(i+1)/(float)m_dwNumFaces ) )*m_fDownLong;
		pVertices->p.y = m_fPosY;
		pVertices->p.z = m_fPosZ + sinf( 2.f*D3DX_PI*( (float)(i+1)/(float)m_dwNumFaces ) )*m_fDownLong;
		pVertices->d = D3DCOLOR_ARGB ( 100, 255, 255, 255 );
		pVertices->t = D3DXVECTOR2 ( 1.f, 1.f );
		pVertices++;
	}
	m_pVB->Unlock ();

	WORD*	pIndices;
	m_pIB->Lock ( 0, 0, (BYTE**)&pIndices, 0 );

	for ( DWORD i=0; i<m_dwNumFaces; i++ )
	{
		*pIndices++ = (WORD)( (i*4)+0 );
		*pIndices++ = (WORD)( (i*4)+1 );
		*pIndices++ = (WORD)( (i*4)+2 );
		*pIndices++ = (WORD)( (i*4)+1 );
		*pIndices++ = (WORD)( (i*4)+3 );
		*pIndices++ = (WORD)( (i*4)+2 );
	}
	m_pIB->Unlock ();

	return S_OK;
}

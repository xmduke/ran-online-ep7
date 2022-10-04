// Terrain0.cpp: implementation of the DxEffectTile class.
//
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "StlFunctions.h"

#include "TextureManager.h"
#include "EditMeshs.h"
#include "DxMaterial.h"
#include "DxFrameMesh.h"
#include "DxEffectDefine.h"

#include "SerialFile.h"
#include "Collision.h"

#include "DxEffectMan.h"

#include "DxEffectTile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

const DWORD DxEffectTile::TYPEID = DEF_EFFECT_TILE;
const DWORD	DxEffectTile::VERSION = 0x00000100;
const char DxEffectTile::NAME[] = "[ 타일 시스템 : BASE ]";
const DWORD DxEffectTile::FLAG = _EFF_REPLACE;

const DWORD		DxEffectTile::VERTEX::FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
const DWORD		DxEffectTile::VERTEX_TEX2::FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2;

DWORD		DxEffectTile::m_dwSavedSB_Detail		= NULL;
DWORD		DxEffectTile::m_dwDrawSB_Detail			= NULL;

void DxEffectTile::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(TILE_PROPERTY);
	dwVer = VERSION;
}

void DxEffectTile::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(TILE_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DxEffectTile::DxEffectTile () :
	m_dwCol(0),
	m_dwRow(0),
	m_pDetailTex(NULL)
{
	strcpy ( m_szDetailTex, "_Eff_detail.bmp" );

	m_strBaseName = "World_";
	m_strExtName = ".dds";

	UseAffineMatrix ();
	D3DXMatrixIdentity ( &m_matFrameComb );
}

DxEffectTile::~DxEffectTile ()
{
	std::for_each ( m_mapBase.begin(), m_mapBase.end(), std_afunc::DeleteMapObject() );
	m_mapBase.clear();
	m_mapFace.clear();
}

HRESULT DxEffectTile::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;

	if ( pframeCur->pmsMeshs != NULL )
	{
		pmsMeshs = pframeCur->pmsMeshs;

		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				SetBaseQuad( pd3dDevice, pmsMeshs );
			}

			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	return hr;
}

//	Note : DxFrame 에 효과를 붙일 경우에 사용된다.
//
HRESULT DxEffectTile::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICE8 pd3dDevice )
{
	if ( !pFrame )		return S_OK;

	m_pAdaptFrame = pFrame;
	m_matFrameComb = pFrame->matCombined;

	this->pLocalFrameEffNext = pFrame->pEffect;
	pFrame->pEffect = this;

	GASSERT(pFrame->szName);
	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		m_szAdaptFrame = new char[strlen(pFrame->szName)+1];

		strcpy ( m_szAdaptFrame, pFrame->szName );
	}

	return AdaptToDxFrameChild ( pFrame, pd3dDevice );
}

HRESULT DxEffectTile::AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICE8 pd3dDevice )
{
	GASSERT ( pFrame );

	m_pAdaptFrame = NULL;
	m_matFrameComb = pFrame->matCombined;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		m_szAdaptFrame = new char[strlen(pFrame->szName)+1];

		strcpy ( m_szAdaptFrame, pFrame->szName );
	}

	return S_OK;
}

HRESULT DxEffectTile::OneTimeSceneInit ()
{
	HRESULT hr = S_OK;

	return hr;
}

HRESULT DxEffectTile::CreateDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE2X );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2,	D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE2X );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_dwSavedSB_Detail );
		else			pd3dDevice->EndStateBlock( &m_dwDrawSB_Detail );
	}

	return S_OK;
}

HRESULT DxEffectTile::ReleaseDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	pd3dDevice->DeleteStateBlock ( m_dwSavedSB_Detail );
	pd3dDevice->DeleteStateBlock ( m_dwDrawSB_Detail );

	m_dwSavedSB_Detail = NULL;
	m_dwDrawSB_Detail = NULL;

	return S_OK;
}

HRESULT DxEffectTile::InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	TextureManager::LoadTexture ( m_szDetailTex, pd3dDevice, m_pDetailTex, 0, 0 );

	return S_OK;
}

HRESULT DxEffectTile::RestoreDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffectTile::InvalidateDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffectTile::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture ( m_pDetailTex );
	m_pDetailTex = NULL;

	return S_OK;
}

HRESULT DxEffectTile::FinalCleanup ()
{
	return S_OK;
}

//	Note : 랜더링 되지 않을때 필요 없는 부분을 제거하고
//		랜더링 되지 않을때의 부하 경감을 위해 가장 단순하게 유지한다.
//
HRESULT DxEffectTile::FrameMove ( float fTime, float fElapsedTime )
{
	//m_listSelectTriangle.clear();

	//m_dwCurPOINT_PREV = m_dwCurPOINT;

	return S_OK;
}

HRESULT DxEffectTile::Render ( DxFrame *pFrame, LPDIRECT3DDEVICE8 pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	PROFILE_BEGIN("DxEffectTile");

	HRESULT hr = S_OK;

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );

	if ( pFrameMesh )
	{
		pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );
		RenderTiles ( pd3dDevice );
	}

	PROFILE_END("DxEffectTile");

	return S_OK;
}

void DxEffectTile::GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	vMin = -vMax;

	D3DXMATRIXA16 matCombined;
	D3DXMatrixMultiply ( &matCombined, m_pmatLocal, &m_matFrameComb );

	COLLISION::TransformAABB ( vMax, vMin, matCombined );
}

BOOL DxEffectTile::IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
{
	D3DXMATRIXA16 matCombined;
	D3DXMatrixMultiply ( &matCombined, m_pmatLocal, &m_matFrameComb );

	D3DXVECTOR3 vCenter;
	vCenter.x = matCombined._41;
	vCenter.y = matCombined._42;
	vCenter.z = matCombined._43;

	return COLLISION::IsWithInPoint(vDivMax,vDivMin,vCenter);
}

void DxEffectTile::SaveBuffer ( CSerialFile &SFile )
{

}

void DxEffectTile::LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICE8 pd3dDevice )
{
	if ( VERSION==dwVer )
	{
	//	DWORD dwCur = SFile.GetfTell();
	//	SFile.SetOffSet ( dwCur+dwBuffSize_Base );
		return;
	}
	else
	{
	//	DWORD dwCur = SFile.GetfTell();
	//	SFile.SetOffSet ( dwCur+dwBuffSize_Base+dwBuffSize_Blend );
		return;
	}
}

void DxEffectTile::LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICE8 pd3dDevice )
{
	if ( VERSION==dwVer )
	{
	//	DWORD dwCur = SFile.GetfTell();
	//	SFile.SetOffSet ( dwCur+dwBuffSize_Base );
		return;
	}
	else
	{
	//	DWORD dwCur = SFile.GetfTell();
	//	SFile.SetOffSet ( dwCur+dwBuffSize_Base+dwBuffSize_Blend );
		return;
	}
}

HRESULT DxEffectTile::CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICE8 pd3dDevice )
{
	GASSERT(pSrcEffect);
	DxEffectTile *pEffSrc = (DxEffectTile*)pSrcEffect;

	m_matFrameComb	= pEffSrc->m_matFrameComb;

	return S_OK;
}

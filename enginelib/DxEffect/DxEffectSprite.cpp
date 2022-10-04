// DxEffectSprite.cpp: implementation of the DxEffectBlurC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DxOctreeMesh.h"
#include "DxFrameMesh.h"
#include "DxEffectMan.h"
#include "SerialFile.h"

#include "DxEffectSprite.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

const DWORD DxEffectSprite::VERTEX::FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
const DWORD DxEffectSprite::TYPEID = DEF_EFFECT_SPRITE;
const DWORD	DxEffectSprite::VERSION = 0x00000100;
const char DxEffectSprite::NAME[] = "[3006]_[ 광원 ]";
const DWORD DxEffectSprite::FLAG = _EFF_REPLACE;

void DxEffectSprite::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(SPRITE_PROPERTY);
	dwVer = VERSION;
}

void DxEffectSprite::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(SPRITE_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
}

DWORD	DxEffectSprite::m_dwSavedStateBlock(0x00000000);
DWORD	DxEffectSprite::m_dwEffectStateBlock(0x00000000);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxEffectSprite::DxEffectSprite() :
	m_pOcMesh(NULL),
	m_pddsTexture(NULL),
	m_fTime(0.0f),
	m_fDistance(5.f),
	m_vCenter(D3DXVECTOR3(0.f,0.f,0.f)),
	m_fWidth(30.0f),
	m_fHeight(30.0f),
	m_iCol(1),
	m_iRow(1),
	m_fAniTime(0.05f),
	m_dwColor(D3DCOLOR_ARGB( 100, 255, 255, 100 )),
	iNowSpriteNum(0)
{
	strcpy ( m_szTexture, "_Eff_flare.tga" );
}

DxEffectSprite::~DxEffectSprite()
{
	SAFE_DELETE_ARRAY(m_szAdaptFrame);
}

HRESULT DxEffectSprite::CreateDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		//
	//	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE, FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,			D3DCULL_CW );
		pd3dDevice->SetRenderState ( D3DRS_ALPHAREF,			0x00 );


		if( which==0 )	pd3dDevice->EndStateBlock( &m_dwSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_dwEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffectSprite::ReleaseDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
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

HRESULT DxEffectSprite::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	if ( pframeCur->pmsMeshs != NULL )
	{
		pmsMeshs = pframeCur->pmsMeshs;

		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				MakeEffSprite( pd3dDevice, pmsMeshs->m_pLocalMesh );
			}

			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	//	Note : 자식 프레임에 붙이기.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = AdaptToDxFrameChild ( pframeChild, pd3dDevice );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return hr;
}

HRESULT DxEffectSprite::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICE8 pd3dDevice )
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

HRESULT DxEffectSprite::AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICE8 pd3dDevice )
{
	assert ( pFrame );

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		m_szAdaptFrame = new char[strlen(pFrame->szName)+1];

		strcpy ( m_szAdaptFrame, pFrame->szName );
	}

	return S_OK;
}


HRESULT DxEffectSprite::InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
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

HRESULT DxEffectSprite::DeleteDeviceObjects ()
{
	//	Note : 이전 텍스쳐가 로드되어 있을 경우 제거.
	//
	if ( m_pddsTexture )
		TextureManager::ReleaseTexture ( m_pddsTexture );
	m_pddsTexture = NULL;

	return S_OK;
}

HRESULT DxEffectSprite::FrameMove ( float fTime, float fElapsedTime )
{
	m_fTime += fElapsedTime;

	return S_OK;
}

HRESULT DxEffectSprite::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICE8 pd3dDevice )
{
	//	Note : 원본 뿌리기
	//
	if ( m_pOcMesh )
	{
		D3DXMATRIX matIdentity, matWorld;
		D3DXMatrixIdentity( &matIdentity );

		pd3dDevice->GetTransform( D3DTS_WORLD, &matWorld );


		pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

		m_pOcMesh->Render ( pd3dDevice );

		pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );
	}

	//	Note : 스프라이트
	//
	if ( m_pSpriteMesh )
	{
		int iSpriteNum = m_iCol * m_iRow;

		if ( m_fTime > m_fAniTime )
		{
			iNowSpriteNum += 1;
			m_fTime = 0.0f;
		}

		if ( iNowSpriteNum >= iSpriteNum )
		{
			iNowSpriteNum = 0;
		}

		DWORD					dwFVFSize;
		DWORD					dwVerts, dwFaces;
		LPDIRECT3DVERTEXBUFFER8 pVB	= NULL;
	
		m_pSpriteMesh->GetVertexBuffer ( &pVB );
		
		dwFVFSize = m_pSpriteMesh->GetFVF ();
		dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );

		dwVerts = m_pSpriteMesh->GetNumVertices();
		dwFaces = m_pSpriteMesh->GetNumFaces();



		D3DXMATRIX matIdentity, matWorld, matWorldSub;
		D3DXMatrixIdentity ( &matIdentity );

		pd3dDevice->GetTransform( D3DTS_WORLD, &matWorld );

		pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );



		//	 빌보드를 합니다...
		//
		D3DXVECTOR3 vWorldCenter = m_vCenter;//MatrixVertex ( &pframeCur->matCombined, &m_vCenter );

		D3DXVECTOR3 &vFromPt = DxViewPort::Instance.GetFromPt ();

		D3DXVECTOR3 Sub11( 0.f, 0.f, 0.f );		// 물체에서 카메라로
		Sub11.x = vFromPt.x - vWorldCenter.x;
		Sub11.y = vFromPt.y - vWorldCenter.y;
		Sub11.z = vFromPt.z - vWorldCenter.z;
		D3DXVec3Normalize ( &Sub11, &Sub11 );
		D3DXVECTOR3 vSubDis11;
		vSubDis11 = Sub11 * m_fDistance;

		vWorldCenter += vSubDis11;

		//	Note : vWorldCenter -> vLookatPt 로 바꿔 주어야지
		D3DXMATRIX matBillboard = BillboardLookAt ( vWorldCenter, vFromPt );	




		D3DXVECTOR3 vWorld1 = D3DXVECTOR3( -m_fWidth, m_fHeight, 0.0f );
		D3DXVECTOR3 vWorld2 = D3DXVECTOR3( m_fWidth, m_fHeight, 0.0f );
		D3DXVECTOR3 vWorld3 = D3DXVECTOR3( -m_fWidth, -m_fHeight, 0.0f );
		D3DXVECTOR3 vWorld4 = D3DXVECTOR3( m_fWidth, -m_fHeight, 0.0f );

		D3DXVec3TransformCoord ( &vWorld1, &vWorld1, &matBillboard );
		D3DXVec3TransformCoord ( &vWorld2, &vWorld2, &matBillboard );
		D3DXVec3TransformCoord ( &vWorld3, &vWorld3, &matBillboard );
		D3DXVec3TransformCoord ( &vWorld4, &vWorld4, &matBillboard );

		QUAD*			pVertexC;

		m_pSpriteMesh->LockVertexBuffer( 0L, (BYTE**)&pVertexC );

		pVertexC->p = vWorld1;
		pVertexC->d = m_dwColor;
		pVertexC->t.x = iNowSpriteNum / (float)m_iCol;
		pVertexC->t.y = (iNowSpriteNum / m_iRow) / (float)m_iRow;
		pVertexC++;
		pVertexC->p = vWorld2;
		pVertexC->d = m_dwColor;
		pVertexC->t.x = (iNowSpriteNum+1) / (float)m_iCol;
		pVertexC->t.y = (iNowSpriteNum / m_iRow) / (float)m_iRow;
		pVertexC++;
		pVertexC->p = vWorld3;
		pVertexC->d = m_dwColor;
		pVertexC->t.x = iNowSpriteNum / (float)m_iCol;
		pVertexC->t.y = ((iNowSpriteNum+m_iRow) / m_iRow) / (float)m_iRow;
		pVertexC++;
		pVertexC->p = vWorld4;
		pVertexC->d = m_dwColor;
		pVertexC->t.x = (iNowSpriteNum+1) / (float)m_iCol;
		pVertexC->t.y = ((iNowSpriteNum+m_iRow) / m_iRow) / (float)m_iRow;

		m_pSpriteMesh->UnlockVertexBuffer();

		pd3dDevice->SetTexture ( 0, m_pddsTexture );


		//	Note : 이전 상태 백업.
		//
		pd3dDevice->CaptureStateBlock ( m_dwSavedStateBlock );

		//	Note : 랜더링 상태 조정.
		//
		pd3dDevice->ApplyStateBlock ( m_dwEffectStateBlock );



		pd3dDevice->SetIndices ( NULL, 0 );
		pd3dDevice->SetStreamSource ( 0, pVB, dwFVFSize );
		pd3dDevice->SetVertexShader ( m_pSpriteMesh->GetFVF () );

		pd3dDevice->DrawPrimitive ( D3DPT_TRIANGLESTRIP, 0, 2 );

		pd3dDevice->SetIndices ( NULL, 0 );
		pd3dDevice->SetStreamSource ( 0, NULL, 0 );

		//	Note : 이전상태로 복원.
		//
		pd3dDevice->ApplyStateBlock ( m_dwSavedStateBlock );

		pd3dDevice->SetTexture ( 0, NULL );

		pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );

		if(pVB) pVB->Release ();

/*
		else
		{
		//	pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CCW );

			D3DXVECTOR3 &vFromPt = DxViewPort::Instance.GetFromPt ();
			D3DXVECTOR3 &vLookatPt = DxViewPort::Instance.GetLookatPt ();

			D3DXVECTOR3 vWorldCenter = MatrixVertex ( &pframeCur->matCombined, &m_vCenter );
			D3DXVECTOR3 vWorld1 = D3DXVECTOR3( -m_fWidth + m_vCenter.x, m_fHeight + m_vCenter.y, m_vCenter.z );
			D3DXVECTOR3 vWorld2 = D3DXVECTOR3( m_fWidth + m_vCenter.x, m_fHeight + m_vCenter.y, m_vCenter.z );
			D3DXVECTOR3 vWorld3 = D3DXVECTOR3( -m_fWidth + m_vCenter.x, -m_fHeight + m_vCenter.y, m_vCenter.z );
			D3DXVECTOR3 vWorld4 = D3DXVECTOR3( m_fWidth + m_vCenter.x, -m_fHeight + m_vCenter.y, m_vCenter.z );
			vWorld1 = MatrixVertex ( &pframeCur->matCombined, &vWorld1 );
			vWorld2 = MatrixVertex ( &pframeCur->matCombined, &vWorld2 );
			vWorld3 = MatrixVertex ( &pframeCur->matCombined, &vWorld3 );
			vWorld4 = MatrixVertex ( &pframeCur->matCombined, &vWorld4 );

			


			D3DXVECTOR3 Sub11( 0.f, 0.f, 0.f );		// 물체에서 카메라로
			Sub11.x = vFromPt.x - vWorldCenter.x;
			Sub11.y = vFromPt.y - vWorldCenter.y;
			Sub11.z = vFromPt.z - vWorldCenter.z;
			D3DXVec3Normalize ( &Sub11, &Sub11 );
			D3DXVECTOR3 vSubDis11;
			vSubDis11 = Sub11 * m_fDistance;


			D3DXVECTOR3 Sub( 0.f, 0.f, 0.f );		// 보는 위치에서 카메라로
			Sub.x = vFromPt.x - vLookatPt.x;
			Sub.y = vFromPt.y - vLookatPt.y;
			Sub.z = vFromPt.z - vLookatPt.z;
			D3DXVec3Normalize ( &Sub, &Sub );
			D3DXVECTOR3 vSubDis;
			vSubDis = Sub * m_fDistance;


	
			//	Note : Shader 상수 선언
			//

			//	Note : SetRenderState() 선언
			//
			



			pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

			//	Note : 1 단계	-	중심점 찾기
			//
			D3DXVECTOR3 vWorldCenterSub;
			vWorldCenterSub.x = vWorldCenter.x + vSubDis11.x;	// 노멀을 물체가 보는 방향으로 해야 한다.
			vWorldCenterSub.y = vWorldCenter.y + vSubDis11.y;
			vWorldCenterSub.z = vWorldCenter.z + vSubDis11.z;


			//	Note : 2 단계	-	면 만들기
			//
			D3DXPLANE	vPlane;												// 노멀을 카메라가 보는 방향으로 해야 한다.
			D3DXPlaneFromPointNormal ( &vPlane, &vWorldCenterSub, &Sub );


			//	Note : 3 단계	-	선분 만들기
			//
			D3DXVECTOR3 vWorldLine1 = vWorld1 + (vSubDis11*10);
			D3DXVECTOR3 vWorldLine2 = vWorld2 + (vSubDis11*10);
			D3DXVECTOR3 vWorldLine3 = vWorld3 + (vSubDis11*10);
			D3DXVECTOR3 vWorldLine4 = vWorld4 + (vSubDis11*10);


			//	Note : 4 단계	-	4 개의 교점 찾기
			//
			vWorldLine1 = Split ( &vPlane, &vWorld1, &vWorldLine1 );
			vWorldLine2 = Split ( &vPlane, &vWorld2, &vWorldLine2 );
			vWorldLine3 = Split ( &vPlane, &vWorld3, &vWorldLine3 );
			vWorldLine4 = Split ( &vPlane, &vWorld4, &vWorldLine4 );


			//	Note : 5 단계	-	거리 계산 
			//


			//	Note : 6 단계	-	거리가 좁다?? 그럼 늘려야쥐.
			//			
			//	6-1 단계	-	실제 X 를 가져와야지
			float RealX = sqrtf( powf(vWorld2.x-vWorld1.x,2) + powf(vWorld2.z-vWorld1.z,2) );
			//	6-2 단계	-	뿌려지는 X 를 가져와야지
			float RenderX = sqrtf( powf(vWorldLine2.x-vWorldLine1.x,2) + powf(vWorldLine2.z-vWorldLine1.z,2) );
			//	6-3 단계	-	실제 X 와 뿌려지는 X의 차를 구해야지	= 비율이 나왔다	
			float RRX = ((m_fWidth*2) - RenderX)/(m_fWidth*2);
			//	6-4 단계	-	뿌려지는 X + 차이*계산된 수지 해줘야지			Sub11


			D3DXVECTOR3 vkk1 = vWorldLine1;
			D3DXVECTOR3 vkk2 = vWorldLine2;
			vkk1.x = vWorldLine1.x + (RRX * m_fRateX * m_fWidth)*(Sub11.z);
			vkk1.z = vWorldLine1.z + (RRX * m_fRateX * m_fWidth)*(-Sub11.x);
			vkk2.x = vWorldLine2.x + (RRX * m_fRateX * m_fWidth)*(-Sub11.z);
			vkk2.z = vWorldLine2.z + (RRX * m_fRateX * m_fWidth)*(Sub11.x);

			D3DXPLANE	vPlane1;
			D3DXVECTOR3 vkkUp = D3DXVECTOR3 ( vFromPt.x, vFromPt.y+1.0f, vFromPt.z );
			D3DXPlaneFromPoints ( &vPlane1, &vWorldCenter, &vFromPt, &vkkUp );

			float NormalPM = (vPlane1.a * vWorldLine1.x) + (vPlane1.b * vWorldLine1.y) + (vPlane1.c * vWorldLine1.z);


			if( NormalPM < 0 )
			{
				vWorldLine1.x = vWorldLine1.x + (RRX * m_fRateX * m_fWidth)*(Sub.z);
				vWorldLine1.z = vWorldLine1.z + (RRX * m_fRateX * m_fWidth)*(-Sub.x);
				vWorldLine2.x = vWorldLine2.x + (RRX * m_fRateX * m_fWidth)*(-Sub.z);
				vWorldLine2.z = vWorldLine2.z + (RRX * m_fRateX * m_fWidth)*(Sub.x);

				vWorldLine3.x = vWorldLine3.x + (RRX * m_fRateX * m_fWidth)*(Sub.z);
				vWorldLine3.z = vWorldLine3.z + (RRX * m_fRateX * m_fWidth)*(-Sub.x);
				vWorldLine4.x = vWorldLine4.x + (RRX * m_fRateX * m_fWidth)*(-Sub.z);
				vWorldLine4.z = vWorldLine4.z + (RRX * m_fRateX * m_fWidth)*(Sub.x);
			}
			else
			{
				vWorldLine1.x = vWorldLine1.x + (RRX * m_fRateX * m_fWidth)*(-Sub.z);
				vWorldLine1.z = vWorldLine1.z + (RRX * m_fRateX * m_fWidth)*(Sub.x);
				vWorldLine2.x = vWorldLine2.x + (RRX * m_fRateX * m_fWidth)*(Sub.z);
				vWorldLine2.z = vWorldLine2.z + (RRX * m_fRateX * m_fWidth)*(-Sub.x);

				vWorldLine3.x = vWorldLine3.x + (RRX * m_fRateX * m_fWidth)*(-Sub.z);
				vWorldLine3.z = vWorldLine3.z + (RRX * m_fRateX * m_fWidth)*(Sub.x);
				vWorldLine4.x = vWorldLine4.x + (RRX * m_fRateX * m_fWidth)*(Sub.z);
				vWorldLine4.z = vWorldLine4.z + (RRX * m_fRateX * m_fWidth)*(-Sub.x);

				D3DXVECTOR3 vMid = (vWorldLine1 + vWorldLine2)/2;
				vWorldLine1 = vMid + vMid - vWorldLine1;
				vWorldLine2 = vMid + vMid - vWorldLine2;

				vMid = (vWorldLine3 + vWorldLine4)/2;
				vWorldLine3 = vMid + vMid - vWorldLine3;
				vWorldLine4 = vMid + vMid - vWorldLine4;

				float	fTempCh;
				fTempCh = vWorldLine1.y;
				vWorldLine1.y = vWorldLine2.y;
				vWorldLine2.y = fTempCh;

				fTempCh = vWorldLine3.y;
				vWorldLine3.y = vWorldLine4.y;
				vWorldLine4.y = fTempCh;
			}

			//	Note : 7 단계	-	완성된 점 집어 넣기
			//
			QUAD*			pVertexC;

			m_pSpriteMesh->LockVertexBuffer( 0L, (BYTE**)&pVertexC );

			pVertexC->p = vWorldLine1;
			pVertexC->d = m_dwColor;
			pVertexC->t.x = iNowSpriteNum / (float)m_iCol;
			pVertexC->t.y = (iNowSpriteNum / m_iRow) / (float)m_iRow;
			pVertexC++;
			pVertexC->p = vWorldLine2;
			pVertexC->d = m_dwColor;
			pVertexC->t.x = (iNowSpriteNum+1) / (float)m_iCol;
			pVertexC->t.y = (iNowSpriteNum / m_iRow) / (float)m_iRow;
			pVertexC++;
			pVertexC->p = vWorldLine3;
			pVertexC->d = m_dwColor;
			pVertexC->t.x = iNowSpriteNum / (float)m_iCol;
			pVertexC->t.y = ((iNowSpriteNum+m_iRow) / m_iRow) / (float)m_iRow;
			pVertexC++;
			pVertexC->p = vWorldLine4;
			pVertexC->d = m_dwColor;
			pVertexC->t.x = (iNowSpriteNum+1) / (float)m_iCol;
			pVertexC->t.y = ((iNowSpriteNum+m_iRow) / m_iRow) / (float)m_iRow;

			m_pSpriteMesh->UnlockVertexBuffer();

		}

*/

	}

	return S_OK;
}

HRESULT DxEffectSprite::MakeEffSprite (LPDIRECT3DDEVICE8 pd3dDevice, LPD3DXMESH pLocalMesh )
{
//	QUAD* pVertices;
	DWORD	dwFVF		= pLocalMesh->GetFVF();
	DWORD	dwVertices	= pLocalMesh->GetNumVertices();
/*
	float	fUp, fDown, fLeft, fRight, fZ1, fZ2, BestZ;

	pLocalMesh->LockVertexBuffer( 0L, (BYTE**)&pVertices );

	for( DWORD i=0; i<dwVertices; i++ )
	{
		if( i == 0 )
		{
			fRight	= fLeft = pVertices->p.x;
			fUp		= fDown = pVertices->p.y;
			fZ1		= fZ2	= pVertices->p.z;
		}

		fRight = ( fRight < pVertices->p.x ) ? pVertices->p.x : fRight;
		fLeft = ( fLeft > pVertices->p.x ) ? pVertices->p.x : fLeft;
		fUp = ( fUp < pVertices->p.y ) ? pVertices->p.y : fUp;
		fDown = ( fDown > pVertices->p.y ) ? pVertices->p.y : fDown;
		fZ1 = ( fZ1 < pVertices->p.z ) ? pVertices->p.z : fZ1;
		fZ2 = ( fZ2 > pVertices->p.z ) ? pVertices->p.z : fZ2;

        pVertices++;
	}

	pLocalMesh->UnlockVertexBuffer();

	m_fWidth = ( fRight < fLeft ) ? fLeft : fRight;
	m_fHeight = ( fUp < fDown ) ? fDown : fUp;
	BestZ = ( fZ2 < fZ1 ) ? fZ1 : fZ2;
*/
	QUAD*			pVertexC;

	D3DXCreateMeshFVF( 2,	//	TRIANGLESTRIP 이라서 -2 
						4,
						D3DXMESH_SYSTEMMEM, 
						D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1,
						pd3dDevice,
						&m_pSpriteMesh );

	m_pSpriteMesh->LockVertexBuffer( 0L, (BYTE**)&pVertexC );

	pVertexC->p = D3DXVECTOR3( -m_fWidth, m_fHeight, 0.f );
	pVertexC->d = D3DCOLOR_ARGB( 100, 255, 255, 100 );
	pVertexC->t = D3DXVECTOR2( 0.f, 0.f );
	pVertexC++;
	pVertexC->p = D3DXVECTOR3( m_fWidth, m_fHeight, 0.f );
	pVertexC->d = D3DCOLOR_ARGB( 100, 255, 255, 100 );
	pVertexC->t = D3DXVECTOR2( 1.f, 0.f );
	pVertexC++;
	pVertexC->p = D3DXVECTOR3( -m_fWidth, -m_fHeight, 0.f );
	pVertexC->d = D3DCOLOR_ARGB( 100, 255, 255, 100 );
	pVertexC->t = D3DXVECTOR2( 0.f, 1.f );
	pVertexC++;
	pVertexC->p = D3DXVECTOR3( m_fWidth, -m_fHeight, 0.f );
	pVertexC->d = D3DCOLOR_ARGB( 100, 255, 255, 100 );
	pVertexC->t = D3DXVECTOR2( 1.f, 1.f );
	pVertexC++;	

	m_pSpriteMesh->UnlockVertexBuffer();


	return S_OK;
}

D3DMATRIX DxEffectSprite::BillboardLookAt ( D3DXVECTOR3 BillboardPos, D3DXVECTOR3 CarmeraPos )
{
	D3DXMATRIX	Matrix;
	D3DXVECTOR3	View = CarmeraPos - BillboardPos;
	D3DXVec3Normalize ( &View, &View );

	D3DXVECTOR3 WorldUp = D3DXVECTOR3 ( 0.0f, 1.0f, 0.0f );

	float angle = D3DXVec3Dot ( &WorldUp, &View );
	D3DXVECTOR3	Up = WorldUp - angle*View;
	D3DXVec3Normalize ( &Up, &Up );

	D3DXVECTOR3	Right;
	D3DXVec3Cross ( &Right, &Up, &View );

	Matrix._11 = Right.x;	Matrix._21 = Up.x;	Matrix._31 = View.x;
	Matrix._12 = Right.y;	Matrix._22 = Up.y;	Matrix._32 = View.y;
	Matrix._13 = Right.z;	Matrix._23 = Up.z;	Matrix._33 = View.z;

	Matrix._41 = BillboardPos.x;
	Matrix._42 = BillboardPos.y;
	Matrix._43 = BillboardPos.z;

	Matrix._14 = 0.0f;
	Matrix._24 = 0.0f;
	Matrix._34 = 0.0f;
	Matrix._44 = 1.0f;

	return Matrix;
}

D3DXVECTOR3 DxEffectSprite::MatrixVertex ( D3DMATRIX *matWorld, D3DXVECTOR3 *vPos )
{
	D3DXVECTOR3 vChangePos;

	vChangePos.x = (matWorld->_11 * vPos->x) + (matWorld->_21 * vPos->y) + (matWorld->_31 * vPos->z) + matWorld->_41;
	vChangePos.y = (matWorld->_12 * vPos->x) + (matWorld->_22 * vPos->y) + (matWorld->_32 * vPos->z) + matWorld->_42;
	vChangePos.z = (matWorld->_13 * vPos->x) + (matWorld->_23 * vPos->y) + (matWorld->_33 * vPos->z) + matWorld->_43;

	return vChangePos;
}

D3DXVECTOR3 DxEffectSprite::Split ( D3DXPLANE *vPlane, D3DXVECTOR3 *vPos1, D3DXVECTOR3 *vPos2 )
{
	D3DXVECTOR3 vPlaneNormal;
	vPlaneNormal = D3DXVECTOR3 ( vPlane->a, vPlane->b, vPlane->c );	// 면의 노멀

    float aDot = D3DXVec3Dot ( &(*vPos1), &(vPlaneNormal) );
	float bDot = D3DXVec3Dot ( &(*vPos2), &(vPlaneNormal) );

	float scale = ( - vPlane->d - aDot ) / ( bDot - aDot );

	D3DXVECTOR3 vChangePos = *vPos1 + ( scale * ( *vPos2 - *vPos1 ) );	//vPos1 + 

	return vChangePos;
}

void DxEffectSprite::SaveBuffer ( CSerialFile &SFile )
{
	m_pOcMesh->SaveFile ( SFile );
}

void DxEffectSprite::LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICE8 pd3dDevice )
{
	//if ( dwVer == 0x0100 )
	//{
	//	DWORD dwBuffSize;
	//	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

	//	return;
	//}

	m_pOcMesh->LoadFile ( SFile, pd3dDevice );
}

void DxEffectSprite::LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICE8 pd3dDevice )
{
	//if ( dwVer == 0x0100 )
	//{
	//	DWORD dwBuffSize;
	//	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

	//	if ( m_pOcMesh )
	//	{
	//		SAFE_DELETE ( m_pOcMesh );
	//		m_pOcMesh = NULL;
	//	}
	//	return;
	//}

	m_pOcMesh->LoadFile ( SFile, pd3dDevice );
}

HRESULT DxEffectSprite::CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICE8 pd3dDevice )
{
	DxEffectSprite* pSrcSprite = (DxEffectSprite*) pSrcEffect;
	assert(pSrcSprite->m_pSpriteMesh);
	assert(pd3dDevice);

	m_fDistance	= pSrcSprite->m_fDistance;
	m_fWidth	= pSrcSprite->m_fWidth;
	m_fHeight	= pSrcSprite->m_fHeight;
	m_dwColor	= pSrcSprite->m_dwColor;
	m_fAniTime	= pSrcSprite->m_fAniTime;
	m_iCol		= pSrcSprite->m_iCol;
	m_iRow		= pSrcSprite->m_iRow;

	if ( pSrcSprite->m_pSpriteMesh )
	{
		QUAD*			pSrcVertex;
		QUAD*			pDestVertex;
	//	WORD*			pSrcIndex;
	//	WORD*			pDestindex;

		D3DXCreateMeshFVF( 2,	//	TRIANGLESTRIP 이라서 -2 
						4,
						D3DXMESH_SYSTEMMEM, 
						D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1,
						pd3dDevice,
						&m_pSpriteMesh );

		pSrcSprite->m_pSpriteMesh->LockVertexBuffer ( 0, (BYTE**)&pSrcVertex );
		m_pSpriteMesh->LockVertexBuffer ( 0, (BYTE**)&pDestVertex );
		memcpy ( pDestVertex, pSrcVertex, sizeof(QUAD)*4 );
		m_pSpriteMesh->UnlockVertexBuffer ();
		pSrcSprite->m_pSpriteMesh->UnlockVertexBuffer ();

		//pSrcSprite->m_pSpriteMesh->LockIndexBuffer ( 0, (BYTE**)&pSrcIndex );
		//m_pSpriteMesh->LockIndexBuffer ( 0, (BYTE**)&pDestindex );
		//memcpy ( pDestindex, pSrcIndex, sizeof(WORD)*4 );
		//m_pSpriteMesh->UnlockIndexBuffer ();
		//pSrcSprite->m_pSpriteMesh->UnlockIndexBuffer ();
	}

	if ( pSrcSprite->m_pOcMesh )
	{
		SAFE_DELETE(m_pOcMesh);
		m_pOcMesh = new DxOcMeshes;
		m_pOcMesh->CloneMesh ( pd3dDevice, pSrcSprite->m_pOcMesh, NULL );
	}
	
	return S_OK;
}
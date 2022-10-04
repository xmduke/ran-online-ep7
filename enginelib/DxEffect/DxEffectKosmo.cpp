// DxEffectKosmo.cpp: implementation of the DxEffectToon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DxFrameMesh.h"
#include "DxEffectMan.h"

#include "DxEffectKosmo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

const DWORD DxEffectKosmo::KOSMOSTRIP::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
const DWORD DxEffectKosmo::TYPEID = DEF_EFFECT_CARTOON;
const DWORD	DxEffectKosmo::VERSION = 0x00000100;
const char DxEffectKosmo::NAME[] = "[3010]_[ 마법 효과 ]";
const DWORD DxEffectKosmo::FLAG = NULL;

void DxEffectKosmo::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(KOSMO_PROPERTY);
	dwVer = VERSION;
}

void DxEffectKosmo::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(KOSMO_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////6.f/32.f , 0.35f, 31.f/32.f, 0.4f

DxEffectKosmo::DxEffectKosmo() :
	m_iTemp(0),
	m_dwVertex(20),
	m_dwFace(25),
	m_vPos(D3DXVECTOR3(0.f,0.f,0.f)),
	LineStart(D3DXVECTOR3(0.f,2.0f,0.f)),
	LineEnd(D3DXVECTOR3(0.f,10.0f,0.f)),
	m_fWidth(1.f),
	m_fHeight(4.f),
	m_fScale(0.9f),
	m_pddsTexture(NULL),
	m_fTime(0.0f),
	m_fElapsedTime(0.0f),
	m_iScene(5),
	m_pBaseVertex(NULL),
	m_pVB(NULL),
	m_pIB(NULL)
{
	strcpy ( m_szTexture, "_1d_glow1.bmp" );
}

DxEffectKosmo::~DxEffectKosmo()
{
	SAFE_DELETE_ARRAY(m_szAdaptFrame);

	SAFE_DELETE_ARRAY ( m_pBaseVertex );
	SAFE_RELEASE ( m_pVB );
	SAFE_RELEASE ( m_pIB );
}

HRESULT DxEffectKosmo::CreateDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT			hr;
	LPD3DXBUFFER	pCode = NULL; 

	DWORD dwDecl[] =
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG(0, D3DVSDT_FLOAT3 ),
		D3DVSD_REG(1, D3DVSDT_FLOAT3 ),
		D3DVSD_REG(2, D3DVSDT_FLOAT4 ),
		D3DVSD_REG(3, D3DVSDT_FLOAT3 ),
		D3DVSD_REG(5, D3DVSDT_FLOAT1 ),
		D3DVSD_END()
	};

	hr = D3DXAssembleShader( m_strEffect, (UINT)strlen( m_strEffect ), 0, NULL, &pCode, NULL );
	if( FAILED(hr) )	return E_FAIL;

	hr = pd3dDevice->CreateVertexShader ( dwDecl, (DWORD*)pCode->GetBufferPointer(), &m_dwEffect, 
											DxEffectMan::Instance.GetUseSwShader() );
	if( FAILED(hr) )	return E_FAIL;

	SAFE_RELEASE ( pCode );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetTextureStageState() 선언
		//
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE ,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,			D3DCULL_NONE );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,	D3DBLEND_ONE );
		//pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,	D3DBLEND_ONE );
		
		//pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );
		//pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );


		if( which==0 )	pd3dDevice->EndStateBlock( &m_dwSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_dwEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffectKosmo::ReleaseDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
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

HRESULT DxEffectKosmo::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr = S_OK;
	/*
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	if ( pframeCur->pmsMeshs != NULL )
	{
		pmsMeshs = pframeCur->pmsMeshs;

		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				AddKosmoVertex ( pd3dDevice );
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

	*/
	return hr;
}

HRESULT DxEffectKosmo::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICE8 pd3dDevice )
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

	//CreateMeshKosmo ( pd3dDevice );

	return S_OK;
}


HRESULT DxEffectKosmo::InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
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

HRESULT DxEffectKosmo::DeleteDeviceObjects ()
{
	//	Note : 이전 텍스쳐가 로드되어 있을 경우 제거.
	//
	if ( m_pddsTexture )
		TextureManager::ReleaseTexture ( m_pddsTexture );
	m_pddsTexture = NULL;

	return S_OK;
}

HRESULT DxEffectKosmo::FrameMove ( float fTime, float fElapsedTime )
{
	m_fTime = fTime;
	m_fElapsedTime += fElapsedTime;
	return S_OK;
}

HRESULT DxEffectKosmo::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr = S_OK;

	if ( m_pVB )
	{
		DWORD dwVerts, dwFaces;

		dwVerts = (m_dwFace)*4;
		dwFaces = (m_dwFace)*2;


		D3DXVECTOR4 const8Nums( 0.0f, 10.f, 0.0f, 0.0f );
		pd3dDevice->SetVertexShaderConstant( VSC_CUSTOM+1, &const8Nums, 1);

		D3DXVECTOR4 constNums( 0.5f, 0.25f, 0.125f, 0.0625f );
		pd3dDevice->SetVertexShaderConstant( VSC_CUSTOM+2, &constNums, 1);


		//	Note : SetVertexShader에 World와 WorldViewProjection 행렬 집어 넣기
		//
		D3DXMATRIX matWorld, matView, matProj, matWVP;

		matView = DxViewPort::Instance.GetMatView();
		matProj = DxViewPort::Instance.GetMatProj();

		D3DXMatrixTranspose( &matWorld, &(pframeCur->matCombined) );
		pd3dDevice->SetVertexShaderConstant ( VSC_MATWORLD,			&matWorld,	4 );

		D3DXMatrixMultiply ( &matWVP, &(pframeCur->matCombined), &matView );
		D3DXMatrixMultiply ( &matWVP, &matWVP, &matProj );
		D3DXMatrixTranspose( &matWVP, &matWVP );
		pd3dDevice->SetVertexShaderConstant ( VSC_MATWORLDVIEWPROJ, &matWVP,	4 );




/*
		//	Note : 움직이게 만들자.
		//
		KOSMOFACE*	pVertices;
		m_pVB->Lock ( 0, 0, (BYTE**)&pVertices, 0 );
		AddVertexBuffer ( m_pBaseVertex, pVertices, m_dwVertex );
		m_pVB->Unlock ();
		*/


		//	Note : 선을 움직이자.
		//
		if ( m_iTemp == 0 )
		{
			KOSMOFACE*	pVertices;
			m_pVB->Lock ( 0, 0, (BYTE**)&pVertices, 0 );
			LineMove ( pVertices, m_pBaseVertex, m_dwFace );
			m_pVB->Unlock ();
		}


		
		//	Note : 타원을 움직이자.
		//
		else if ( m_iTemp == 1 )
		{
			D3DXMatrixRotationY ( &matWorld, m_fTime * 2.f );

			D3DXMatrixMultiply ( &matWorld, &matWorld, &pframeCur->matCombined );

			D3DXMatrixMultiply ( &matWVP, &matWorld, &matView );
			D3DXMatrixMultiply ( &matWVP, &matWVP, &matProj );
			D3DXMatrixTranspose( &matWVP, &matWVP );
			pd3dDevice->SetVertexShaderConstant ( VSC_MATWORLDVIEWPROJ, &matWVP,	4 );

			D3DXMatrixTranspose( &matWorld, &matWorld );
			pd3dDevice->SetVertexShaderConstant ( VSC_MATWORLD,			&matWorld,	4 );

			KOSMOFACE*	pVertices;
			m_pVB->Lock ( 0, 0, (BYTE**)&pVertices, 0 );
			CircleMove ( pVertices, m_pBaseVertex, m_dwFace );
			m_pVB->Unlock ();
		}

		
		//	Note : 회오리를 움직이자.
		//
		else if ( m_iTemp == 2 )
		{
			D3DXMatrixRotationY ( &matWorld, -(m_fTime * 20.f) );

			D3DXMatrixMultiply ( &matWorld, &matWorld, &pframeCur->matCombined );

			D3DXMatrixMultiply ( &matWVP, &matWorld, &matView );
			D3DXMatrixMultiply ( &matWVP, &matWVP, &matProj );
			D3DXMatrixTranspose( &matWVP, &matWVP );
			pd3dDevice->SetVertexShaderConstant ( VSC_MATWORLDVIEWPROJ, &matWVP,	4 );

			D3DXMatrixTranspose( &matWorld, &matWorld );
			pd3dDevice->SetVertexShaderConstant ( VSC_MATWORLD,			&matWorld,	4 );
		}







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


		pd3dDevice->SetTexture ( 0, m_pddsTexture );

		//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
		//
		pd3dDevice->SetIndices ( m_pIB, 0 );

		pd3dDevice->SetStreamSource ( 0, m_pVB, sizeof(KOSMOFACE) );
	
		pd3dDevice->SetVertexShader ( m_dwEffect );


		pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, dwVerts, 0, dwFaces );

		//	Note : 소프트웨어 버텍스 프로세싱
		//
		if ( DxEffectMan::Instance.GetUseSwShader() )
			pd3dDevice->SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, FALSE );

		//	Note : 이전상태로 복원.
		//
		pd3dDevice->ApplyStateBlock ( m_dwSavedStateBlock );

		pd3dDevice->SetIndices ( NULL, 0 );
		pd3dDevice->SetStreamSource ( 0, NULL, 0 );

		pd3dDevice->SetTexture ( 0, NULL );
	}

	return S_OK;
}

D3DXVECTOR3 DxEffectKosmo::GetThisVector ( DWORD Data )
{
	D3DXVECTOR3		vVector( 0.f, 0.f, 0.f );
	if ( m_pBaseVertex )
		vVector = m_pBaseVertex[Data].Pos;

	return vVector;
}

float DxEffectKosmo::GetThisScale ( DWORD Data )
{
	float fScale = 0.0f;
	if ( m_pBaseVertex )
		fScale = m_pBaseVertex[Data].fSize;

	return fScale;
}

HRESULT	DxEffectKosmo::GetThisVector ( DWORD Data, D3DXVECTOR3 vVector )
{
	m_pBaseVertex[Data].Pos = vVector;

	return S_OK;
}

HRESULT	DxEffectKosmo::GetThisScale ( DWORD Data, float fScale )
{
	m_pBaseVertex[Data].fSize = fScale;

	return S_OK;
}

HRESULT DxEffectKosmo::AddKosmoVertex ( LPDIRECT3DDEVICE8 pd3dDevice, D3DXVECTOR3 Pos, float fSize )
{
	m_dwVertex++;	// 점이 1씩 더해 진다.

	VERTEX*		m_pTempVertex;
	m_pTempVertex = new VERTEX[m_dwVertex];

	if ( m_pBaseVertex )
	{
		memcpy ( m_pTempVertex, m_pBaseVertex, sizeof(VERTEX)*(m_dwVertex-1) );
		SAFE_DELETE_ARRAY ( m_pBaseVertex );
	}

	//	Note : 새로운 점 추가 작업
	//
	m_pTempVertex[m_dwVertex-1].Pos = Pos;
	m_pTempVertex[m_dwVertex-1].fSize = fSize;



	m_pBaseVertex = new VERTEX[m_dwVertex];

	memcpy ( m_pBaseVertex, m_pTempVertex, sizeof(VERTEX)*m_dwVertex );
	SAFE_DELETE_ARRAY ( m_pTempVertex );

	return AddKosmoMesh ( pd3dDevice, m_dwFace );
}




HRESULT DxEffectKosmo::MakeMagLine ( LPDIRECT3DDEVICE8 pd3dDevice, D3DXVECTOR3 v1, D3DXVECTOR3 v2 )
{
	SAFE_DELETE_ARRAY ( m_pBaseVertex );

	D3DXVECTOR3 Dis = (v2 - v1)/(float)(m_dwFace-1);

	m_pBaseVertex = new VERTEX[m_dwFace];

	for ( DWORD i=0; i<m_dwFace; i++ )
	{
		m_pBaseVertex[i].Pos = v1 + (Dis*(float)i);
		m_pBaseVertex[i].Diff = (float)(m_dwFace-i)/(float)m_dwFace;
		m_pBaseVertex[i].fSize = 1.0f;
	}

	return AddKosmoMesh ( pd3dDevice, m_dwFace );
}

HRESULT DxEffectKosmo::LineMove ( KOSMOFACE* pVertices, VERTEX* pBaseVertices, DWORD Faces )
{
	static float g = 0.f;
	float g2 = 0.f;

	float f = 20.f*m_fTime;
	float h = 10.f*m_fTime;

	float f2 = 20.f*m_fTime;
	float h2 = 10.f*m_fTime;

	float na = 0.1f;
	float na2 = 0.1f;

	float hh = 0.f;
	float hh2 = 0.f;

	static DWORD	NowUp = 0;

	for ( DWORD i=0; i<Faces; i++ )
	{
		if ( NowUp > Faces - 2 )
			NowUp = 0;

		if ( i == 0 )
			na = 0.0f;

		if ( i == NowUp )
			hh = 1.0f;
		

		if ( i == 0 )
			PushLine ( pVertices, 
						D3DXVECTOR3 ( pBaseVertices[i].Pos.x - sinf(f)*na, pBaseVertices[i].Pos.y, pBaseVertices[i].Pos.z - sinf(h)*na ),
						D3DXVECTOR3 ( pBaseVertices[i].Pos.x - sinf(f)*na, pBaseVertices[i].Pos.y, pBaseVertices[i].Pos.z - sinf(h)*na ),
						pBaseVertices[i].fSize, pBaseVertices[i].fSize );
		else if ( i == 1 )
			PushLine ( pVertices, 
						D3DXVECTOR3 ( pBaseVertices[i].Pos.x - sinf(f)*na, pBaseVertices[i].Pos.y, pBaseVertices[i].Pos.z - sinf(h)*na ),
						D3DXVECTOR3 ( pBaseVertices[i].Pos.x - sinf(f)*na, pBaseVertices[i].Pos.y + sinf(g), pBaseVertices[i].Pos.z - sinf(h)*na ),
						pBaseVertices[i].fSize, pBaseVertices[i].fSize );
		else if ( i == Faces - 1 )
			PushLine ( pVertices, 
						D3DXVECTOR3 ( pBaseVertices[i-1].Pos.x - sinf(f2)*na2, pBaseVertices[i-1].Pos.y, pBaseVertices[i-1].Pos.z - sinf(h2)*na2 ),
						D3DXVECTOR3 ( pBaseVertices[i].Pos.x - sinf(f)*na, pBaseVertices[i].Pos.y, pBaseVertices[i].Pos.z - sinf(h)*na ),
						pBaseVertices[i-1].fSize, pBaseVertices[i].fSize );
		else
			PushLine ( pVertices, 
						D3DXVECTOR3 ( pBaseVertices[i-1].Pos.x - sinf(f2)*na2, pBaseVertices[i-1].Pos.y + sinf(g2), pBaseVertices[i-1].Pos.z - sinf(h2)*na2 ),
						D3DXVECTOR3 ( pBaseVertices[i].Pos.x - sinf(f)*na, pBaseVertices[i].Pos.y + sinf(g), pBaseVertices[i].Pos.z - sinf(h)*na ),
						pBaseVertices[i-1].fSize, pBaseVertices[i].fSize );

		f2 = f;
		h2 = h;

		f += 1.0f;
		h += 1.0f;

		na2 = na;
		na = 0.1f;

		hh2 = hh;
		hh = 0.0f;

		g2 = g;
	}

	if ( m_fElapsedTime > 0.05f )
	{
		m_fElapsedTime = 0.0f;
		g += 0.1f;

		if ( g > 1.57f )
		{
			g = g - 1.57f;
			NowUp++;
		}
	}

	return S_OK;
}





HRESULT DxEffectKosmo::MakeMagCircle ( LPDIRECT3DDEVICE8 pd3dDevice, D3DXVECTOR3 Pos )
{
	SAFE_DELETE_ARRAY ( m_pBaseVertex );

	float Dis = D3DX_PI*2.f/(float)(m_dwFace-1);

	m_pBaseVertex = new VERTEX[m_dwFace];

	float Divide = m_fWidth / m_fHeight;

	for ( DWORD i=0; i<m_dwFace; i++ )
	{
		D3DXVECTOR3	TempPos = Pos;
		m_pBaseVertex[i].Pos = D3DXVECTOR3 ( TempPos.x + cosf(Dis*i)*m_fHeight*Divide, TempPos.y + sinf(Dis*i)*m_fHeight + m_fHeight, TempPos.z );
		m_pBaseVertex[i].Diff = (float)(m_dwFace-i)/(float)m_dwFace;
		m_pBaseVertex[i].fSize = 1.0f;
	}
	m_pBaseVertex[0].Pos = m_pBaseVertex[1].Pos;;
	m_pBaseVertex[0].Diff = m_pBaseVertex[1].Diff;
	m_pBaseVertex[0].fSize = m_pBaseVertex[1].fSize;


	return AddKosmoMesh ( pd3dDevice, m_dwFace );
}

HRESULT DxEffectKosmo::CircleMove ( KOSMOFACE* pVertices, VERTEX* pBaseVertices, DWORD Faces )
{
	static float kk =0.f;
	if ( m_fElapsedTime > 0.05f )
	{
		m_fElapsedTime = 0.f;

		kk -= 0.3f;

		float Dis = D3DX_PI/(float)m_dwFace;
		float Divide = m_fWidth / m_fHeight;

		for ( DWORD i=0; i<Faces; i++ )
		{
			D3DXVECTOR3	TempPos = LineStart;
			m_pBaseVertex[i].Pos = D3DXVECTOR3 ( TempPos.x + cosf(Dis*i+kk)*m_fHeight*Divide, TempPos.y + sinf(Dis*i+kk)*m_fHeight + m_fHeight, TempPos.z );
		}
		m_pBaseVertex[0].Pos = m_pBaseVertex[1].Pos;

		for ( i=0; i<Faces; i++)
		{
			if ( i == 0 )
				LineDuffuse ( pVertices, pBaseVertices[i].Pos, pBaseVertices[i].Pos );
			else
				LineDuffuse ( pVertices, pBaseVertices[i-1].Pos, pBaseVertices[i].Pos );
		}

	}

	return S_OK;
}

void DxEffectKosmo::LineDuffuse	( KOSMOFACE* &pBuff, D3DXVECTOR3 v1, D3DXVECTOR3 v2 )
{
	pBuff->Pos1 = v1;
	pBuff->Pos2 = v2;
	++pBuff;
	pBuff->Pos1 = v2;
	pBuff->Pos2 = v1;
	++pBuff;
	pBuff->Pos1 = v2;
	pBuff->Pos2 = v1;
	++pBuff;
	pBuff->Pos1 = v1;
	pBuff->Pos2 = v2;
	++pBuff;
}





HRESULT DxEffectKosmo::MakeMagCyclone ( LPDIRECT3DDEVICE8 pd3dDevice, D3DXVECTOR3 v1, D3DXVECTOR3 v2 )
{
	SAFE_DELETE_ARRAY ( m_pBaseVertex );

	D3DXVECTOR3 Dis = (v2 - v1)/(float)(m_dwFace-1);

	m_pBaseVertex = new VERTEX[m_dwFace];

	float fX = 0.f;
	float fZ = 0.f;

	float fMul = 0.2f;

	for ( DWORD i=0; i<m_dwFace; i++ )
	{
		D3DXVECTOR3	TempPos = v1 + (Dis*(float)i);
		m_pBaseVertex[i].Pos = D3DXVECTOR3 ( TempPos.x + sinf(fX)*fMul, TempPos.y, TempPos.z + sinf(fZ)*fMul );
		m_pBaseVertex[i].Diff = (float)(m_dwFace-i)/(float)m_dwFace;
		m_pBaseVertex[i].fSize = 0.6f;

		fX += 0.4f;
		fZ += 0.5f;
	}

	return AddKosmoMesh ( pd3dDevice, m_dwFace );
}





HRESULT DxEffectKosmo::MakeMagLighting ( LPDIRECT3DDEVICE8 pd3dDevice, D3DXVECTOR3 v1, D3DXVECTOR3 v2 )
{
	SAFE_DELETE_ARRAY ( m_pBaseVertex );

	D3DXVECTOR3 Dis = (v2 - v1)/(float)(m_dwFace-1);

	m_pBaseVertex = new VERTEX[m_dwFace];

	float fMul = 0.2f;

	for ( DWORD i=0; i<m_dwFace; i++ )
	{
		D3DXVECTOR3	TempPos = v1 + (Dis*(float)i);
		m_pBaseVertex[i].Pos = D3DXVECTOR3 ( TempPos.x, TempPos.y, TempPos.z );
		m_pBaseVertex[i].Diff = 1.f;	//(float)(m_dwFace-i)/(float)m_dwFace;
		m_pBaseVertex[i].fSize = 0.6f;
	}

	return AddMeshStrip ( pd3dDevice, m_dwFace );
}





HRESULT DxEffectKosmo::AddMeshStrip ( LPDIRECT3DDEVICE8 pd3dDevice, DWORD Faces )
{
	HRESULT hr;

	if ( m_pBaseVertex )
	{
		//	Note : 버텍스 버퍼를 만들자.
		//
		SAFE_RELEASE ( m_pVB );
		hr = pd3dDevice->CreateVertexBuffer( Faces *2* sizeof(KOSMOSTRIP),
											D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pVB );
		if (FAILED(hr))		return hr;

		KOSMOSTRIP*	pVertices;
		m_pVB->Lock ( 0, 0, (BYTE**)&pVertices, 0 );

		for ( DWORD i=0; i<Faces; i++ )
			PushStrip ( pVertices, m_pBaseVertex[i].Pos, m_pBaseVertex[i].fSize, m_pBaseVertex[i].Diff );

		m_pVB->Unlock ();



		//	Note : 인덱스 버퍼를 없애 버리자 ㅋㅋ
		//
		SAFE_RELEASE ( m_pIB );
	}

	return S_OK;
}

void DxEffectKosmo::PushStrip ( KOSMOSTRIP* &pBuff, D3DXVECTOR3 Pos, float Size, float Color )
{
	/*
	D3DXVECTOR3 v=inFromPos-inToPos;
	float invlen=1.0f/(sqrtf(v.x*v.x+v.y*v.y+v.z*v.z)+inFromSize+inToSize);

	pBuff->Pos1 = inFromPos;
	pBuff->Pos2 = inToPos;
	pBuff->Temp1 = D3DXVECTOR4(inFromSize,-inFromSize,0,0);
    pBuff->Temp2 = D3DXVECTOR3(inFromSize,inToSize,invlen);
	pBuff->Diff = Color1;
	++pBuff;
	pBuff->Pos1 = inFromPos;
	pBuff->Pos2 = inToPos;
	pBuff->Temp1 = D3DXVECTOR4(inFromSize,inFromSize,0,0.25f);
    pBuff->Temp2 = D3DXVECTOR3(inFromSize,inToSize,invlen);
	pBuff->Diff = Color1;
	++pBuff;
	*/
}



HRESULT DxEffectKosmo::AddKosmoMesh ( LPDIRECT3DDEVICE8 pd3dDevice, DWORD Faces )
{
	HRESULT hr;

	if ( m_pBaseVertex )
	{
		//	Note : 버텍스 버퍼를 만들자.
		//
		SAFE_RELEASE ( m_pVB );
		hr = pd3dDevice->CreateVertexBuffer( Faces *4* sizeof(KOSMOFACE),
											D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pVB );
		if (FAILED(hr))		return hr;

		KOSMOFACE*	pVertices;
		m_pVB->Lock ( 0, 0, (BYTE**)&pVertices, 0 );

		AddVertexBuffer ( pVertices, m_pBaseVertex, Faces );
		m_pVB->Unlock ();



		//	Note : 인덱스 버퍼를 만들자.
		//
		SAFE_RELEASE ( m_pIB );

		//	Note :  1개의 파티클에서 2개의 삼각 형을 만든다.
		//
		hr = pd3dDevice->CreateIndexBuffer( Faces *6* sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB );
		if (FAILED(hr))		return S_OK;

		WORD *pIndices;
		hr = m_pIB->Lock(0, 0,(BYTE**)&pIndices, 0);
		if (FAILED(hr))		return S_OK;

		for(DWORD i=0;i<Faces;i++)
		{
			*pIndices++ = (WORD)(i*4+0);
			*pIndices++ = (WORD)(i*4+1);
			*pIndices++ = (WORD)(i*4+2);

   			*pIndices++ = (WORD)(i*4+2);
			*pIndices++ = (WORD)(i*4+3);
			*pIndices++ = (WORD)(i*4+0);
		}
		m_pIB->Unlock();

	}

	return S_OK;
}








HRESULT DxEffectKosmo::AddVertexBuffer ( KOSMOFACE* pVertices, VERTEX* pBaseVertices, DWORD Faces )
{
	for ( DWORD i=0; i<Faces; i++ )
	{
		if ( i == 0 )
			PushLine2 ( pVertices, pBaseVertices[i].Pos, pBaseVertices[i].Pos, pBaseVertices[i].fSize, pBaseVertices[i].fSize, 
						pBaseVertices[i].Diff, pBaseVertices[i].Diff );
		else
			PushLine2 ( pVertices, pBaseVertices[i-1].Pos, pBaseVertices[i].Pos, pBaseVertices[i-1].fSize, pBaseVertices[i].fSize, 
						pBaseVertices[i-1].Diff, pBaseVertices[i].Diff );
	}

	/*
	DWORD i=0;
	D3DXVECTOR3 pos0(0,0,0);	
	float f=4.f*m_fTime;
	float g=3.f*m_fTime;
	float size0=-1.0f,size1;

	for(;i<m_dwVertex;f+=0.1f,g+=0.06f)
	{
		D3DXVECTOR3 pos1;

		pos1=1.0f*D3DXVECTOR3(sinf(f)*cosf(g*0.5f)*4.0f,
						    sinf(f*1.27f)*cosf(g)*4.0f,sinf(f*1.8f)*cosf(g*2.7f)*3.7f);

		size1=1.4f*(1.f+sinf(f*1.5f+0.3f)*sinf(g*2.7f));

		if(size0!=-1.0)
		{
			PushLine(pVertices,pos0,pos1,size0,size1);

			i++;
		}

		pos0=pos1;size0=size1;
	}
	*/

	return S_OK;
}

void DxEffectKosmo::PushLine( KOSMOFACE* &pBuff, D3DXVECTOR3 inFromPos, D3DXVECTOR3 inToPos, float inFromSize, float inToSize )
{
	D3DXVECTOR3 v=inFromPos-inToPos;
	float invlen=1.0f/(sqrtf(v.x*v.x+v.y*v.y+v.z*v.z)+inFromSize+inToSize);

	pBuff->Pos1 = inFromPos;
	pBuff->Pos2 = inToPos;
	pBuff->Temp1 = D3DXVECTOR4(inFromSize,-inFromSize,0,0);
    pBuff->Temp2 = D3DXVECTOR3(inFromSize,inToSize,invlen);
	++pBuff;
	pBuff->Pos1 = inToPos;
	pBuff->Pos2 = inFromPos;
	pBuff->Temp1 = D3DXVECTOR4(inToSize,inToSize,0.25f,0);
    pBuff->Temp2 = D3DXVECTOR3(inToSize,inFromSize,invlen);
	++pBuff;
	pBuff->Pos1 = inToPos;
	pBuff->Pos2 = inFromPos;
	pBuff->Temp1 = D3DXVECTOR4(inToSize,-inToSize,0.25f,0.25f);
    pBuff->Temp2 = D3DXVECTOR3(inToSize,inFromSize,invlen);
	++pBuff;
	pBuff->Pos1 = inFromPos;
	pBuff->Pos2 = inToPos;
	pBuff->Temp1 = D3DXVECTOR4(inFromSize,inFromSize,0,0.25f);
    pBuff->Temp2 = D3DXVECTOR3(inFromSize,inToSize,invlen);
	++pBuff;
}

void DxEffectKosmo::PushLine2 ( KOSMOFACE* &pBuff, D3DXVECTOR3 inFromPos, D3DXVECTOR3 inToPos, float inFromSize, float inToSize, float Color1, float Color2 )
{
	D3DXVECTOR3 v=inFromPos-inToPos;
	float invlen=1.0f/(sqrtf(v.x*v.x+v.y*v.y+v.z*v.z)+inFromSize+inToSize);

	pBuff->Pos1 = inFromPos;
	pBuff->Pos2 = inToPos;
	pBuff->Temp1 = D3DXVECTOR4(inFromSize,-inFromSize,0,0);
    pBuff->Temp2 = D3DXVECTOR3(inFromSize,inToSize,invlen);
	pBuff->Diff = Color1;
	++pBuff;
	pBuff->Pos1 = inToPos;
	pBuff->Pos2 = inFromPos;
	pBuff->Temp1 = D3DXVECTOR4(inToSize,inToSize,0.25f,0);
    pBuff->Temp2 = D3DXVECTOR3(inToSize,inFromSize,invlen);
	pBuff->Diff = Color2;
	++pBuff;
	pBuff->Pos1 = inToPos;
	pBuff->Pos2 = inFromPos;
	pBuff->Temp1 = D3DXVECTOR4(inToSize,-inToSize,0.25f,0.25f);
    pBuff->Temp2 = D3DXVECTOR3(inToSize,inFromSize,invlen);
	pBuff->Diff = Color2;
	++pBuff;
	pBuff->Pos1 = inFromPos;
	pBuff->Pos2 = inToPos;
	pBuff->Temp1 = D3DXVECTOR4(inFromSize,inFromSize,0,0.25f);
    pBuff->Temp2 = D3DXVECTOR3(inFromSize,inToSize,invlen);
	pBuff->Diff = Color1;
	++pBuff;
}


HRESULT DxEffectKosmo::CreateMeshKosmo ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr;
	//	Note :  1개의 파티클에서 4개의 점을 생성 한다.
	//
	SAFE_RELEASE ( m_pVB );
    hr = pd3dDevice->CreateVertexBuffer( m_dwVertex *4* sizeof(KOSMOFACE),
										D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pVB );
	if (FAILED(hr))
		return hr;

	//	Note : 중요 셋팅
	//
	FillVertexBuffer();


	//	Note :  1개의 파티클에서 2개의 삼각 형을 만든다.
	//
	SAFE_RELEASE ( m_pIB );
	hr = pd3dDevice->CreateIndexBuffer( m_dwVertex *6* sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB );
	if (FAILED(hr))		return S_OK;

	WORD *pIndices;
	hr = m_pIB->Lock(0, m_dwVertex *6* sizeof(WORD),(BYTE**)&pIndices, 0);
	if (FAILED(hr))		return S_OK;

    for(DWORD i=0;i<m_dwVertex;i++)
    {
		*pIndices++ = (WORD)(i*4+0);
		*pIndices++ = (WORD)(i*4+1);
		*pIndices++ = (WORD)(i*4+2);

   		*pIndices++ = (WORD)(i*4+2);
		*pIndices++ = (WORD)(i*4+3);
		*pIndices++ = (WORD)(i*4+0);
	}
	m_pIB->Unlock();


	return S_OK;
}

HRESULT DxEffectKosmo::FillVertexBuffer( void )
{
	HRESULT hr;

	





	KOSMOFACE* pBuff;

    assert(m_pVB != NULL);
	hr = m_pVB->Lock(0, m_dwVertex *4* sizeof(KOSMOFACE),(BYTE**)&pBuff, 0);
	if (FAILED(hr))		return E_FAIL;

    float const     kRandMax  = static_cast<float>(RAND_MAX);

	DWORD i=0;

	

	// generate particle values
	switch(m_iScene)
	{
		case 0:
			{
				float z=0.0f;
				float k=6.0f;
				while(m_dwVertex-i>=12)
				{
					PushLine(pBuff,D3DXVECTOR3(1,1,z),D3DXVECTOR3(-1,1,z),0.9f,0.9f);
					PushLine(pBuff,D3DXVECTOR3(-1,1,z),D3DXVECTOR3(-1,-1,z),0.9f,0.9f);
					PushLine(pBuff,D3DXVECTOR3(-1,-1,z),D3DXVECTOR3(1,-1,z),0.9f,0.9f);
					PushLine(pBuff,D3DXVECTOR3(1,-1,z),D3DXVECTOR3(1,1,z),0.9f,0.9f);

					PushLine(pBuff,D3DXVECTOR3(k+1*0.3f,1*0.3f,z),D3DXVECTOR3(k-1*0.3f,1*0.3f,z),0.9f*0.3f,0.9f*0.3f);
					PushLine(pBuff,D3DXVECTOR3(k-1*0.3f,1*0.3f,z),D3DXVECTOR3(k-1*0.3f,-1*0.3f,z),0.9f*0.3f,0.9f*0.3f);
					PushLine(pBuff,D3DXVECTOR3(k-1*0.3f,-1*0.3f,z),D3DXVECTOR3(k+1*0.3f,-1*0.3f,z),0.9f*0.3f,0.9f*0.3f);
					PushLine(pBuff,D3DXVECTOR3(k+1*0.3f,-1*0.3f,z),D3DXVECTOR3(k+1*0.3f,1*0.3f,z),0.9f*0.3f,0.9f*0.3f);

					PushLine(pBuff,D3DXVECTOR3(-k+1,1,z),D3DXVECTOR3(-k-1,1,z),0.9f,0.9f);
					PushLine(pBuff,D3DXVECTOR3(-k-1,1,z),D3DXVECTOR3(-k-1,-1,z),0.9f,0.9f);
					PushLine(pBuff,D3DXVECTOR3(-k-1,-1,z),D3DXVECTOR3(-k+1,-1,z),0.9f,0.9f);
					PushLine(pBuff,D3DXVECTOR3(-k+1,-1,z),D3DXVECTOR3(-k+1,1,z),0.9f,0.9f);

					i+=12;z+=2.0f;
				}
			}
			break;

		case 1:
			{
				for(;i<m_dwVertex;i++)
				{
					D3DXVECTOR3 pos0,pos1;

					pos0=D3DXVECTOR3(static_cast<float>(rand())/kRandMax,static_cast<float>(rand())/kRandMax,static_cast<float>(rand())/kRandMax);
					pos1=D3DXVECTOR3(static_cast<float>(rand())/kRandMax,static_cast<float>(rand())/kRandMax,static_cast<float>(rand())/kRandMax);

					pos0=pos0*8.0f-D3DXVECTOR3(4.0f,4.0f,4.0f);

					pos1=pos0+pos1*0.5f-D3DXVECTOR3(0.25f,0.25f,0.25f);

					PushLine(pBuff,pos0,pos1,0.9f,0.9f);
				}
			}
			break;

		case 2:
			{
				D3DXVECTOR3 pos0(0,0,0);	
				float f=0.0f;

				for(;i<m_dwVertex;f+=0.2f)
				{
					D3DXVECTOR3 pos1;

					pos1=D3DXVECTOR3(sinf(f)*4.0f,cosf(f)*4.0f,f*0.7f);

					if(f!=0.0f)
					{
						PushLine(pBuff,pos0,pos1,0.9f,0.9f);	// z,w 크면 두껍게 작으면 얇게 나온다.

						i++;
					}

					pos0=pos1;
				}
			}
			break;

		case 3:
			{
				D3DXVECTOR3 pos0(0,0,0);	
				float f=0.0f;

				for(;i<m_dwVertex;f+=0.1f)
				{
					D3DXVECTOR3 pos1;

					pos1=2.0f*D3DXVECTOR3(sinf(f)*cosf(f*0.5f)*4.0f,
						             sinf(f*1.7f)*cosf(f)*4.0f,sinf(f*1.8f)*cosf(f*0.7f)*3.7f);

					if(f!=0.0f)
					{
						PushLine(pBuff,pos0,pos1,0.9f,0.9f);

						i++;
					}

					pos0=pos1;
				}
			}
			break;
		case 4:
			{
				D3DXVECTOR3 pos0(0,0,0);	
				float f=0.0f;
				float size0=0.0f,size1;

				for(;i<m_dwVertex;f+=0.1f)
				{
					D3DXVECTOR3 pos1;

					pos1=2.0f*D3DXVECTOR3(sinf(f)*cosf(f*0.5f)*4.0f,
						             sinf(f*1.7f)*cosf(f)*4.0f,sinf(f*1.8f)*cosf(f*0.7f)*3.7f);

					size1=1.4f*(1.f+sinf(f*1.5f+0.3f)*sinf(f*2.7f));

					if(f!=0.0f)
					{
						PushLine(pBuff,pos0,pos1,size0,size1);

						i++;
					}

					pos0=pos1;size0=size1;
				}
			}
			break;
		case 5:
			{
				D3DXVECTOR3 pos0(0,0,0);	
				float f=0.004f*timeGetTime();
				float g=0.003f*timeGetTime();
				float size0=-1.0f,size1;

				for(;i<m_dwVertex;f+=0.1f,g+=0.06f)
				{
					D3DXVECTOR3 pos1;

					pos1=2.0f*D3DXVECTOR3(sinf(f)*cosf(g*0.5f)*4.0f,
						             sinf(f*1.27f)*cosf(g)*4.0f,sinf(f*1.8f)*cosf(g*2.7f)*3.7f);

					size1=1.4f*(1.f+sinf(f*1.5f+0.3f)*sinf(g*2.7f));

					if(size0!=-1.0)
					{
						PushLine(pBuff,pos0,pos1,size0,size1);

						i++;
					}

					pos0=pos1;size0=size1;
				}
			}
			break;
		case 6:
			{
				D3DXVECTOR3 pos0(0,0,0);	
				float f=0.0f;
				float g=0.002f*timeGetTime();
				float size0=0.0f,size1;

				for(;i<m_dwVertex;f+=0.1f,g+=0.06f)
				{
					D3DXVECTOR3 pos1;

					pos1=2.0f*D3DXVECTOR3(sinf(f)*cosf(f*1.5f)*4.0f,
						             sinf(f*2.7f)*cosf(f)*4.0f,sinf(f*1.4f)*cosf(f*0.7f)*3.7f);

					size1=1.4f*(1.f+sinf(g*1.5f+0.3f)*sinf(g*2.7f));

					if(f!=0.0f)
					{
						PushLine(pBuff,pos0,pos1,size0,size1);

						i++;
					}

					pos0=pos1;size0=size1;
				}
			}
			break;
		case 7:
			{
				D3DXVECTOR3 pos0(0,0,0);	
				float f=0.0f;
				float g=0.002f*timeGetTime();
				float size0=0.0f,size1;

				for(;i<m_dwVertex;f+=0.1f,g+=0.06f)
				{
					D3DXVECTOR3 pos1;

					pos1=2.0f*D3DXVECTOR3(sinf(f)*cosf(f*0.5f)*4.0f,
						             sinf(f*1.7f)*cosf(f)*4.0f,sinf(f*1.8f)*cosf(f*0.7f)*3.7f);

					size1=1.4f*(1.f+sinf(g*1.5f+0.3f)*sinf(g*2.7f));

					if(f!=0.0f)
					{
						PushLine(pBuff,pos0,pos1,size0,size1);

						i++;
					}

//					pos0=pos1;
					size0=size1;
				}
			}
			break;
		case 8:
			{
				D3DXVECTOR3 pos0(0,0,0);	
				float gg=0.0005f*timeGetTime();
				float size1;

				for(;i<m_dwVertex;gg+=0.08f)
				{
					float size0=-1.0f;

					float g=(float)sin(i)*10.0f+sinf(i*2.4f)*40.0f+gg;

					for(float h=0;h<1.0f;h+=0.1f)
					{
						D3DXVECTOR3 pos1;

						pos1=(h+0.2f)*1.0f*D3DXVECTOR3(
										sinf(g)*cosf(h*0.5f)*4.0f+cosf(gg+h*8.0f)*0.5f,
										sinf(g*1.7f)*cosf(h)*4.0f+sinf(gg+h*10.0f)*0.5f,
										sinf(h*1.8f)*cosf(g*0.7f)*3.7f);

						size1=0.3f*(1.f+sinf(g*1.5f+0.3f)*sinf(g*2.7f));

						if(size0!=-1.0f)
						{
							PushLine(pBuff,pos0,pos1,size0,size1);

							i++;if(i>=m_dwVertex)break;
						}

						pos0=pos1;
						size0=size1;
					}
				}
			}
			break;
	}

	m_pVB->Unlock();

	return(S_OK);
}
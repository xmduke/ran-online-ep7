#include "stdafx.h"
#include "DxEffSingle.h"
#include "DxEffSinglePropGMan.h"
#include "SerialFile.h"

#include "DxViewPort.h"

#include "DxEffCharElectricity.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const DWORD DxEffCharElectricity::D3DVERTEX::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
DWORD		DxEffCharElectricity::TYPEID			= EMEFFCHAR_ELECTRICITY;
DWORD		DxEffCharElectricity::VERSION			= 0x0100;
char		DxEffCharElectricity::NAME[MAX_PATH]	= "2.전기";

DWORD		DxEffCharElectricity::m_dwSavedStateBlock		= NULL;
DWORD		DxEffCharElectricity::m_dwEffectStateBlock	= NULL;

DxEffCharElectricity::DxEffCharElectricity(void) :
	DxEffChar(),
	m_pd3dDevice(NULL),
	m_vTransUP(0.f,0.f,0.f),
	m_vTransDOWN(0.f,0.f,0.f),
	m_vPrevUP(0.f,0.f,0.f),
	m_vPrevDOWN(0.f,0.f,0.f),
	m_pVertex(NULL),
	m_pVertex_Out(NULL),
	m_pRootVertex(NULL),

//	m_pTargetPos(NULL),
//	m_nDivide(5),
//	m_nRoop(0),

	m_nVertexNum(0),
	m_dwOriginNum(0),
	m_fLifeTime(3.f),
	m_fAlpha(0.f),
	m_fAlphaDelta1(0.f),
	m_fAlphaDelta2(0.f),
	m_fAlphaDelta3(0.f),
	m_fAlphaTime1(0.f),
	m_fAlphaTime2(0.f),
	m_cColor(0.f,0.f,0.f,0.f),
	m_cColorDelta(0.f,0.f,0.f,0.f),
	m_fElapsedTime(0.f),
	m_pTexture_In(NULL),
	m_pTexture_Out(NULL)
{
	m_szTraceUP[0] = NULL;
	m_szTraceDOWN[1] = NULL;
}


DxEffCharElectricity::~DxEffCharElectricity(void)
{
	CleanUp ();

	SAFE_DELETE_ARRAY ( m_pRootVertex );
	SAFE_DELETE_ARRAY ( m_pVertex );
	SAFE_DELETE_ARRAY ( m_pVertex_Out );
//	SAFE_DELETE_ARRAY ( m_pTargetPos );
}

DxEffChar* DxEffCharElectricity::CloneInstance ( LPDIRECT3DDEVICE8 pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	HRESULT hr;
	DxEffCharElectricity *pEffChar = new DxEffCharElectricity;
	pEffChar->SetLinkObj ( pCharPart, pSkinPiece );
	pEffChar->SetProperty ( &m_Property );

	hr = pEffChar->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

	return pEffChar;
}

HRESULT DxEffCharElectricity::CreateDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
{	
	D3DCAPS8	d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		//
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,			D3DCULL_NONE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );

		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_dwSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_dwEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffCharElectricity::ReleaseDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
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

HRESULT DxEffCharElectricity::OneTimeSceneInit ()
{

	return S_OK;
}

HRESULT DxEffCharElectricity::InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr = S_OK;
	D3DXVECTOR3	vNormal;

	//	Note : Eff Group의 매트릭스 트랜스폼 지정.
	//
	if ( m_szTraceUP[0] == NULL )	return E_FAIL;
	if ( m_szTraceDOWN[0] == NULL )	return E_FAIL;
	
	if ( m_pCharPart )
	{
		hr = m_pCharPart->CalculateVertexInflu ( m_szTraceUP, m_vTransUP, vNormal );
		if ( FAILED(hr) )	return E_FAIL;

		hr = m_pCharPart->CalculateVertexInflu ( m_szTraceDOWN, m_vTransDOWN, vNormal );
		if ( FAILED(hr) )	return E_FAIL;
	}
	else if ( m_pSkinPiece )
	{
		hr = m_pSkinPiece->CalculateVertexInflu ( m_szTraceUP, m_vTransUP, vNormal );
		if ( FAILED(hr) )	return E_FAIL;

		hr = m_pSkinPiece->CalculateVertexInflu ( m_szTraceDOWN, m_vTransDOWN, vNormal );
		if ( FAILED(hr) )	return E_FAIL;
	}

	//	Note : 텍스쳐 로드
	//
	TextureManager::LoadTexture ( m_szTexture_In, pd3dDevice, m_pTexture_In, 0, 0 );

	TextureManager::LoadTexture ( m_szTexture_Out, pd3dDevice, m_pTexture_Out, 0, 0 );

	if ( !m_pTexture_In )
	{
		char szMsg[256];
		sprintf ( szMsg, "이미지 파일 '%s' 읽기에 실패", m_szTexture_In );
		MessageBox ( NULL, szMsg, "ERROR", MB_OK );
	}

	if ( !m_pTexture_Out )
	{
		char szMsg[256];
		sprintf ( szMsg, "이미지 파일 '%s' 읽기에 실패", m_szTexture_Out );
		MessageBox ( NULL, szMsg, "ERROR", MB_OK );
	}

	CreateBlurVB ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffCharElectricity::CreateBlurVB( LPDIRECT3DDEVICE8 pd3dDevice )
{
	D3DXVECTOR3 pDelta;


	m_dwOriginNum = m_dwDivision+2;
	m_nVertexNum = (m_dwDivision+2)*2;

	m_fAlphaTime1 = m_fLifeTime*m_fAlphaRate1/100.f;
	m_fAlphaTime2 = m_fLifeTime*m_fAlphaRate2/100.f;

	if ( m_fAlphaRate1 == 0.f )					m_fAlphaDelta1	= 0.f;
	else										m_fAlphaDelta1 = ( m_fAlphaMid1 - m_fAlphaStart ) / (m_fAlphaTime1);
	if ( m_fAlphaRate2-m_fAlphaRate1 == 0.f )	m_fAlphaDelta2	= 0.f;
	else										m_fAlphaDelta2 = ( m_fAlphaMid2 - m_fAlphaMid1 ) / (m_fAlphaTime2-m_fAlphaTime1);
	if ( m_fAlphaRate2 == 100.f )				m_fAlphaDelta3	= 0.f;
	else										m_fAlphaDelta3 = ( m_fAlphaEnd - m_fAlphaMid2 ) / (m_fLifeTime-m_fAlphaTime2);


	//	Note : 분할면이 있다면 점을 얻어 와라.
	//
	SAFE_DELETE_ARRAY ( m_pRootVertex );

	m_pRootVertex = new D3DROOT[m_dwOriginNum];

	float Temp = (float)(m_dwOriginNum-2);
	if ( Temp > 0.f )
	{
		pDelta = ( m_vTransDOWN - m_vTransUP ) / (float)(m_dwOriginNum-1);
		for ( int i=0; i<m_dwOriginNum; i++ )
		{
			m_pRootVertex[i].vPos = m_vTransUP + pDelta*(float)i;
		}
	}


	D3DXVECTOR3		vCarDirec;
	D3DXVECTOR3		vDirection;
	D3DXVECTOR3&	vFromPt		= DxViewPort::Instance.GetFromPt();
	D3DXVECTOR3&	vLookatPt	= DxViewPort::Instance.GetLookatPt();
	D3DXVECTOR3		vParent(0.f,0.f,0.f);

	//	Note : 초기 위치 생성
	//
	SAFE_DELETE_ARRAY ( m_pVertex );
	m_pVertex = new D3DVERTEX[m_nVertexNum];

	SAFE_DELETE_ARRAY ( m_pVertex_Out );
	m_pVertex_Out = new D3DVERTEX[m_nVertexNum];

	for ( int i=0; i<m_dwOriginNum; i++ )
	{
		vCarDirec = vLookatPt - vFromPt;

		vDirection = DirectSum( m_pRootVertex, i, m_dwOriginNum-1 );	// 방향을 얻을 수 있다.. !!

		D3DXVec3Cross ( &m_pRootVertex[i].vLookatCross, &vDirection, &vCarDirec );
		D3DXVec3Normalize ( &m_pRootVertex[i].vLookatCross, &m_pRootVertex[i].vLookatCross );		// 계속 값을 받아서 바꾸어줘야 하는 것.!!

		m_pVertex[i*2+0].vPos = m_pRootVertex[i].vPos+(m_pRootVertex[i].vLookatCross*m_fWidth_In);	//	m_dwAngel
		m_pVertex[i*2+1].vPos = m_pRootVertex[i].vPos-(m_pRootVertex[i].vLookatCross*m_fWidth_In);

		//	Note : 잠시.. 꼭 지워야 함.
		//
		m_cColor.a = 1.f;
		m_cColor.r = 1.f;
		m_cColor.g = 1.f;
		m_cColor.b = 1.f;


		m_pVertex[i*2+0].Diffuse = m_cColor;
		m_pVertex[i*2+1].Diffuse = m_cColor;

		m_pVertex[i*2+0].vTex = D3DXVECTOR2 ( 0.f, (float)i/(float)(m_dwOriginNum-1) );
		m_pVertex[i*2+1].vTex = D3DXVECTOR2 ( 1.f, (float)i/(float)(m_dwOriginNum-1) );

		m_pVertex_Out[i*2+0].vPos = m_pRootVertex[i].vPos+(m_pRootVertex[i].vLookatCross*m_fWidth_Out);	//	m_dwAngel
		m_pVertex_Out[i*2+1].vPos = m_pRootVertex[i].vPos-(m_pRootVertex[i].vLookatCross*m_fWidth_Out);

		m_pVertex_Out[i*2+0].Diffuse = m_cColor;
		m_pVertex_Out[i*2+1].Diffuse = m_cColor;

		m_pVertex_Out[i*2+0].vTex = D3DXVECTOR2 ( 0.f, (float)i/(float)(m_dwOriginNum-1) );
		m_pVertex_Out[i*2+1].vTex = D3DXVECTOR2 ( 1.f, (float)i/(float)(m_dwOriginNum-1) );

		m_pVertex_Out[i*2+0].vTex = D3DXVECTOR2 ( 0.f, (float)i );
		m_pVertex_Out[i*2+1].vTex = D3DXVECTOR2 ( 1.f, (float)i );
	}

//	SAFE_DELETE_ARRAY ( m_pTargetPos );
//	m_pTargetPos = new POSITION[m_nVertexNum];

	//	Note : 빌보드 및 점들 업데이트..
	//
	UpdatePos ();

	return S_OK;
}

HRESULT DxEffCharElectricity::RestoreDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

HRESULT DxEffCharElectricity::InvalidateDeviceObjects ()
{
	m_pd3dDevice = NULL;

	return S_OK;
}

HRESULT DxEffCharElectricity::DeleteDeviceObjects ()
{
	if ( m_pTexture_In )
		TextureManager::ReleaseTexture ( m_pTexture_In );

	if ( m_pTexture_Out )
		TextureManager::ReleaseTexture ( m_pTexture_Out );

	SAFE_DELETE_ARRAY ( m_pRootVertex );
	SAFE_DELETE_ARRAY ( m_pVertex );
	SAFE_DELETE_ARRAY ( m_pVertex_Out );
//	SAFE_DELETE_ARRAY ( m_pTargetPos );

	return S_OK;
}

HRESULT DxEffCharElectricity::FinalCleanup ()
{

	return S_OK;
}

HRESULT DxEffCharElectricity::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	m_fElapsedTime += fElapsedTime;
/*
	//	Note : 반복할 때 시간 및 여러 값을 초기화 한다.
	//
	if ( m_fGAge>=(m_fGBeginTime+m_fLifeTime) )
	{
		m_fGAge = m_fGAge - (m_fGBeginTime+m_fGLifeTime);
		m_fAlpha = m_fAlphaStart;
		m_cColor = m_cColorStart;
	}

	//	Note : 컬러 조정 
	//
	if ( m_fGAge >= m_fGBeginTime && m_fGAge < m_fAlphaTime1)
		m_fAlpha += m_fAlphaDelta1*fElapsedTime;
	else if ( m_fGAge >= m_fAlphaTime1 && m_fGAge < m_fAlphaTime2)	
		m_fAlpha += m_fAlphaDelta2*fElapsedTime;
	else		
		m_fAlpha += m_fAlphaDelta3*fElapsedTime;

	m_cColor += m_cColorDelta*fElapsedTime;
	m_cColor.a = m_fAlpha;		// 알파값 적용
*/
	return S_OK;
}

HRESULT	DxEffCharElectricity::Render ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr = S_OK;
	D3DXVECTOR3	vNormal;

	if ( m_pCharPart )
	{
		hr = m_pCharPart->CalculateVertexInflu ( m_szTraceUP, m_vTransUP, vNormal );
		if ( FAILED(hr) )	return E_FAIL;

		hr = m_pCharPart->CalculateVertexInflu ( m_szTraceDOWN, m_vTransDOWN, vNormal );
		if ( FAILED(hr) )	return E_FAIL;
	}
	else if ( m_pSkinPiece )
	{
		hr = m_pSkinPiece->CalculateVertexInflu ( m_szTraceUP, m_vTransUP, vNormal );
		if ( FAILED(hr) )	return E_FAIL;

		hr = m_pSkinPiece->CalculateVertexInflu ( m_szTraceDOWN, m_vTransDOWN, vNormal );
		if ( FAILED(hr) )	return E_FAIL;
	}

	//	Note : 위치 변경 되었을 때 !!
	//
	if ( (m_vTransUP!=m_vPrevUP) ||	(m_vTransDOWN!=m_vPrevDOWN) )	UpdatePos ();

	D3DXMATRIX matWorld, matIdentity;

	pd3dDevice->GetTransform ( D3DTS_WORLD, &matWorld );

	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );


	pd3dDevice->CaptureStateBlock ( m_dwSavedStateBlock );
	pd3dDevice->ApplyStateBlock ( m_dwEffectStateBlock );

	D3DCAPS8 d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );
	DWORD	dwSrcBlend, dwDestBlend, dwColorOP, dwZWriteEnable, dwAlphaBlendEnable,
			dwAlpgaTestEnable, dwAlphaRef, dwAlphaFunc;

	pd3dDevice->GetRenderState ( D3DRS_SRCBLEND,			&dwSrcBlend );
	pd3dDevice->GetRenderState ( D3DRS_DESTBLEND,			&dwDestBlend );
	pd3dDevice->GetRenderState ( D3DRS_ZWRITEENABLE,		&dwZWriteEnable );
	pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
	pd3dDevice->GetRenderState ( D3DRS_ALPHATESTENABLE,		&dwAlpgaTestEnable );
	pd3dDevice->GetRenderState ( D3DRS_ALPHAREF,			&dwAlphaRef );
	pd3dDevice->GetRenderState ( D3DRS_ALPHAFUNC,			&dwAlphaFunc );
	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP,	&dwColorOP );

	//	Note : 블렌딩 요소 셋팅
	//
	switch ( m_nBlend )
	{
	case 1 :
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		break;
	case 2:
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE2X );

		break;
	case 3:
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE4X );

		break;			
	case 4:
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		break;
	case 5:
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );

		if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x80 );
			pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		}
		break;
	}


	//	Note : 텍스쳐 설정.
	//
	pd3dDevice->SetIndices ( NULL, 0 );
	pd3dDevice->SetStreamSource ( 0, NULL, 0 );
	pd3dDevice->SetVertexShader ( D3DVERTEX::FVF );

	if ( m_dwFlag & USEIN )
	{
		pd3dDevice->SetTexture ( 0, m_pTexture_In );
		pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, m_nVertexNum-2, m_pVertex, sizeof(D3DVERTEX) );
	}
	if ( m_dwFlag & USEOUT )
	{
		for ( DWORD i=0; i<m_dwLightning; i++ )
		{
			// Note : 계산.. 랜덤
			UpdatePos_Out ();

			pd3dDevice->SetTexture ( 0, m_pTexture_Out );
			pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, m_nVertexNum-2, m_pVertex_Out, sizeof(D3DVERTEX) );
		}
	}

	pd3dDevice->SetTexture( 0, NULL );

	
	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			dwSrcBlend );
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			dwDestBlend );
	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		dwZWriteEnable );
	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
	pd3dDevice->SetRenderState ( D3DRS_ALPHATESTENABLE,		dwAlpgaTestEnable );
	pd3dDevice->SetRenderState ( D3DRS_ALPHAREF,			dwAlphaRef );
	pd3dDevice->SetRenderState ( D3DRS_ALPHAFUNC,			dwAlphaFunc );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	dwColorOP );

	pd3dDevice->ApplyStateBlock ( m_dwSavedStateBlock );

	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );

	m_vPrevUP = m_vTransUP;
	m_vPrevDOWN = m_vTransDOWN;

	return S_OK;
}

D3DXVECTOR3		DxEffCharElectricity::DirectSum ( D3DROOT* pPos, int i, int End )
{
	D3DXVECTOR3 pTempPos1;
	D3DXVECTOR3 pTempPos2;

	if ( i==0 )		pTempPos1 = D3DXVECTOR3 ( 0.f,0.f,0.f );
	else
	{
		pTempPos1 = pPos[i].vPos - pPos[i-1].vPos;
	}

	if ( i>=End )	pTempPos2 = D3DXVECTOR3 ( 0.f,0.f,0.f );
	else
	{
		pTempPos2 = pPos[i+1].vPos - pPos[i].vPos;
	}

	pTempPos1 = pTempPos1 + pTempPos2;
	D3DXVec3Normalize ( &pTempPos1, &pTempPos1 );

	return pTempPos1;
}

void	DxEffCharElectricity::UpdatePos ()
{
	D3DXVECTOR3		vCarDirec;
	D3DXVECTOR3		vDirection;
	D3DXVECTOR3&	vFromPt		= DxViewPort::Instance.GetFromPt();
	D3DXVECTOR3&	vLookatPt	= DxViewPort::Instance.GetLookatPt();

	
	//	Note : 뿌려질 위치 설정 
	//
	m_pRootVertex[0].vPos					= m_vTransUP;
	m_pRootVertex[m_dwOriginNum-1].vPos		= m_vTransDOWN;

	//	Note : 회전 
	//
	D3DXVECTOR3		vVel(0.f,0.f,0.f);
	//	% 계산상 오류를 피하기 위해서 
	//
	if ( m_dwVelocity > 0 )
	{
		vVel = D3DXVECTOR3 ( (float)(rand()%(m_dwVelocity*10)/10.f)-((float)m_dwVelocity/2.f), (float)(rand()%(m_dwVelocity*10)/10.f)-((float)m_dwVelocity/2.f), (float)(rand()%(m_dwVelocity*10)/10.f)-((float)m_dwVelocity/2.f) );
	}
		
	for ( int i=1; i<m_dwOriginNum-1; i++ )
	{
		m_pRootVertex[i].vPos = m_pRootVertex[i-1].vPos + (vVel / (float)(i));
		m_pRootVertex[i].vPos += ((m_pRootVertex[m_dwOriginNum-1].vPos - m_pRootVertex[i].vPos) / (float)(m_dwOriginNum-i));
	}

	for ( int i=0; i<m_dwOriginNum; i++ )
	{
		vCarDirec = vLookatPt - vFromPt;

		vDirection = DirectSum( m_pRootVertex, i, m_dwOriginNum-1 );	// 방향을 얻을 수 있다.. !!

		D3DXVec3Cross ( &m_pRootVertex[i].vLookatCross, &vDirection, &vCarDirec );
		D3DXVec3Normalize ( &m_pRootVertex[i].vLookatCross, &m_pRootVertex[i].vLookatCross );		// 계속 값을 받아서 바꾸어줘야 하는 것.!!

		m_pVertex[i*2+0].vPos = m_pRootVertex[i].vPos+(m_pRootVertex[i].vLookatCross*m_fWidth_In);	//	m_dwAngel
		m_pVertex[i*2+1].vPos = m_pRootVertex[i].vPos-(m_pRootVertex[i].vLookatCross*m_fWidth_In);

		m_pVertex[i*2+0].Diffuse = m_cColor;
		m_pVertex[i*2+1].Diffuse = m_cColor;

		m_pVertex_Out[i*2+0].Diffuse = m_cColor;
		m_pVertex_Out[i*2+1].Diffuse = m_cColor;
	}
}

void	DxEffCharElectricity::UpdatePos_Out ()
{
	D3DXVECTOR3		vDirection;

	for ( int i=0; i<m_dwOriginNum; i++ )
	{
		//	% 계산상 오류를 피하기 위해서 
		//
		if ( m_dwMaxLenth > 0 )
		{
			if ( (m_dwFlag&USESTART) && i==0 )						
			{ 
				m_pVertex_Out[i*2+0].vPos = m_pRootVertex[i].vPos+(m_pRootVertex[i].vLookatCross*m_fWidth_Out);
				m_pVertex_Out[i*2+1].vPos = m_pRootVertex[i].vPos-(m_pRootVertex[i].vLookatCross*m_fWidth_Out);
			}
			else if ( (m_dwFlag&USEEND) && i >= m_dwOriginNum-1 )
			{
				m_pVertex_Out[i*2+0].vPos = m_pRootVertex[i].vPos+(m_pRootVertex[i].vLookatCross*m_fWidth_Out);
				m_pVertex_Out[i*2+1].vPos = m_pRootVertex[i].vPos-(m_pRootVertex[i].vLookatCross*m_fWidth_Out);
			}
			else
			{
				vDirection = D3DXVECTOR3 ( (float)(rand()%(m_dwMaxLenth*10)/10.f)-((float)m_dwMaxLenth/2.f), (float)(rand()%(m_dwMaxLenth*10)/10.f)-((float)m_dwMaxLenth/2.f), (float)(rand()%(m_dwMaxLenth*10)/10.f)-((float)m_dwMaxLenth/2.f) );

				m_pVertex_Out[i*2+0].vPos = m_pRootVertex[i].vPos+(m_pRootVertex[i].vLookatCross*m_fWidth_Out) + vDirection;
				m_pVertex_Out[i*2+1].vPos = m_pRootVertex[i].vPos-(m_pRootVertex[i].vLookatCross*m_fWidth_Out) + vDirection;
			}
		}
	}

	//	Note ; 속도 조절
	//
//	m_nRoop--;
//	if ( m_nRoop < 0 )	m_nRoop = m_nDivide-1;
}

HRESULT	DxEffCharElectricity::LoadFile ( basestream &SFile, LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	//
	if ( dwVer == VERSION )
	{
		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;

	}
	else
	{
		//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
		//
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		
		return E_FAIL;
	}

	return S_OK;
}

HRESULT	DxEffCharElectricity::SaveFile ( basestream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	//
	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}
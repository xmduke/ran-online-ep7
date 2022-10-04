// DxEffectBlurC.cpp: implementation of the DxEffectBlurC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DxFrameMesh.h"
#include "DxEffectMan.h"

#include "DxEffectBlurC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW


//const DWORD DxEffectBlurC::BLURCURVE::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE;
const DWORD DxEffectBlurC::BLURTEX::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
const DWORD DxEffectBlurC::TYPEID = DEF_EFFECT_BLURCURVE;
const DWORD	DxEffectBlurC::VERSION = 0x00000100;
const char DxEffectBlurC::NAME[] = "[3001]_[ 잔상효과 (곡선) ]";
const DWORD DxEffectBlurC::FLAG = NULL;

void DxEffectBlurC::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(BLURCURVE_PROPERTY);
	dwVer = VERSION;
}

void DxEffectBlurC::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(BLURCURVE_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
}

DWORD	DxEffectBlurC::m_dwSavedStateBlock(0x00000000);
DWORD	DxEffectBlurC::m_dwEffectStateBlock(0x00000000);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxEffectBlurC::DxEffectBlurC() :
	m_pVB(NULL),
	m_fTime(0.0f),
	m_fTestTime(0.0f),
	m_iBlurNum(0),
	m_iAdd(0),
	m_dwCount(0),
	m_vBlurTexNow(NULL),
	m_pddsTexture(NULL),
	m_bUse(FALSE),
	m_bTestPlay(FALSE),
	m_dwFaces(20),
	m_vColorUP(D3DCOLOR_ARGB(255,255,87,41)),
	m_vColorDOWN(D3DCOLOR_ARGB(105,145,133,255)),
	m_iBlurAdd(1),
	m_iColorUP(0),
	m_iColorDOWN(0),
	m_vBlurStart(D3DXVECTOR3(0,20,0)),
	m_vBlurEnd(D3DXVECTOR3(0,-20,0)),
	m_bTex(FALSE)
{
	strcpy ( m_szTexture, "_Spr_spotlight.tga" );
}

DxEffectBlurC::~DxEffectBlurC()
{
	SAFE_DELETE_ARRAY(m_szAdaptFrame);

	CleanUp ();
}

HRESULT DxEffectBlurC::CreateDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
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
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_dwSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_dwEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffectBlurC::ReleaseDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
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

HRESULT DxEffectBlurC::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	m_pframeCur = pframeCur;

	if ( pframeCur->pmsMeshs != NULL )
	{
		pmsMeshs = pframeCur->pmsMeshs;

		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				MakeEffBlurCMeshs( pd3dDevice );
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

HRESULT DxEffectBlurC::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICE8 pd3dDevice )
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


HRESULT DxEffectBlurC::InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	//	Note : 텍스쳐의 읽기 오류는 무시한다.
	//
	if ( !m_pddsTexture )
		TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pddsTexture, 0, 0 );

	if ( !m_pddsTexture )
	{
		char szMsg[256];
		sprintf ( szMsg, "이미지 파일 '%s' 읽기에 실패", m_szTexture );
		MessageBox ( NULL, szMsg, "ERROR", MB_OK );
	}

	return S_OK;
}

HRESULT DxEffectBlurC::DeleteDeviceObjects ()
{
	SAFE_RELEASE ( m_pVB );

	//	Note : 이전 텍스쳐가 로드되어 있을 경우 제거.
	//
	if ( m_pddsTexture )
		TextureManager::ReleaseTexture ( m_pddsTexture );
	m_pddsTexture = NULL;

	m_iBlurNum = 0;
	m_dwCount = 0;
	m_vBlurTexNow = NULL;

	return S_OK;
}

HRESULT DxEffectBlurC::FrameMove ( float fTime, float fElapsedTime )
{
	m_fTime += fElapsedTime;

	m_fTestTime = fTime;

	return S_OK;
}

HRESULT DxEffectBlurC::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;


	//	Note : Render ( 원본 )
	//
	if ( m_bTestPlay )
	{
		//	Note : Render ( 원본 )
		//
		if ( pframeCur->pmsMeshs != NULL )
		{
			//	Note : 트렌스폼 설정.
			//
			D3DXMATRIX	matWorld, matTran, matRota, matRotaX;

			matWorld = pframeCur->matCombined;

			D3DXMatrixTranslation ( &matTran, 0.f, 0.f, -20.f );
			D3DXMatrixRotationY ( &matRota, m_fTestTime*2 );
			D3DXMatrixRotationX ( &matRotaX, m_fTestTime );
			D3DXMatrixMultiply ( &(pframeCur->matCombined), &(pframeCur->matCombined), &matTran );
			D3DXMatrixMultiply ( &(pframeCur->matCombined), &(pframeCur->matCombined), &matRota );
			D3DXMatrixMultiply ( &(pframeCur->matCombined), &(pframeCur->matCombined), &matRotaX );

			matTestWorld = pframeCur->matCombined;		// Test 잔상때 쓰이기 위해서 

			hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );
			if (FAILED(hr))
				return hr;

			//	Note : 메쉬 그리기.
			//
			pmsMeshs = pframeCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pLocalMesh )
				{
					pmsMeshs->Render ( pd3dDevice );
				}

				pmsMeshs = pmsMeshs->pMeshNext;
			}

			pframeCur->matCombined = matWorld;
		}
	}

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
		DWORD					dwVerts, dwFaces;


		BLURTEX*				pVertex;
		
		dwFVFSize = BLURTEX::FVF;
		dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );

		dwVerts = (m_dwFaces*2)+2;
		dwFaces = (m_dwFaces*2);


		//	Note : Shader 상수 선언
		//

		if ( m_fTime > 0.05f )
		{
			m_fTime = 0.0f;

			int iTempBlurAdd;
			if ( m_iBlurAdd <= 2 ) 
				iTempBlurAdd = 2;
			else
				iTempBlurAdd = m_iBlurAdd;

			//	Note : 알파 빼야 할 값을 지정
			//
			int FirstAlpha1 = ((m_vColorUP&0xff000000)>>24);
			int TempUp = (FirstAlpha1 - m_iColorUP) / (dwFaces/2-2);
			int FirstAlpha2 = ((m_vColorDOWN&0xff000000)>>24);
			int TempDown = (FirstAlpha2 - m_iColorDOWN) / (dwFaces/2-2);
			TempUp /= iTempBlurAdd-1;
			TempDown /= iTempBlurAdd-1;

			//	Note : 칼에 붙어 있는 잔상 만들기.
			//
			int FirstAlpha11 = ((m_pFirstBlur[2].d&0xff000000)>>24);
			DWORD Sum = (m_pFirstBlur[2].d&0xffffff);
			FirstAlpha11 = FirstAlpha11 - TempUp;
			if ( FirstAlpha11 < 0 )
				FirstAlpha11 = 0;
			m_pFirstBlur[2].d = (FirstAlpha11<<24) + Sum;
			m_pFirstBlur[2].t.x += 1.f;

			FirstAlpha11 = ((m_pFirstBlur[3].d&0xff000000)>>24);
			Sum = (m_pFirstBlur[3].d&0xffffff);
			FirstAlpha11 = FirstAlpha11 - TempDown;
			if ( FirstAlpha11 < 0 )
				FirstAlpha11 = 0;
			m_pFirstBlur[3].d = (FirstAlpha11<<24) + Sum;
			m_pFirstBlur[3].t.x += 1.f;




			BLURTEX* VertexFirst;
			m_pVB->Lock( 0, 0, (BYTE**)&pVertex, 0 );
			
			VertexFirst = pVertex;


			//	Note : 디퓨즈 값을 슬슬 빼준다.
			//			dwFaces/2  -> 이건 알파값 잘 나누어 준다.
			//
			for( DWORD i=0; i<dwVerts/2; i++ )
			{
				FirstAlpha1 = ((pVertex->d&0xff000000)>>24);
				DWORD Sum = (pVertex->d&0xffffff);
				FirstAlpha1 = FirstAlpha1 - TempUp;
				if ( FirstAlpha1 < 0 )
					FirstAlpha1 = 0;
				pVertex->d = (FirstAlpha1<<24) + Sum;
				pVertex->t.x += 1.0f;				

				pVertex++;
				
				FirstAlpha2 = ((pVertex->d&0xff000000)>>24);
				Sum = (pVertex->d&0xffffff);
				FirstAlpha2 = FirstAlpha2 - TempDown;
				if ( FirstAlpha2 < 0 )
					FirstAlpha2 = 0;
				pVertex->d = (FirstAlpha2<<24) + Sum;
				pVertex->t.x += 1.0f;				

				pVertex++;
			}


			if ( m_iAdd >= m_iBlurAdd && m_bUse )
			{


				D3DXVECTOR3	vTempUp;
				D3DXVECTOR3	vTempDown;

				// Vertex 를 World 좌표로 변환
				D3DXVec3TransformCoord ( &vTempUp, &m_vBlurStart, &matTestWorld );
				D3DXVec3TransformCoord ( &vTempDown, &m_vBlurEnd, &matTestWorld );



				
				pVertex = VertexFirst;


				//	Note : 새로 생기는 점이 버텍스 버퍼의 제일 끝을 넘어갔을때 이것이 발생한다.
				//			
				if( m_dwCount >= dwFaces )	
				{
					//	Note : 처음 2개 점의 위치와 디퓨즈값은
					//			마지막의 2개 점의 위치와 디퓨즈값과 같아야 한다.
					//
					if( m_vBlurTexNow )
						pVertex = m_vBlurTexNow;

					//	Note : 잔상을 사용 안 하면 새로운 점을 생성 안한다.
					//
					if ( m_bUse )
					{
						pVertex->p = vTempUp;
						pVertex->d = m_vColorUP;
						pVertex->t.x = 0.0f;
						pVertex++;
						pVertex->p = vTempDown;
						pVertex->d = m_vColorDOWN;
						pVertex->t.x = 0.0f;
					}
					else
					{
						pVertex->p = vTempUp;
						pVertex->d = 0x00000000;
						pVertex->t.x = 0.0f;
						pVertex++;
						pVertex->p = vTempDown;
						pVertex->d = 0x00000000;
						pVertex->t.x = 0.0f;
					}

					m_dwCount = 0;
					m_vBlurTexNow = VertexFirst;
				}
				


				//	Note : 계속해서 새로운 점을 생성해 낸다.
				//
				if( m_vBlurTexNow )
					pVertex = m_vBlurTexNow;

				if ( m_bUse )
				{
					pVertex->p = vTempUp;
					pVertex->d = m_vColorUP;
					pVertex->t.x = 0.0f;
					pVertex++;
					pVertex->p = vTempDown;
					pVertex->d = m_vColorDOWN;
					pVertex->t.x = 0.0f;
					pVertex++;

					m_pFirstBlur[2].p = vTempUp;
					m_pFirstBlur[2].d = m_vColorUP;
					m_pFirstBlur[2].t.x = 0.0f;
					m_pFirstBlur[3].p = vTempDown;
					m_pFirstBlur[3].d = m_vColorDOWN;
					m_pFirstBlur[3].t.x = 0.0f;
				}
				else
				{
					pVertex->p = vTempUp;
					pVertex->d = 0x00000000;
					pVertex->t.x = 0.0f;
					pVertex++;
					pVertex->p = vTempDown;
					pVertex->d = 0x00000000;
					pVertex->t.x = 0.0f;
					pVertex++;
				}

				m_dwCount += 2;
				m_iBlurNum += 2;

				m_vBlurTexNow = pVertex;


				m_iAdd = 0;
			}
			m_iAdd++;

			
			m_pVB->Unlock();
		}

		//	Note : 칼에 붙어 있는 잔상 만들기
		//
		D3DXVECTOR3	vTempUp;
		D3DXVECTOR3	vTempDown;

		// Vertex 를 World 좌표로 변환
		D3DXVec3TransformCoord ( &vTempUp, &m_vBlurStart, &matTestWorld );
		D3DXVec3TransformCoord ( &vTempDown, &m_vBlurEnd, &matTestWorld );

		if ( m_bUse )
		{
			m_pFirstBlur[0].p = vTempUp;
			m_pFirstBlur[0].d = m_vColorUP;
			m_pFirstBlur[1].p = vTempDown;
			m_pFirstBlur[1].d = m_vColorDOWN;
		}


		pd3dDevice->SetVertexShader ( BLURTEX::FVF );

		pd3dDevice->SetTexture ( 0, m_pddsTexture );

		//	Note : 이전 상태 백업.
		//
		pd3dDevice->CaptureStateBlock ( m_dwSavedStateBlock );

		//	Note : 랜더링 상태 조정.
		//
		pd3dDevice->ApplyStateBlock ( m_dwEffectStateBlock );

		if ( m_bTex )
			pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );


		//	Note : 칼에 붙어 있는 잔상 만들기 
		//
		pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, m_pFirstBlur, sizeof(BLURTEX) );



		pd3dDevice->SetIndices ( NULL, 0 );
		pd3dDevice->SetStreamSource ( 0, m_pVB, dwFVFSize );



		int iFaces;
		iFaces = dwFaces-m_dwCount;	//	1
		if( iFaces > 0 && m_iBlurNum >= (int)dwFaces )
		{
			pd3dDevice->DrawPrimitive ( D3DPT_TRIANGLESTRIP, m_dwCount, iFaces );
		}
		
		
		iFaces = m_dwCount-2;		//	1
		if( iFaces > 0 )
		{
			pd3dDevice->DrawPrimitive ( D3DPT_TRIANGLESTRIP, 0, iFaces );
		}
		

		pd3dDevice->SetIndices ( NULL, 0 );
		pd3dDevice->SetStreamSource ( 0, NULL, 0 );



		//	Note : 이전상태로 복원.
		//
		pd3dDevice->ApplyStateBlock ( m_dwSavedStateBlock );



		pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );


	//	pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		pd3dDevice->SetTexture ( 0, NULL );
	}

	return S_OK;
}

HRESULT DxEffectBlurC::MakeEffBlurCMeshs( LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr;

	SAFE_RELEASE ( m_pVB );


	BLURTEX*			pVertexC;

	pd3dDevice->CreateVertexBuffer ( ((m_dwFaces*2)+2)*sizeof(BLURTEX), D3DUSAGE_WRITEONLY, 
										BLURTEX::FVF, D3DPOOL_MANAGED, &m_pVB  );

	hr = m_pVB->Lock ( 0, 0, (BYTE**)&pVertexC, 0 );
	if ( FAILED(hr) )	return E_FAIL;


	D3DXVECTOR3	vTempUp;
	D3DXVECTOR3	vTempDown;

	// Vertex 를 World 좌표로 변환
	D3DXVec3TransformCoord ( &vTempUp, &m_vBlurStart, &matTestWorld );
	D3DXVec3TransformCoord ( &vTempDown, &m_vBlurEnd, &matTestWorld );


	m_pFirstBlur[0].p = vTempUp;
	m_pFirstBlur[0].d = D3DCOLOR_ARGB( 0, 0, 0, 0 );
	m_pFirstBlur[0].t = D3DXVECTOR2( 0.f, 0.0f );
	m_pFirstBlur[1].p = vTempDown;
	m_pFirstBlur[1].d = D3DCOLOR_ARGB( 0, 0, 0, 0 );
	m_pFirstBlur[1].t = D3DXVECTOR2( 0.f, 1.0f );
	m_pFirstBlur[2].p = vTempUp;
	m_pFirstBlur[2].d = D3DCOLOR_ARGB( 0, 0, 0, 0 );
	m_pFirstBlur[2].t = D3DXVECTOR2( 1.0f, 0.0f );
	m_pFirstBlur[3].p = vTempDown;
	m_pFirstBlur[3].d = D3DCOLOR_ARGB( 0, 0, 0, 0 );
	m_pFirstBlur[3].t = D3DXVECTOR2( 1.0f, 1.0f );
	

	for( DWORD i=0; i<((m_dwFaces*2)+2)/2; i++ )
	{
		pVertexC->p = vTempUp;
		pVertexC->d = D3DCOLOR_ARGB( 0, 0, 0, 0 );
		pVertexC->t = D3DXVECTOR2( 0, 0.0f );
		pVertexC++;

		pVertexC->p = vTempDown;
		pVertexC->d = D3DCOLOR_ARGB( 0, 0, 0, 0 );
		pVertexC->t = D3DXVECTOR2( (float)(m_dwFaces*2)+2-i, 1.0f );
		pVertexC++;
	}

	m_pVB->Unlock();

	return S_OK;
}

HRESULT	DxEffectBlurC::SetInitMesh( )
{
	if ( m_pVB )
	{
		D3DXVECTOR3	vTempUp;
		D3DXVECTOR3	vTempDown;

		// Vertex 를 World 좌표로 변환
		D3DXVec3TransformCoord ( &vTempUp, &m_vBlurStart, &matTestWorld );
		D3DXVec3TransformCoord ( &vTempDown, &m_vBlurEnd, &matTestWorld );



		BLURTEX*			pVertexC;

		m_pVB->Lock ( 0, 0, (BYTE**)&pVertexC, 0 );

		for( DWORD i=0; i<((m_dwFaces*2)+2)/2; i++ )
		{
			pVertexC->p = vTempUp;
			pVertexC->d = D3DCOLOR_ARGB( 0, 0, 0, 0 );
			pVertexC->t = D3DXVECTOR2( (float)(m_dwFaces*2)+2-i, 0.0f );
			pVertexC++;

			pVertexC->p = vTempDown;
			pVertexC->d = D3DCOLOR_ARGB( 0, 0, 0, 0 );
			pVertexC->t = D3DXVECTOR2( (float)(m_dwFaces*2)+2-i, 1.0f );
			pVertexC++;
		}

		m_pVB->Unlock();
	}
	m_iBlurNum = 0;
	m_dwCount = 0;
	m_vBlurTexNow = NULL;

	return S_OK;
}

D3DXVECTOR3 DxEffectBlurC::GetPoint ( int UpDown )
{
	D3DXVECTOR3 vPosition;

	if ( UpDown == 1 )
		vPosition = m_vBlurStart;

	else if ( UpDown == 2 )
		vPosition = m_vBlurEnd;

	return vPosition;
}
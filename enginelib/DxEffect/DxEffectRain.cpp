// DxEffectRain.cpp: implementation of the DxEffectRain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DxFrameMesh.h"
#include "DxEffectMan.h"

#include "DxEffectRain.h"


const DWORD DxEffectRain::TYPEID = DEF_EFFECT_RAIN;
const DWORD	DxEffectRain::VERSION = 0x00000100;
const char DxEffectRain::NAME[] = "[14]_[ 빗방울 효과 ]";
const DWORD DxEffectRain::FLAG = NULL;

void DxEffectRain::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(RAIN_PROPERTY);
	dwVer = VERSION;
}

void DxEffectRain::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(RAIN_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
}

//	Note : 정적 변수 생성.
//
DWORD	DxEffectRain::m_dwEffect;

//	Note : 
//
char DxEffectRain::m_strEffect[] =
{
	"vs.1.1	\n"	//

	//;-------------------------------------------------------------------- 
	//;	Normal 벡터와 Rain벡터의 내적 --------- r0.x 에 등록

	//; Transform the normal to world
	"m3x3 r1, v3, c10	\n"

	//	단위 벡터로 만든다.
	"dp3 r1.w, r1, r1	\n"
	"rsq r1.w, r1.w		\n"
	"mul r1, r1, r1.w	\n"

	//; Calculate dp of rain direction and vertex normal
	"dp3 r0.x, r1, -c9	\n"	//	; 노멀 벡터와 Rain 벡터를 내적

	//;----------------------------------------------------------------------
	//;	점을 뿌려줌, 	파티클과의 거리를 계산한다. ----- r1.w 에 등록

	//;Transform vertex position to screen
	"m4x4 r2, v0, c10	\n"
	"m4x4 r4, r2, c14	\n"
	"m4x3 oPos, r4, c18	\n"

	"dp4 r1.w, r4, c21	\n"
	"mov oPos.w, r1.w	\n"

	//;----------------------------------------------------------------------
	//;	무언가 랜덤한 값을..  ----- r2.w 에 등록

	//; Compute animation seed for this vertex
	//; Take the time index and limit it to the range definef for the vertex. (this 
	//; causes the animation to randomize on each vertex
	//; Then we clamp the output to 0-1.
	"rcp r2.x, v1.x			\n"	//		; 1/1~10 까지
	"mul r2.y, r2.x, c29.x	\n"	//	; 시간/1~10
	"expp r2.y, r2.y		\n"	//		; 정수부분은 다 빠진다.
	"mov r2.z, r2.y			\n"	//		; r2.z = r2.y 
	"mul r2.y, r2.y, v1.x	\n"	//	; (시간/1~10)소수부분만 * 1~10
	"min r2.w, r2.y, c29.y	\n"	//	; 결과 값은 1보다 작다. (0에서 1까지 간다. loop)

	//;----------------------------------------------------------------------
	//;	r1.w 를		 1/r1.w 으로 	 ----- r1.w 에 등록

	//;Compute the basic distance based on the distance
	"rcp r1.w, r1.w	\n"	//				; 1/w (used to scale vert proportional to distance)

	//;----------------------------------------------------------------------
	//;	Diffuse 컬러 생성

	//; Figure out final color and alpha
	"add r4.x, c29.y, -r2.w			\n"	//		; c9.z 값에 의해 알파값 변신
	"mul oD0.xyzw, r0.xxxx, r4.xxxx	\n"	//	; 

	//;----------------------------------------------------------------------
	//;		파티클의 사이즈를 결정한다.

	"mul r2.x, r1.w, r2.w		\n"	//		; Scale the splash by the animation seed
				//	; 1/거리 * (0~1)	거리가 멀면 값이 작아진다.		-	r2.w 는 0~1 까지의 값이다.
	"mul r2.x, r2.x, r0.x		\n"	//		; Scale the splash proprotinate to the rain-vertNorm angle
				//	; Normal과 Rain의 내적에 1/거리 * (0~1) 의 곱
	"mul oPts.x, r2.x, c29.z	\n"	//		; Calculate final sphash size by multiplying by a size constant
					//;  값을 변화시켜서 스케일에 변화를 줄수 있다.

						/*
	"vs.1.1	\n"	//

	//;-------------------------------------------------------------------- 
	//;	Normal 벡터와 Rain벡터의 내적 --------- r0.x 에 등록

	//; Transform the normal to world
	"m3x3 r1, v3, c10	\n"

	//	단위 벡터로 만든다.
	"dp3 r1.w, r1, r1	\n"
	"rsq r1.w, r1.w		\n"
	"mul r1, r1, r1.w	\n"

	//; Calculate dp of rain direction and vertex normal
	"dp3 r0.x, r1, -c9	\n"	//	; 노멀 벡터와 Rain 벡터를 내적

	//;----------------------------------------------------------------------
	//;	점을 뿌려줌, 	파티클과의 거리를 계산한다. ----- r1.w 에 등록

	//;Transform vertex position to screen
	"m4x4 r2, v0, c10	\n"
	"m4x4 r4, r2, c14	\n"
	"m4x3 oPos, r4, c18	\n"

	"dp4 r1.w, r4, c21	\n"
	"mov oPos.w, r1.w	\n"

	//;----------------------------------------------------------------------
	//;	무언가 랜덤한 값을..  ----- r2.w 에 등록

	//; Compute animation seed for this vertex
	//; Take the time index and limit it to the range definef for the vertex. (this 
	//; causes the animation to randomize on each vertex
	//; Then we clamp the output to 0-1.
	"rcp r2.x, v1.x			\n"	//		; 1/1~10 까지
	"mul r2.y, r2.x, c29.x	\n"	//	; 시간/1~10
	"expp r2.y, r2.y		\n"	//		; 정수부분은 다 빠진다.
	"mov r2.z, r2.y			\n"	//		; r2.z = r2.y 
	"mul r2.y, r2.y, v1.x	\n"	//	; (시간/1~10)소수부분만 * 1~10
	"min r2.w, r2.y, c29.y	\n"	//	; 결과 값은 1보다 작다. (0에서 1까지 간다. loop)

	//;----------------------------------------------------------------------
	//;	r1.w 를		 1/r1.w 으로 	 ----- r1.w 에 등록

	//;Compute the basic distance based on the distance
	"rcp r1.w, r1.w	\n"	//				; 1/w (used to scale vert proportional to distance)

	//;----------------------------------------------------------------------
	//;	Diffuse 컬러 생성

	//; Figure out final color and alpha
	"add r4.x, c29.y, -r2.w			\n"	//		; c9.z 값에 의해 알파값 변신
	"mul oD0.xyzw, r0.xxxx, r4.xxxx	\n"	//	; 

	//;----------------------------------------------------------------------
	//;		파티클의 사이즈를 결정한다.

//	"sge r2.w, r2.w, c28.y		\n"

	"mul r2.x, r1.w, r2.w		\n"	//		; Scale the splash by the animation seed
				//	; 1/거리 * (0~1)	거리가 멀면 값이 작아진다.		-	r2.w 는 0~1 까지의 값이다.
	"mul r2.x, r2.x, r0.x		\n"	//		; Scale the splash proprotinate to the rain-vertNorm angle
				//	; Normal과 Rain의 내적에 1/거리 * (0~1) 의 곱
	"mul oPts.x, r2.x, c29.z	\n"	//		; Calculate final sphash size by multiplying by a size constant
					//;  값을 변화시켜서 스케일에 변화를 줄수 있다.

					*/
};

DWORD	DxEffectRain::m_dwSavedStateBlock(0x00000000);
DWORD	DxEffectRain::m_dwEffectStateBlock(0x00000000);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxEffectRain::DxEffectRain() :
	m_pddsTexture(NULL),
	m_fTime(0.0f),	
	m_fAlpha(1.0f),		
	m_fScale(150.0f),			// 크기
	m_fRainRate(0.003f)
{
	strcpy ( m_szTexture, "./Rain.bmp" );
}

DxEffectRain::~DxEffectRain()
{
}

HRESULT DxEffectRain::CreateDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr;
	LPD3DXBUFFER pCode = NULL; 

	DWORD dwDecl[] =
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG(0, D3DVSDT_FLOAT3 ),		//	D3DVSDE_POSITION,  0  
		D3DVSD_REG(1, D3DVSDT_FLOAT1 ),		//	음.. 이것땜시 빗방울이 퍼지는 시기가 다르다.
		D3DVSD_REG(3, D3DVSDT_FLOAT3 ),		//	D3DVSDE_  
		D3DVSD_END()
	};


    hr = D3DXAssembleShader( m_strEffect, (UINT)strlen( m_strEffect ), 0, NULL, &pCode, NULL );
	if( FAILED(hr) )	return E_FAIL;

	hr = pd3dDevice->CreateVertexShader ( dwDecl, (DWORD*)pCode->GetBufferPointer(), &m_dwEffect, 
											DxEffectMan::Instance.GetUseSwShader() );
	if( FAILED(hr) )	return E_FAIL;


	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		//
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

		pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, true );		// SPRITE	- TRUE
		pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  false );	// Scale	- FALSE

		// Set point sprite params for rain layer
		pd3dDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.1f) );	// 최소 이미지
		pd3dDevice->SetRenderState( D3DRS_POINTSIZE_MAX, FtoDW(64.0f) );	// 최대 이미지
		pd3dDevice->SetRenderState( D3DRS_POINTSIZE,     FtoDW(32.0f) );	// 평균 이미지 (맞나?)


		//	Note : SetTextureStageState() 선언
		//
	//	pd3dDevice->SetTextureStageState ( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	//	pd3dDevice->SetTextureStageState ( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	//	pd3dDevice->SetTextureStageState ( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );

	//	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	//	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	//	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

	//	pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	//	pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );


		if( which==0 )	pd3dDevice->EndStateBlock( &m_dwSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_dwEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffectRain::ReleaseDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
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

HRESULT DxEffectRain::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICE8 pd3dDevice )
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
				pmsMeshs->MakeEffectMeshs( pd3dDevice, D3DFVF_XYZ|D3DFVF_NORMAL );
				D3DXComputeNormals( pmsMeshs->m_pEffectMesh, NULL );
				MakeEffRainMeshs( pd3dDevice, &(pmsMeshs->m_pEffectMesh) );
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

HRESULT DxEffectRain::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICE8 pd3dDevice )
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


HRESULT DxEffectRain::InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	//	Note : 텍스쳐의 읽기 오류는 무시한다.
	//
	TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pddsTexture, 0, 0 );

	return S_OK;
}

HRESULT DxEffectRain::DeleteDeviceObjects ()
{
	//	Note : 이전 텍스쳐가 로드되어 있을 경우 제거.
	//
	if ( m_pddsTexture )
		TextureManager::ReleaseTexture ( m_pddsTexture );
	m_pddsTexture = NULL;

	return S_OK;
}

HRESULT DxEffectRain::FrameMove ( float fTime, float fElapsedTime )
{
	m_fTime = fTime;

	return S_OK;
}

HRESULT DxEffectRain::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : Render ( 원본 )
	//
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : 트렌스폼 설정.
		//
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
				pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
				pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

				pmsMeshs->Render ( pd3dDevice );
			}

			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	//	Note : Render ( 빗방울 )
	//
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : 메쉬 그리기.
		//
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pEffectMesh )
			{
				DWORD dwFVFSize;
				DWORD dwVerts;
				LPDIRECT3DVERTEXBUFFER8 pVB;
				
				pmsMeshs->m_pEffectMesh->GetVertexBuffer ( &pVB );
				
				dwFVFSize = pmsMeshs->m_pEffectMesh->GetFVF ();
				dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );

				dwVerts = pmsMeshs->m_pEffectMesh->GetNumVertices();


				//	Note : Shader 상수 선언
				//
				pd3dDevice->SetVertexShaderConstant( VSC_CUSTOM+1, D3DXVECTOR4( m_fTime, m_fAlpha, m_fScale, 1.0f ), 1 );
				
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


				pd3dDevice->SetTexture( 0, m_pddsTexture );


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


				pd3dDevice->SetIndices ( NULL, 0 );
				pd3dDevice->SetStreamSource ( 0, pVB, dwFVFSize );
				pd3dDevice->SetVertexShader ( m_dwEffect );
				
				pd3dDevice->DrawPrimitive ( D3DPT_POINTLIST, 0, dwVerts );


				//	Note : 소프트웨어  버텍스 프로세싱
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

// This function computes the surface area of a triangle
float DxEffectRain::CalcSurfaceArea(VERTEX* pFV[3])
{
	float s;
	float len[3];

	for (DWORD i=0;i<3;i++)
	{
		float x,y,z;
		x = pFV[(i+1)%3]->p.x - pFV[i]->p.x;
		y = pFV[(i+1)%3]->p.y - pFV[i]->p.y;
		z = pFV[(i+1)%3]->p.z - pFV[i]->p.z;
		len[i] = (float)sqrt(x*x+y*y+z*z);
	}
	
	s = (len[0]+len[1]+len[2])/2.0f;
	return (float)sqrt(s*(s-len[0])*(s-len[1])*(s-len[2]));
}

//	물방을 버텍스 만들 때 사용한다.
HRESULT DxEffectRain::MakeEffRainMeshs(LPDIRECT3DDEVICE8 pd3dDevice, LPD3DXMESH *pEffectMesh )
{	
	int			attribs = 1;
	RAINVERTEX* pRainVert;
	VERTEX*		Array[3];
	float		area=0;

	VERTEX*		pVertices;
	WORD*		pIndices;
	DWORD		Faces;
	WORD*		pIndicesCur;

	Faces		= (*pEffectMesh)->GetNumFaces ( );

	(*pEffectMesh)->LockVertexBuffer ( 0L, (BYTE**)&pVertices );
    (*pEffectMesh)->LockIndexBuffer ( 0L, (BYTE**)&pIndices );

	pIndicesCur = pIndices;

	for(DWORD i=0; i<Faces; i++)
	{
		Array[0] = &pVertices[pIndicesCur[i*3+0]];
		Array[1] = &pVertices[pIndicesCur[i*3+1]];
		Array[2] = &pVertices[pIndicesCur[i*3+2]];
		area += m_fRainRate*CalcSurfaceArea(Array);
	}


//	Vertices = (int)(area*m_fRainRate);	// 빗방울의 갯수를 결정한다.

	//	빗방울이 하나도 생성이 안돼면 지워버린다.
//	if( Vertices < 1.0f )
//		GXRELEASE(*pEffectMesh);
	
	RAINVERTEX* pRainVertex = new RAINVERTEX[(int)area];
	area = 0;
	DWORD		dwRainVert = 0;

	for( DWORD j=0; j<Faces; j++ )
	{
		Array[0] = &pVertices[pIndicesCur[j*3+0]];
		Array[1] = &pVertices[pIndicesCur[j*3+1]];
		Array[2] = &pVertices[pIndicesCur[j*3+2]];
		area += m_fRainRate*CalcSurfaceArea(Array);	// 1면의 면적을 계산해서 빗방울 갯수 정함.
		//area += m_fRainRate;							// 1면당 - 생성할 빗방울 갯수.

		// Here we create random vertices in the triangle. The randomness is defines
		// based on 2 edge vector of the triangle. We compute a ratio r1 and r2 which represent a fraction
		// of the edge. If we compute the point as pt = r1*v1 + r2*v2 and we satisfy r1+r2 <= 1.0, the point
		// will be within the triangle.
		while (area>1.0f)
		{
			float r1,r2;
			D3DXVECTOR3 s,t;
			// Create new vertex
			r1 = ((float)(rand()%1000)) / 1000.0f;
			r2 = 1.0f-r1;
			r2 = r2 * ((rand()%1000) / 1000.0f);
			s.x = Array[1]->p.x - Array[0]->p.x;
			s.y = Array[1]->p.y - Array[0]->p.y;
			s.z = Array[1]->p.z - Array[0]->p.z;
			t.x = Array[2]->p.x - Array[0]->p.x;
			t.y = Array[2]->p.y - Array[0]->p.y;
			t.z = Array[2]->p.z - Array[0]->p.z;
			pRainVertex[dwRainVert].p.x = Array[0]->p.x + s.x*r1 + t.x*r2;							// 1 정점 근처에 Vertex를 생성
			pRainVertex[dwRainVert].p.y = Array[0]->p.y + s.y*r1 + t.y*r2;
			pRainVertex[dwRainVert].p.z = Array[0]->p.z + s.z*r1 + t.z*r2;
			pRainVertex[dwRainVert].loop = (100+rand()%900) / 100.0f;							// 랜덤한 값 
			pRainVertex[dwRainVert].n.x = (Array[0]->n.x + Array[1]->n.x + Array[2]->n.x)/3.0f;	// 노멀 벡터 생성
			pRainVertex[dwRainVert].n.y = (Array[0]->n.y + Array[1]->n.y + Array[2]->n.y)/3.0f;
			pRainVertex[dwRainVert].n.z = (Array[0]->n.z + Array[1]->n.z + Array[2]->n.z)/3.0f;

			area -= 1.0f;dwRainVert++;
		}
	}

	(*pEffectMesh)->UnlockVertexBuffer();
    (*pEffectMesh)->UnlockIndexBuffer();

	//	지워주는 거당.
	GXRELEASE(*pEffectMesh);


	// 다시 만든당.
	D3DXCreateMeshFVF( dwRainVert,
						dwRainVert,
						D3DXMESH_SYSTEMMEM, 
						D3DFVF_XYZB1|D3DFVF_NORMAL,
						pd3dDevice,
						&(*pEffectMesh) );

	(*pEffectMesh)->LockVertexBuffer ( 0L, (BYTE**)&pRainVert );

	for( j=0; j<dwRainVert; j++ )
	{
		pRainVert->p	= pRainVertex[j].p;
		pRainVert->loop = pRainVertex[j].loop;
		pRainVert->n	= pRainVertex[j].n;
		pRainVert++;
	}

	(*pEffectMesh)->UnlockVertexBuffer();

	delete [] pRainVertex;


	return S_OK;
}

/*
//	물방을 버텍스 만들 때 사용한다.
HRESULT DxEffectRain::MakeEffRainMeshs(LPDIRECT3DDEVICE8 pd3dDevice, LPD3DXMESH *pEffectMesh )
{	
	int			attribs = 1;
	RAINVERTEX* pRainVert;
	VERTEX*		Array[3];
	float		area=0.0f;
	DWORD		Vertices;	// 빗방울 갯수

	VERTEX*		pVertices;
	WORD*		pIndices;
	float		Faces;
	WORD*		pIndicesCur;

	Faces		= (*pEffectMesh)->GetNumFaces ( );

	(*pEffectMesh)->LockVertexBuffer ( 0L, (BYTE**)&pVertices );
    (*pEffectMesh)->LockIndexBuffer ( 0L, (BYTE**)&pIndices );

	pIndicesCur = pIndices;

	Vertices = Faces*m_fRainRate;

	//	빗방울이 하나도 생성이 안돼면 지워버린다.
	if( Vertices < 1.0f )
		GXRELEASE(*pEffectMesh);
	
	area = 0;
	DWORD		dwRainVert = 0;
	RAINVERTEX* pRainVertex = new RAINVERTEX[Vertices];

	for( DWORD j=0; j<Faces; j++ )
	{
		Array[0] = &pVertices[pIndicesCur[j*3+0]];
		Array[1] = &pVertices[pIndicesCur[j*3+1]];
		Array[2] = &pVertices[pIndicesCur[j*3+2]];
		//area += m_fRainRate*CalcSurfaceArea(Array);	// 1면의 면적을 계산해서 빗방울 갯수 정함.
		area += m_fRainRate;							// 1면당 - 생성할 빗방울 갯수.

		// Here we create random vertices in the triangle. The randomness is defines
		// based on 2 edge vector of the triangle. We compute a ratio r1 and r2 which represent a fraction
		// of the edge. If we compute the point as pt = r1*v1 + r2*v2 and we satisfy r1+r2 <= 1.0, the point
		// will be within the triangle.
		while (area>1.0f)
		{
			float r1,r2;
			D3DXVECTOR3 s,t;
			// Create new vertex
			r1 = ((float)(rand()%1000)) / 1000.0f;
			r2 = 1.0f-r1;
			r2 = r2 * ((rand()%1000) / 1000.0f);
			s.x = Array[1]->p.x - Array[0]->p.x;
			s.y = Array[1]->p.y - Array[0]->p.y;
			s.z = Array[1]->p.z - Array[0]->p.z;
			t.x = Array[2]->p.x - Array[0]->p.x;
			t.y = Array[2]->p.y - Array[0]->p.y;
			t.z = Array[2]->p.z - Array[0]->p.z;
			pRainVertex[dwRainVert].p.x = Array[0]->p.x + s.x*r1 + t.x*r2;							// 1 정점 근처에 Vertex를 생성
			pRainVertex[dwRainVert].p.y = Array[0]->p.y + s.y*r1 + t.y*r2;
			pRainVertex[dwRainVert].p.z = Array[0]->p.z + s.z*r1 + t.z*r2;
			pRainVertex[dwRainVert].loop = (100+rand()%900) / 100.0f;							// 랜덤한 값 
			pRainVertex[dwRainVert].n.x = (Array[0]->n.x + Array[1]->n.x + Array[2]->n.x)/3.0f;	// 노멀 벡터 생성
			pRainVertex[dwRainVert].n.y = (Array[0]->n.y + Array[1]->n.y + Array[2]->n.y)/3.0f;
			pRainVertex[dwRainVert].n.z = (Array[0]->n.z + Array[1]->n.z + Array[2]->n.z)/3.0f;

			area -= 1.0f;dwRainVert++;
		}
	}

	(*pEffectMesh)->UnlockVertexBuffer();
    (*pEffectMesh)->UnlockIndexBuffer();

	//	지워주는 거당.
	GXRELEASE(*pEffectMesh);


	// 다시 만든당.
	D3DXCreateMeshFVF( dwRainVert,
						dwRainVert,
						D3DXMESH_SYSTEMMEM, 
						D3DFVF_XYZB1|D3DFVF_NORMAL,
						pd3dDevice,
						&(*pEffectMesh) );

	(*pEffectMesh)->LockVertexBuffer( 0L, (BYTE**)&pRainVert );

	for( j=0; j<dwRainVert; j++ )
	{
		pRainVert->p	= pRainVertex[j].p;
		pRainVert->loop = pRainVertex[j].loop;
		pRainVert->n	= pRainVertex[j].n;
		pRainVert++;
	}

	(*pEffectMesh)->UnlockVertexBuffer();

	delete [] pRainVertex;


	return S_OK;
}
*/
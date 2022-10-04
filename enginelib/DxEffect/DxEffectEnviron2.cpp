// DxEffectEnviron2.cpp: implementation of the DxEffectShadowHW class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DxFrameMesh.h"
#include "DxEffectMan.h"

#include "DxEffectEnviron2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW


const DWORD DxEffectEnviron2::TYPEID = DEF_EFFECT_ENVIRON2;
const char DxEffectEnviron2::NAME[] = "[3008]_[환경2효과]";

DWORD	DxEffectEnviron2::m_dwEffReflect;

//	Note : 
//
char DxEffectEnviron2::m_strEffReflect[] =
{
	"vs.1.0	;\n" //

	//;transform position
	//;----------------------------------------------------------------------------
	//;	Vertex 를 뿌린다.~~~~~ㅋㅋ
	"m4x4 oPos, v0, c22	;\n"

	//;----------------------------------------------------------------------------
	//;	Vertex 를 World 좌표로 이동 시킨다.

	// 버텍스 좌표를 World 좌표로.
	"m4x3 r0, v0, c10	;\n"
	"mov r0.w, v0.w		;\n"

	//;----------------------------------------------------------------------------
	//		노멀벡터를 단위 벡터로.		---		r9 에 등록

	// 노멀벡터를 가져온다.
	"m3x3 r9, v3, c10	;\n"

	// 단위 벡터로 만들어 준다.
	"dp3 r9.w, r9, r9	\n"	//
	"rsq r9.w, r9.w		\n"	//
	"mul r9, r9, r9.w	\n"	//

	// 카메라 부터 버텍스까지의 단위 벡터를 얻는다.
	"sub r1, r0, c8		\n"	// 거리 계산.
	"dp3 r3.w, r1, r1	\n"	// 단위 벡터 만들기 시작.
	"rsq r3.w, r3.w		\n"	//
	"mul r5, r1, r3.w	\n"	//

	//;-------------------------------------------------------------------- 
	//;		Diffuse 칼라를 생성

	//; The directional light source.
	"dp3 r4.x, r9, -c0		\n" //

	//	Texture 설정.
	//
	"dp3 r4, r5, r9				\n"	// r4 = r3 (내적) 노멀벡터r9
	"add r4, r4, r4				\n"	// r4 = 2 * r4
	"mad oT0.xyz, r9, -r4, r5	\n"	// r3 - ( r4 ) * r9
	"mov oT0.w, c28.z			\n"	// c1.x = 1.0f
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxEffectEnviron2::DxEffectEnviron2() :
	m_wEnvCase(0x0002),		// 종류 고른다.
	m_pCloudTexture(NULL),					// 구름 그림자	// 구름 텍스쳐
	m_fTime(0.0f),							// 구름 그림자	// 시간
	m_fLoop(0.0f),							// 구름 그림자	// 누적돼는 값
	m_fLoopAdd(0.0001f),					// 구름 그림자	// 구름이 흘러가는 속도
	m_fDistance(10000.f),					// 구름 그림자	// 가로 길이 와 세로 길이
	m_fMultiply(1.0f),						// 구름 그림자	// 격자 숫자 ?
	m_pDot3Texture(NULL),	// 내적 블렌딩
	m_fDotPower(0.9f),
	m_pDetailTexture(NULL),						// 새로 시작
	m_fWidth(1.0f),							// 새로 시작
	m_fHeight(1.0f),						// 새로 시작
	m_pCubeTexture(NULL),		// 반사
	m_dwBlendAlpha(0x80ffffff)	// 반사	- 0 에가까우면 원본에 ff에 가까우면 반사에~~!!!
{
	strcpy ( m_szTexture, "cloudcover.bmp" );	// 구름 그림자
	strcpy ( m_szTexDot3, "_Dot_Ripples2_512.tga" );				// 내적 블렌딩
	strcpy ( m_szTexDetail, "_Det_1.bmp" );		// 시작
	strcpy ( m_szTexture1,"_Env_lobbyxpos.bmp" );	// 반사
	strcpy ( m_szTexture2,"_Env_lobbyxneg.bmp" );	// 반사
	strcpy ( m_szTexture3,"_Env_lobbyypos.bmp" );	// 반사
	strcpy ( m_szTexture4,"_Env_lobbyyneg.bmp" );	// 반사
	strcpy ( m_szTexture5,"_Env_lobbyzpos.bmp" );	// 반사
	strcpy ( m_szTexture6,"_Env_lobbyzneg.bmp" );	// 반사
}

DxEffectEnviron2::~DxEffectEnviron2()
{
}

HRESULT DxEffectEnviron2::CreateDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr;
	LPD3DXBUFFER pCode = NULL; 

	DWORD dwDecl[] =
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG(0, D3DVSDT_FLOAT3 ),			//D3DVSDE_POSITION,  0  
		D3DVSD_REG(3, D3DVSDT_FLOAT3 ),		//D3DVSDE_DIFFUSE,   5  
		D3DVSD_REG(7, D3DVSDT_FLOAT2 ),		//D3DVSDE_
		D3DVSD_END()
	};

	//	Note : 반사
	//
	hr = D3DXAssembleShader( m_strEffReflect, (UINT)strlen( m_strEffReflect ), 0, NULL, &pCode, NULL );
	if( FAILED(hr) )	return E_FAIL;

	hr = pd3dDevice->CreateVertexShader ( dwDecl, (DWORD*)pCode->GetBufferPointer(), &m_dwEffReflect, 
												DxEffectMan::Instance.GetUseSwShader() );
	if( FAILED(hr) )	return E_FAIL;

	SAFE_RELEASE ( pCode );

	return S_OK;
}

HRESULT DxEffectEnviron2::ReleaseDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	pd3dDevice->DeleteVertexShader ( m_dwEffReflect );

	return S_OK;
}

HRESULT DxEffectEnviron2::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICE8 pd3dDevice )
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

	return S_OK;
}


HRESULT DxEffectEnviron2::InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	//	Note : 텍스쳐의 읽기 오류는 무시한다.
	//
	TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pCloudTexture, 0, 0 );

	if ( !m_pCloudTexture )
	{
		char szMsg[256];
		sprintf ( szMsg, "이미지 파일 '%s' 읽기에 실패", m_szTexture );
		MessageBox ( NULL, szMsg, "ERROR", MB_OK );
	}

	//	Note : 텍스쳐의 읽기 오류는 무시한다.
	//
	TextureManager::LoadTexture ( m_szTexDot3, pd3dDevice, m_pDot3Texture, 0, 0 );

	if ( !m_pDot3Texture )
	{
		char szMsg[256];
		sprintf ( szMsg, "이미지 파일 '%s' 읽기에 실패", m_szTexDot3 );
		MessageBox ( NULL, szMsg, "ERROR", MB_OK );
	}

	//	Note : 텍스쳐의 읽기 오류는 무시한다.
	//
	TextureManager::LoadTexture ( m_szTexDetail, pd3dDevice, m_pDetailTexture, 0, 0 );

	if ( !m_pDetailTexture )
	{
		char szMsg[256];
		sprintf ( szMsg, "이미지 파일 '%s' 읽기에 실패", m_szTexDetail );
		MessageBox ( NULL, szMsg, "ERROR", MB_OK );
	}



	//	Note : 
	//
	// Note	: Shader 에서 쓰이는 Cube Texture 만들기
	//
	TCHAR* szSkyTex[6] =
	{
		m_szTexture1, m_szTexture2, m_szTexture3,
		m_szTexture4, m_szTexture5, m_szTexture6
	};

	LPDIRECT3DTEXTURE8	m_pSkyTex[6];

	//	Note : 
	//
	for(UINT i = 0; i < 6; i++)
	{
		TextureManager::LoadTexture ( szSkyTex[i], pd3dDevice, m_pSkyTex[i], 0, 0 );
	}

	if(SUCCEEDED(D3DXCreateCubeTexture(pd3dDevice, 128, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, &m_pCubeTexture)))
	{
		for(UINT i = 0; i < 6; i++)
		{
			if(m_pSkyTex[i])
			{
				IDirect3DSurface8 *pSrc;
				IDirect3DSurface8 *pDest;

				m_pSkyTex[i]->GetSurfaceLevel(0, &pSrc);
				m_pCubeTexture->GetCubeMapSurface((D3DCUBEMAP_FACES) i, 0, &pDest);

				if(pSrc && pDest)
					D3DXLoadSurfaceFromSurface(pDest, NULL, NULL, pSrc, NULL, NULL, D3DX_DEFAULT, 0);

				SAFE_RELEASE(pDest);
				SAFE_RELEASE(pSrc);

				//	Note : 지워 준다.
				//
				TextureManager::ReleaseTexture ( m_pSkyTex[i] );
				m_pSkyTex[i] = NULL;
			}
		}

		D3DXFilterCubeTexture( m_pCubeTexture, NULL, 0, D3DX_DEFAULT);
	}

	return S_OK;
}

HRESULT DxEffectEnviron2::DeleteDeviceObjects ()
{
	//	Note : 이전 텍스쳐가 로드되어 있을 경우 제거.
	//
	if ( m_pCloudTexture )
		TextureManager::ReleaseTexture ( m_pCloudTexture );
	m_pCloudTexture = NULL;

	//	Note : 이전 텍스쳐가 로드되어 있을 경우 제거.
	//
	if ( m_pDot3Texture )
		TextureManager::ReleaseTexture ( m_pDot3Texture );
	m_pDot3Texture = NULL;

	//	Note : 이전 텍스쳐가 로드되어 있을 경우 제거.
	//
	if ( m_pDetailTexture )
		TextureManager::ReleaseTexture ( m_pDetailTexture );
	m_pDetailTexture = NULL;

	//	Note : 이전 텍스쳐가 로드되어 있을 경우 제거.
	//
	if ( m_pCubeTexture )
		m_pCubeTexture->Release ();
	m_pCubeTexture = NULL;

	return S_OK;
}

HRESULT DxEffectEnviron2::FrameMove ( float fTime, float fElapsedTime )
{
	m_fTime += fElapsedTime;

	return S_OK;
}

HRESULT DxEffectEnviron2::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICE8 pd3dDevice )
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
			if ( pmsMeshs->m_pLocalMesh )
			{
				DWORD dwFVFSize;
				DWORD dwVerts, dwFaces;
				LPDIRECT3DINDEXBUFFER8 pIB;
				LPDIRECT3DVERTEXBUFFER8 pVB;
				
				pmsMeshs->m_pLocalMesh->GetIndexBuffer ( &pIB );
				pmsMeshs->m_pLocalMesh->GetVertexBuffer ( &pVB );
				
				dwFVFSize = pmsMeshs->m_pLocalMesh->GetFVF ();
				dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );

				dwVerts = pmsMeshs->m_pLocalMesh->GetNumVertices();
				dwFaces = pmsMeshs->m_pLocalMesh->GetNumFaces();


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



				//	Note : SetRenderState() 선언
				//
				pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0xffffffff );//m_dwBlendAlpha );	// 알파값... ㅋㅋㅋ환경맵 알파.


				//	Note : SetTextureStageState() 선언
				//
				pd3dDevice->SetTextureStageState ( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
				pd3dDevice->SetTextureStageState ( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
				pd3dDevice->SetTextureStageState ( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );				

				pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		//		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
				pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_BLENDFACTORALPHA );

				



				switch ( m_wEnvCase )
				{
				case 0x0001:	// 내적 블렌딩
					pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
					pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,	D3DBLEND_ZERO );
					pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,	D3DBLEND_SRCCOLOR );

					pd3dDevice->SetTexture ( 0, m_pDot3Texture );
			//		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
					pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_DOTPRODUCT3 );

					break;

				case 0x0002:	// 디테일 맵

					pd3dDevice->SetTexture ( 0, m_pDetailTexture );

					pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
					pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,	D3DBLEND_DESTCOLOR );
					pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,	D3DBLEND_SRCCOLOR );

			//		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

					break;

				case 0x0003:	// 반사
					pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
					pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,	D3DBLEND_DESTCOLOR );
					pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,	D3DBLEND_SRCCOLOR );
//
					pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, m_dwBlendAlpha );
					
		//			pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

					pd3dDevice->SetTexture ( 0, m_pCubeTexture );

					break;

				case 0x0010:	// 구름 그림자

					pd3dDevice->SetTexture ( 0, m_pCloudTexture );

			//		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

					if( m_fTime > 0.05f )
					{
						m_fTime = 0.0f;

						m_fLoop += m_fLoopAdd;
						if( m_fLoop > 1.0f )
							m_fLoop = 0.0f;

						struct VERTEX { D3DXVECTOR3 p,n;  D3DXVECTOR2 t; };
						VERTEX* pVertices;

						D3DXVECTOR4	vLightDirect;
						pd3dDevice->GetVertexShaderConstant ( VSC_LIGHTDIRECT01, &vLightDirect, 1 );

						D3DXVECTOR4	vCameraPos;
						pd3dDevice->GetVertexShaderConstant ( VSC_CAMERAPOSITION, &vCameraPos, 1 );



						pmsMeshs->m_pLocalMesh->LockVertexBuffer ( 0L, (BYTE**)&pVertices );

						for( DWORD j=0; j<dwVerts; j++ )
						{
							float X = pVertices->p.x * pframeCur->matCombined._11 + pVertices->p.y * pframeCur->matCombined._21 +
										pVertices->p.z * pframeCur->matCombined._31 + pframeCur->matCombined._41;

							float Y = pVertices->p.x * pframeCur->matCombined._12 + pVertices->p.y * pframeCur->matCombined._22 +
										pVertices->p.z * pframeCur->matCombined._32 + pframeCur->matCombined._42;

							float Z = pVertices->p.x * pframeCur->matCombined._13 + pVertices->p.y * pframeCur->matCombined._23 +
										pVertices->p.z * pframeCur->matCombined._33 + pframeCur->matCombined._43;

							//	Note : 너비가 (m_fDistance) 인 구름을 만든 다음
							//			U,V 값을 (m_fLoop)움직여서 땅에 그림자가 드리우는 효과를 낸다.
							//		 위치		높이와 빛 각도 땜시.			전체크기반		전체크기	움직인다
							float u = (X + (vLightDirect.x/vLightDirect.y)*Y - m_fDistance/2) / m_fDistance - m_fLoop;// + DistX/1000;
							float v = (Z + (vLightDirect.z/vLightDirect.y)*Y - m_fDistance/2) / m_fDistance - m_fLoop;// + DistZ/1000;

							pVertices->t.x = u*m_fMultiply;
							pVertices->t.y = v*m_fMultiply;

							pVertices++;
						}
						pmsMeshs->m_pLocalMesh->UnlockVertexBuffer ();
					}

					pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
					pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,	D3DBLEND_DESTCOLOR );
					pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,	D3DBLEND_SRCCOLOR );

					break;

				case 0x0011:	// 구름 그림자 + 내적 블렌딩

					pd3dDevice->SetTexture ( 0, m_pDot3Texture );
					pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
					pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_DOTPRODUCT3 );

					break;

				case 0x0012:	// 구름 그림자 + 

					break;

				case 0x0013:	// 구름 그림자 + 반사

					if( m_fTime > 0.05f )
					{
						m_fTime = 0.0f;

						m_fLoop += m_fLoopAdd;
						if( m_fLoop > 1.0f )
							m_fLoop = 0.0f;
					}
					//	Note : Shader 상수 선언
					//
					pd3dDevice->SetVertexShaderConstant ( VSC_CUSTOM+5,	&D3DXVECTOR4( m_fDistance , m_fLoop, m_fMultiply, 0.0f ),	1 );



					pd3dDevice->SetTexture ( 0, m_pCloudTexture );

					pd3dDevice->SetTextureStageState ( 0, D3DTSS_ADDRESSU, D3DTADDRESS_MIRROR );
					pd3dDevice->SetTextureStageState ( 0, D3DTSS_ADDRESSV, D3DTADDRESS_MIRROR );

					pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

					//	Note : SetRenderState() 선언
					//
					pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, m_dwBlendAlpha );	// 알파값... ㅋㅋㅋ환경맵 알파.

					pd3dDevice->SetTexture ( 1, m_pCubeTexture );

					pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_BLENDFACTORALPHA );

					pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
					pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,	D3DBLEND_ZERO );
					pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,	D3DBLEND_SRCCOLOR );
					//pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,	D3DBLEND_ONE );

				//	pd3dDevice->SetVertexShader ( m_dwEffCloudReflect );
					pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, dwVerts, 0, dwFaces );

					pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );
					
					break;

				default:
					return S_OK;

					break;

				}






				DWORD					AttribTableSize = 0;
				LPD3DXATTRIBUTERANGE	pSrcAttribTable = NULL;

				pmsMeshs->m_pLocalMesh->GetAttributeTable ( NULL, &AttribTableSize );
				
				LPD3DXATTRIBUTERANGE pAttribTable;
				pAttribTable = new D3DXATTRIBUTERANGE [ AttribTableSize ];
				pmsMeshs->m_pLocalMesh->GetAttributeTable( pAttribTable, &AttribTableSize );

				//	Note : 불투명 메쉬 드로잉.
				//		pmsMeshs->cMaterials --> AttribTableSize
				//
				for ( DWORD i=0; i<AttribTableSize; i++ )
				{












				pd3dDevice->SetVertexShaderConstant ( VSC_MATRIAL, &( pmsMeshs->rgMaterials[0].Diffuse ), 1 );


				//	Note : 소프트웨어 버텍스 프로세싱
				//
				if ( DxEffectMan::Instance.GetUseSwShader() )
					pd3dDevice->SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, TRUE );		

				pd3dDevice->SetIndices ( pIB, 0 );
				pd3dDevice->SetStreamSource ( 0, pVB, dwFVFSize );

				switch ( m_wEnvCase )
				{
				case 0x0003:	// 반사
					pd3dDevice->SetVertexShader ( m_dwEffReflect );
					break;
				case 0x0013:	// 구름 그림자 + 반사
		//			pd3dDevice->SetVertexShader ( m_dwEffCloudReflect );
					break;
				default:
					pd3dDevice->SetTransform ( D3DTS_WORLD, &(pframeCur->matCombined) );
					pd3dDevice->SetVertexShader ( pmsMeshs->m_pLocalMesh->GetFVF () );
					break;
				}

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








				}

				SAFE_DELETE(pAttribTable);






				
				pd3dDevice->SetTexture(0, NULL);
				pd3dDevice->SetTexture(1, NULL);

				pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );

				pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
				pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

				pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
				pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_DISABLE );

				pd3dDevice->SetTextureStageState ( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
				pd3dDevice->SetTextureStageState ( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

				pd3dDevice->SetTextureStageState ( 1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
				pd3dDevice->SetTextureStageState ( 1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

				pd3dDevice->SetTextureStageState ( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
				pd3dDevice->SetTextureStageState ( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
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

#include "stdafx.h"

#include <string>
#include "SubPath.h"
#include "Dxviewport.h"
#include "dxincommand.h"
#include "DxInputDevice.h"
#include "DxEffectMan.h"
#include "dxfontman.h"
#include "Cursor.h"

#include "DxSurfaceTex.h"
#include "DxGlowMan.h"

#include "GLPeriod.h"
#include "DxResponseMan.h"
#include "DxGlobalStage.h"
#include "DxShadowMap.h"
#include "dxparamset.h"
#include "RANPARAM.h"
#include "DxGlobalStage.h"

#include "Basic.h"
#include "BasicWnd.h"
#include "DlgSetting.h"

#include "LoadingThread.h"

#include "DxMeshTexMan.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


HRESULT CBasicWnd::MainLoadCreateObject()
{
	CBasicApp *pFrame = (CBasicApp *) AfxGetApp();

	m_strTEXTURE = "Loading.tga";

	HRESULT hr = S_OK;
	hr = TextureManager::LoadTexture ( m_strTEXTURE.c_str(), m_pd3dDevice, m_pLoadingTexture, 0, 0 );
	if ( FAILED ( hr ) )	return hr;

	RECT rect;
	GetWindowRect ( &rect );

	const float fWidth = float(rect.right - rect.left);
	const float fHeight = float(rect.bottom - rect.top);

	const float	fRealImageX = 800.0f;
	const float	fRealImageY = 600.0f;
	const float	fImageSize = 1023.0f;

	const float fWidthRatio = fRealImageX / fImageSize;
	const float fHeightRatio= fRealImageY / fImageSize;

	const float LeftPos  = (fWidth - fRealImageX) / 2;
	const float TopPos   = (fHeight- fRealImageY) / 2;
	const float RightPos = (LeftPos+ fRealImageX);
	const float BottomPos= (TopPos + fRealImageY);

	const D3DXVECTOR2 TexturePos[9] = 
	{
		D3DXVECTOR2(0, 607), D3DXVECTOR2(0, 674), D3DXVECTOR2(0, 768),
		D3DXVECTOR2(0, 819), D3DXVECTOR2(0, 865), D3DXVECTOR2(0, 939),
		D3DXVECTOR2(261, 603), D3DXVECTOR2(261, 715), D3DXVECTOR2(261, 836)
	};

	const D3DXVECTOR2 TextureSize[9] = 
	{
		D3DXVECTOR2(46, 61), D3DXVECTOR2(61, 88), D3DXVECTOR2(45, 46),
		D3DXVECTOR2(32, 41), D3DXVECTOR2(53, 66), D3DXVECTOR2(60, 73),
		D3DXVECTOR2(71, 108), D3DXVECTOR2(113, 117), D3DXVECTOR2(51, 53)
	};

	const D3DXVECTOR2 TextureRenderPos[9] = 
	{
		D3DXVECTOR2(198, 497), D3DXVECTOR2(264, 481), D3DXVECTOR2(334, 470),
		D3DXVECTOR2(386, 463), D3DXVECTOR2(410, 436), D3DXVECTOR2(467, 398),
		D3DXVECTOR2(527, 419), D3DXVECTOR2(613, 410), D3DXVECTOR2(743, 393)
	};

	const TEXTUREVERTEX Vertices[6] =
	{
		TEXTUREVERTEX ( LeftPos,  TopPos, 0.0f, 1.0f,	0xFFFFFFFF, 0.0f, 0.0f ), // x, y, z, rhw, color, tu, tv
		TEXTUREVERTEX ( RightPos, TopPos, 0.0f, 1.0f,	0xFFFFFFFF, fWidthRatio, 0.0f ),
		TEXTUREVERTEX ( LeftPos, BottomPos, 0.0f, 1.0f,	0xFFFFFFFF, 0.0f, fHeightRatio ),

		TEXTUREVERTEX ( LeftPos, BottomPos, 0.0f, 1.0f,	0xFFFFFFFF, 0.0f, fHeightRatio ),
		TEXTUREVERTEX ( RightPos, TopPos, 0.0f, 1.0f,	0xFFFFFFFF, fWidthRatio, 0.0f ),
		TEXTUREVERTEX ( RightPos, BottomPos, 0.0f, 1.0f,0xFFFFFFFF, fWidthRatio, fHeightRatio )
	};

	if( FAILED( m_pd3dDevice->CreateVertexBuffer( 6*sizeof(TEXTUREVERTEX),
		0, TEXTUREVERTEX::FVF, D3DPOOL_MANAGED, &m_pTextureVB ) ) )
	{
		return E_FAIL;
	}

	VOID* pVertices;
	if( FAILED( m_pTextureVB->Lock( 0, sizeof(Vertices), (BYTE**)&pVertices, 0 ) ) )
	{
		return E_FAIL;
	}

	memcpy( pVertices, Vertices, sizeof(Vertices) );
	m_pTextureVB->Unlock();
	
	for ( int i = 0; i < 9; i++ )
	{
		LPDIRECT3DVERTEXBUFFER8 &rpTexturePart = m_pTexturePartVB[i];
		TEXTUREVERTEX VerticesPart[6] = 
		{
			TEXTUREVERTEX ( LeftPos + TextureRenderPos[i].x, TopPos + TextureRenderPos[i].y, 0.0f, 1.0f, 0xFFFFFFFF, TexturePos[i].x / fImageSize, TexturePos[i].y / fImageSize ), // x, y, z, rhw, color, tu, tv
			TEXTUREVERTEX ( LeftPos + TextureRenderPos[i].x + TextureSize[i].x, TopPos + TextureRenderPos[i].y, 0.0f, 1.0f,	0xFFFFFFFF, (TexturePos[i].x + TextureSize[i].x) / fImageSize, TexturePos[i].y / fImageSize ),
			TEXTUREVERTEX ( LeftPos + TextureRenderPos[i].x, TopPos + TextureRenderPos[i].y + TextureSize[i].y, 0.0f, 1.0f,	0xFFFFFFFF, TexturePos[i].x / fImageSize, (TexturePos[i].y + TextureSize[i].y) / fImageSize ),

			TEXTUREVERTEX ( LeftPos + TextureRenderPos[i].x, TopPos + TextureRenderPos[i].y + TextureSize[i].y, 0.0f, 1.0f,	0xFFFFFFFF, TexturePos[i].x / fImageSize, (TexturePos[i].y + TextureSize[i].y) / fImageSize ),
			TEXTUREVERTEX ( LeftPos + TextureRenderPos[i].x + TextureSize[i].x, TopPos + TextureRenderPos[i].y, 0.0f, 1.0f,	0xFFFFFFFF, (TexturePos[i].x + TextureSize[i].x) / fImageSize, TexturePos[i].y / fImageSize ),
			TEXTUREVERTEX ( LeftPos + TextureRenderPos[i].x + TextureSize[i].x, TopPos + TextureRenderPos[i].y + TextureSize[i].y, 0.0f, 1.0f,0xFFFFFFFF, (TexturePos[i].x + TextureSize[i].x) / fImageSize, (TexturePos[i].y + TextureSize[i].y) / fImageSize )
		};

		if( FAILED( m_pd3dDevice->CreateVertexBuffer( 6*sizeof(TEXTUREVERTEX),
													0, TEXTUREVERTEX::FVF,
													D3DPOOL_MANAGED, &rpTexturePart ) ) )
		{
			return E_FAIL;
		}

		VOID* pVertices;
		if( FAILED( rpTexturePart->Lock( 0, sizeof(Vertices), (BYTE**)&pVertices, 0 ) ) )
		{
			return E_FAIL;
		}
		memcpy( pVertices, VerticesPart, sizeof(VerticesPart) );
		rpTexturePart->Unlock();
	}

	return S_OK;
}

HRESULT CBasicWnd::MainLoadDeleteObjects()
{
	SAFE_RELEASE ( m_pTextureVB );

	for ( int i = 0; i < 9; i++ )
	{
		SAFE_RELEASE ( m_pTexturePartVB[i] );
	}

	TextureManager::ReleaseTexture( m_strTEXTURE.c_str(), m_pLoadingTexture );
	
	return S_OK;
}

HRESULT CBasicWnd::MainLoadRender()
{
	HRESULT hr(S_OK);

	SleepEx ( 33, FALSE );

	hr = m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );

	// Begin the scene
	if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
	{
		m_pd3dDevice->SetTexture ( 0, m_pLoadingTexture );

		hr = m_pd3dDevice->SetStreamSource( 0, m_pTextureVB, sizeof(TEXTUREVERTEX) );
		hr = m_pd3dDevice->SetVertexShader( TEXTUREVERTEX::FVF );
		hr = m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );

		m_pd3dDevice->SetTexture ( 0, NULL );


		/////////////////////////////////////////////////////////////
		//	좋지 않은 모습의 코드
		//	아이디어가 없다. 만들고 좋은 생각나면 수정하자.
		int pri_Step = LOADINGSTEP::GETSTEP ();

		/////////////////////////////////////////////////////////////

		for ( int i = 0; i < pri_Step; i++ )
		{
			m_pd3dDevice->SetTexture ( 0, m_pLoadingTexture );

			hr = m_pd3dDevice->SetStreamSource( 0, m_pTexturePartVB[i], sizeof(TEXTUREVERTEX) );
			hr = m_pd3dDevice->SetVertexShader( TEXTUREVERTEX::FVF );
			hr = m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );

			m_pd3dDevice->SetTexture ( 0, NULL );
		}

		// End the scene.
		m_pd3dDevice->EndScene();
	}

	return S_OK;
}
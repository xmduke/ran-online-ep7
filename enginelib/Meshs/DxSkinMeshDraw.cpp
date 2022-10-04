#include "stdafx.h"
#include "DxRenderStates.h"

#include "ShaderConstant.h"
#include "DxViewPort.h"
#include "DxLightMan.h"

#include "DxSkinPieceContainer.h"
#include "DxSkinMesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//	Note : FrameMove - 시간 경과를 적용한다.
//
HRESULT DxSkinMesh::FrameMove ( float fElapsedTime )
{
	return S_OK;
}

HRESULT DxSkinMesh::Render ( D3DXMATRIX matRot, LPDIRECT3DDEVICE8 pd3dDevice )
{
	if ( !pframeRoot ) return S_OK;

	// Note : 랜더 지점 설정.
	//
	pframeRoot->matRot = matRot;
    
	UINT cTriangles = 0;
	HRESULT hr;
	D3DXMATRIXA16 mCur;
	D3DXVECTOR3 vTemp;

	D3DXMatrixIdentity ( &mCur );

	//	Note : 메트릭스 계산.
	//
	hr = UpdateFrames ( pframeRoot, mCur );
	if (FAILED(hr))		return hr;

	//	Note : 메쉬 그리기 수행.
	//
	hr = DrawFrames ( pd3dDevice, pframeRoot, cTriangles );
	if (FAILED(hr))		return hr;

	return S_OK;
}

void DxSkinMesh::SetDrawState ( BOOL bOriginDraw, BOOL bAlpha, BOOL bShadow, BOOL bAlphaTex, BOOL bLevel, float fHeight )
{
	m_bOriginDraw	= bOriginDraw;
	m_bAlpha		= bAlpha;
	m_bShadow		= bShadow;
	m_bAlphaTex		= bAlphaTex;
	m_bLevel		= bLevel;
	m_fHeight		= fHeight;
}

HRESULT DxSkinMesh::DrawMeshContainer ( LPDIRECT3DDEVICE8 pd3dDevice, SMeshContainer *pmcMesh,
									   SMATERIAL_PIECE *pmtrlPiece, DXMATERIAL_CHAR_EFF* pmtrlSpecular )
{
	UINT ipattr;
	HRESULT hr = S_OK;
	LPD3DXBONECOMBINATION pBoneComb;
	DWORD AttribIdPrev;

	if ( !pmcMesh->m_pSkinMesh )	goto _RETURN;

	if ( m_method != pmcMesh->m_Method )
	{
		pmcMesh->GenerateMesh (m_method );
    }

	if ( pmcMesh->m_Method == D3DNONINDEXED )
	{
		AttribIdPrev = UNUSED32; 
		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pmcMesh->m_pBoneCombinationBuf->GetBufferPointer());
		
		// Draw using default vtx processing of the device (typically HW)
		for ( ipattr = 0; ipattr < pmcMesh->m_cpattr; ipattr++ )
		{
			DWORD numBlend = 0;
			for ( DWORD i = 0; i < pmcMesh->m_maxFaceInfl; ++i )
            {
				if ( pBoneComb[ipattr].BoneId[i] != UINT_MAX )
				{
					numBlend = i;
				}
			}

			if ( m_dwD3dMaxVertexBlendMatrices >= numBlend + 1 )
            {
				D3DXMATRIX	matBoneMatrix_UP, matTemp;
				for ( DWORD i = 0; i < pmcMesh->m_maxFaceInfl; ++i )
				{
					DWORD matid = pBoneComb[ipattr].BoneId[i];
					if ( matid != UINT_MAX )
					{
						matBoneMatrix_UP = *pmcMesh->m_pBoneMatrix[matid];
						matBoneMatrix_UP._42 += m_fHeight;

						D3DXMatrixMultiply_MM( matTemp, pmcMesh->m_pBoneOffsetMat[matid], matBoneMatrix_UP );
						pd3dDevice->SetTransform ( D3DTS_WORLDMATRIX(i), &matTemp );
					}
				}

				
				pd3dDevice->SetRenderState ( D3DRS_VERTEXBLEND, numBlend );
				DWORD dwThisAttribId = pBoneComb[ipattr].AttribId;

				if ( (AttribIdPrev != dwThisAttribId) || (AttribIdPrev == UNUSED32) )
				{
					pmcMesh->SetMaterial ( pd3dDevice, dwThisAttribId, pmtrlPiece, pmtrlSpecular );
					AttribIdPrev = dwThisAttribId;
				}

				//	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
				//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
				//
				BOOL bDraw = m_bOriginDraw || ( pmtrlSpecular && pmtrlSpecular[dwThisAttribId].bEffUse );
				if ( bDraw )
				{
					RenderDefault ( pd3dDevice, pmcMesh, ipattr, dwThisAttribId, pmtrlPiece, pmtrlSpecular );
				}
			}
		}

		// If necessary, draw parts that HW could not handle using SW
		if ( pmcMesh->m_iAttrSplit < pmcMesh->m_cpattr )
		{
			AttribIdPrev = UNUSED32;
			pd3dDevice->SetRenderState ( D3DRS_SOFTWAREVERTEXPROCESSING, TRUE );
			for ( ipattr = pmcMesh->m_iAttrSplit; ipattr < pmcMesh->m_cpattr; ipattr++ )
			{
				DWORD numBlend = 0;
				for ( DWORD i = 0; i < pmcMesh->m_maxFaceInfl; ++i )
				{
					if ( pBoneComb[ipattr].BoneId[i] != UINT_MAX )
					{
						numBlend = i;
					}
				}

				if ( m_dwD3dMaxVertexBlendMatrices < numBlend + 1 )
				{
					D3DXMATRIX	matBoneMatrix_UP, matTemp;

					for ( DWORD i = 0; i < pmcMesh->m_maxFaceInfl; ++i )
					{
						DWORD matid = pBoneComb[ipattr].BoneId[i];
						if ( matid != UINT_MAX )
						{
							matBoneMatrix_UP = *pmcMesh->m_pBoneMatrix[matid];
							matBoneMatrix_UP._42 += m_fHeight;

							D3DXMatrixMultiply_MM( matTemp, pmcMesh->m_pBoneOffsetMat[matid], matBoneMatrix_UP );
							pd3dDevice->SetTransform ( D3DTS_WORLDMATRIX(i), &matTemp );
						}
					}

					pd3dDevice->SetRenderState ( D3DRS_VERTEXBLEND, numBlend );
					DWORD dwThisAttribId = pBoneComb[ipattr].AttribId;
					if ( (AttribIdPrev!=dwThisAttribId) || (AttribIdPrev==UNUSED32) )
					{
						pmcMesh->SetMaterial ( pd3dDevice, dwThisAttribId, pmtrlPiece, pmtrlSpecular );
						AttribIdPrev = pBoneComb[ipattr].AttribId;
					}

					//	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
					//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
					//
					BOOL bDraw = m_bOriginDraw || ( pmtrlSpecular && pmtrlSpecular[dwThisAttribId].bEffUse );
					if ( bDraw )
					{
						RenderDefault ( pd3dDevice, pmcMesh, ipattr, dwThisAttribId, pmtrlPiece, pmtrlSpecular );
					}
				}
			}
			pd3dDevice->SetRenderState ( D3DRS_SOFTWAREVERTEXPROCESSING, FALSE );
		}
		pd3dDevice->SetRenderState ( D3DRS_VERTEXBLEND, 0 );
	}
	else if (m_method == D3DINDEXEDVS)
    {
		D3DLIGHT8	pLight;
		D3DXVECTOR4 vPointPos;
		D3DXVECTOR3 vPointPos3;
		D3DXVECTOR3 vPointDiff;

		D3DXVECTOR3	vHalfVector;
		D3DXVECTOR3	vLightVector;
		D3DXVECTOR3 vDiffuse;
		D3DXVECTOR3	vAmbient;

		D3DXVECTOR3	vFromPt		= DxViewPort::GetInstance().GetFromPt();
		D3DXVECTOR3	vDir		= DxViewPort::GetInstance().GetLookDir();

		vHalfVector		= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction;
		vHalfVector		= vDir;

		vLightVector	= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction;
		vLightVector	= vLightVector;

		vDiffuse.x		= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Diffuse.r;
		vDiffuse.y		= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Diffuse.g;
		vDiffuse.z		= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Diffuse.b;

		vAmbient.x		= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Ambient.r + 50.f*DIV_1_255;
		vAmbient.y		= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Ambient.g + 50.f*DIV_1_255;
		vAmbient.z		= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Ambient.b + 50.f*DIV_1_255;


		// Use COLOR instead of UBYTE4 since Geforce3 does not support it
		// vConst.w should be 3, but due to about hack, mul by 255 and add epsilon
		D3DXVECTOR4 vConst( 1.0f, 0.0f, 0.5f, 765.01f );
		D3DXMATRIX	matIdentity, matWordl, matView, matProj, matVP;

		matView = DxViewPort::GetInstance().GetMatView();
		matProj = DxViewPort::GetInstance().GetMatProj();

		//D3DXVec3TransformNormal ( &vLightVector, &vLightVector, &matView );
		D3DXVec3Normalize ( &vLightVector, &vLightVector);
		//vLightVector = -vLightVector;

		//D3DXVec3TransformCoord ( &vFromPt, &vFromPt, &matView );
		D3DXVec3Normalize ( &vFromPt, &vFromPt);
		

		m_pd3dDevice->SetVertexShaderConstant(0, &vConst, 1);
		m_pd3dDevice->SetVertexShaderConstant(1, &vLightVector, 1);

		//m_pd3dDevice->SetVertexShaderConstant(1, &vDir, 1);		// 테스트 용이다.
		
		//D3DXMatrixTranspose( &matVP, &matProj);
		D3DXMatrixMultiplyTranspose ( &matVP, &matView, &matProj );
		m_pd3dDevice->SetVertexShaderConstant(2, &matVP, 4);
		m_pd3dDevice->SetVertexShaderConstant(6, &vFromPt, 1);
		m_pd3dDevice->SetVertexShaderConstant(7, &vAmbient, 1);
		m_pd3dDevice->SetVertexShaderConstant(8, &vDiffuse, 1);
		m_pd3dDevice->SetVertexShaderConstant(9, &D3DXVECTOR4(0.25f, 0.5f, 1.f, -1.f), 1);

		//	Note : 스펙큘러 카메라에 고정 위해서
		//
		m_pd3dDevice->SetPixelShaderConstant ( 3, &D3DXVECTOR4(0.25f, 0.5f, 1.f, -1.f), 1 );
		//vLightVector.x = 0.f;
		//vLightVector.y = 0.f;
		//vLightVector.z = 1.f;
		vLightVector = -vLightVector;
		D3DXVec3Normalize ( &vLightVector, &vLightVector);
		m_pd3dDevice->SetPixelShaderConstant ( 4, &vLightVector, 1 );


		for ( int i=1; i<8; ++i )
		{
			if ( DxLightMan::GetInstance()->GetClosedLight(i) )
			{
				pLight = DxLightMan::GetInstance()->GetClosedLight(i)->m_Light;
				vPointPos3 = D3DXVECTOR3 ( pLight.Position.x, pLight.Position.y, pLight.Position.z );
				vPointDiff = D3DXVECTOR3 ( pLight.Diffuse.r, pLight.Diffuse.g, pLight.Diffuse.b );
				D3DXVec3TransformCoord ( &vPointPos3, &vPointPos3, &matView );
				vPointPos.x = vPointPos3.x;
				vPointPos.y = vPointPos3.y;
				vPointPos.z = vPointPos3.z;
				vPointPos.w = pLight.Range;
			}
			else
			{
				vPointPos = D3DXVECTOR4 ( 0.f, 0.f, 0.f, 0.1f );
				vPointDiff = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
			}
			m_pd3dDevice->SetVertexShaderConstant(i*2+8, &vPointPos, 1);
			m_pd3dDevice->SetVertexShaderConstant(i*2+9, &vPointDiff, 1);
		}

		LPDIRECT3DVERTEXBUFFER8 pVB;
        LPDIRECT3DINDEXBUFFER8 pIB;

        if (pmcMesh->m_bUseSW)
        {
            m_pd3dDevice->SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, TRUE);
        }

        pmcMesh->m_pMesh->GetVertexBuffer(&pVB);
        pmcMesh->m_pMesh->GetIndexBuffer(&pIB);

        hr = m_pd3dDevice->SetStreamSource(0, pVB, D3DXGetFVFVertexSize(pmcMesh->m_pMesh->GetFVF()));
        if(FAILED(hr))		return hr;
        hr = m_pd3dDevice->SetIndices(pIB, 0);
        if(FAILED(hr))		return hr;

        pVB->Release();
        pIB->Release();

		D3DXMATRIX	matBoneMatrix_UP;

		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pmcMesh->m_pBoneCombinationBuf->GetBufferPointer());
		for (ipattr = 0; ipattr < pmcMesh->m_cpattr; ipattr++)
		{
			for (DWORD i = 0; i < pmcMesh->m_paletteSize; ++i)
			{
				DWORD matid = pBoneComb[ipattr].BoneId[i];
				if (matid != UINT_MAX)
				{
					matBoneMatrix_UP = *pmcMesh->m_pBoneMatrix[matid];
					matBoneMatrix_UP._42 += m_fHeight;

					D3DXMATRIXA16 mat;
					D3DXMatrixMultiplyTranspose ( &mat, &pmcMesh->m_pBoneOffsetMat[matid], &matBoneMatrix_UP );
					//D3DXMatrixMultiply ( &mat, &pmcMesh->m_pBoneOffsetMat[matid], &matBoneMatrix_UP );
					//D3DXMatrixMultiplyTranspose ( &mat, &mat, &matView );
					m_pd3dDevice->SetVertexShaderConstant(i*3 + 24, &mat, 3);
				}
			}

			DWORD dwThisAttribId = pBoneComb[ipattr].AttribId;

			//	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
			//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
			//
			BOOL bDraw = m_bOriginDraw || ( pmtrlSpecular && pmtrlSpecular[dwThisAttribId].bEffUse );
			if ( bDraw )
			{
				RenderVS ( pd3dDevice, pmcMesh, pBoneComb, ipattr, dwThisAttribId, pmtrlPiece, pmtrlSpecular );
			}

		//	if ( bShadow )
		//	{
		//		m_pd3dDevice->GetTransform ( D3DTS_VIEW,		&matView );
		//		m_pd3dDevice->GetTransform ( D3DTS_PROJECTION,	&matProj );

		////		D3DXMatrixMultiply ( &matVP, &matView, &matProj );
		////		D3DXMatrixTranspose( &matVP, &matProj);
		////		m_pd3dDevice->SetVertexShaderConstant(2, &matVP, 4);

		////		RenderDefaultVS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr );
		//	}
		/*	else if ( dwThisAttribId == 0 )
			{
				switch ( dwCharDetail_1 )
				{
				case 4:
					RenderBumpDOT3VS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr, pTex4 );
					RenderBumpReflectVS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr, pTex4, pTex1 );
					break;
				case 3:
					RenderBumpDOT3VS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr, pTex4 );
					RenderBumpSpecularVS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr, pTex4, pTex1 );
					break;
				case 2:
					RenderReflectVS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr, pTex1 );
					break;
				case 1:
					RenderSpecularVS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr, pTex1 );
					break;
				default:
					RenderDefaultVS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr );
					break;
				}
			}
			else if ( dwThisAttribId == 1 )
			{
				switch ( dwCharDetail_2 )
				{
				case 4:
					RenderBumpDOT3VS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr, pTex5 );
					RenderBumpReflectVS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr, pTex5, pTex2 );
					break;
				case 3:
					RenderBumpDOT3VS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr, pTex5 );
					RenderBumpSpecularVS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr, pTex5, pTex2 );
					break;
				case 2:
					RenderReflectVS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr, pTex2 );
					break;
				case 1:
					RenderSpecularVS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr, pTex2 );
					break;
				default:
					RenderDefaultVS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr );
					break;
				}
			}
			else if ( dwThisAttribId == 2 )
			{
				switch ( dwCharDetail_3 )
				{
				case 4:
					RenderBumpDOT3VS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr, pTex6 );
					RenderBumpReflectVS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr, pTex6, pTex3 );
					break;
				case 3:
					RenderBumpDOT3VS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr, pTex6 );
					RenderBumpSpecularVS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr, pTex6, pTex3 );
					break;
				case 2:
					RenderReflectVS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr, pTex3 );
					break;
				case 1:
					RenderSpecularVS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr, pTex3 );
					break;
				default:
					RenderDefaultVS ( pd3dDevice, pmcMesh, pBoneComb, bShadow, bSpecular, ipattr );
					break;
				}
			}*/
		}
		
        if (pmcMesh->m_bUseSW)
        {
            m_pd3dDevice->SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, FALSE);
        }

    }
	else if ( pmcMesh->m_Method == D3DINDEXED )
	{
		if ( pmcMesh->m_bUseSW )
		{
			pd3dDevice->SetRenderState ( D3DRS_SOFTWAREVERTEXPROCESSING, TRUE );
		}

		if ( pmcMesh->m_maxFaceInfl == 1 )
			pd3dDevice->SetRenderState ( D3DRS_VERTEXBLEND, D3DVBF_0WEIGHTS );
		else
			pd3dDevice->SetRenderState ( D3DRS_VERTEXBLEND, pmcMesh->m_maxFaceInfl - 1 );
		if ( pmcMesh->m_maxFaceInfl )
			pd3dDevice->SetRenderState ( D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE );
		
		D3DXMATRIX	matBoneMatrix_UP, matTemp;

		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pmcMesh->m_pBoneCombinationBuf->GetBufferPointer());
		for ( ipattr = 0; ipattr < pmcMesh->m_cpattr; ipattr++ )
		{
			for ( DWORD i = 0; i < pmcMesh->m_paletteSize; ++i )
			{
				DWORD matid = pBoneComb[ipattr].BoneId[i];
				if ( matid != UINT_MAX )
				{
					matBoneMatrix_UP = *pmcMesh->m_pBoneMatrix[matid];
					matBoneMatrix_UP._42 += m_fHeight;

					D3DXMatrixMultiply_MM( matTemp, pmcMesh->m_pBoneOffsetMat[matid], matBoneMatrix_UP );
					pd3dDevice->SetTransform ( D3DTS_WORLDMATRIX(i), &matTemp );
				}
			}
            
			DWORD dwThisAttribId = pBoneComb[ipattr].AttribId;
			pmcMesh->SetMaterial ( pd3dDevice, dwThisAttribId, pmtrlPiece, pmtrlSpecular );

			BOOL bDraw = m_bOriginDraw || ( pmtrlSpecular && pmtrlSpecular[dwThisAttribId].bEffUse );
			if ( bDraw )
			{
				RenderDefault ( pd3dDevice, pmcMesh, ipattr, dwThisAttribId, pmtrlPiece, pmtrlSpecular );
			}
		}
        
		pd3dDevice->SetRenderState ( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
		pd3dDevice->SetRenderState ( D3DRS_VERTEXBLEND, 0 );

		if (pmcMesh->m_bUseSW)
		{
			pd3dDevice->SetRenderState ( D3DRS_SOFTWAREVERTEXPROCESSING, FALSE );
		}
	}
	else if ( pmcMesh->m_Method == SOFTWARE )
	{
		D3DXMATRIX  Identity;
		DWORD       cBones  = pmcMesh->m_pSkinMesh->GetNumBones();

		// set up bone transforms
		for ( DWORD iBone = 0; iBone < cBones; ++iBone )
		{
			//D3DXMatrixMultiply
			//(
			//	&m_pBoneMatrices[iBone],                 // output
			//	&pmcMesh->m_pBoneOffsetMat[iBone], 
			//	pmcMesh->m_pBoneMatrix[iBone]
			//);
			D3DXMatrixMultiply_MM( m_pBoneMatrices[iBone], pmcMesh->m_pBoneOffsetMat[iBone], *pmcMesh->m_pBoneMatrix[iBone] );
		}

		// set world transform
        D3DXMatrixIdentity ( &Identity );
		Identity._42 += m_fHeight;
		hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &Identity );
		if (FAILED(hr))		return hr;

		// generate skinned mesh
		hr = pmcMesh->m_pSkinMesh->UpdateSkinnedMesh ( m_pBoneMatrices, NULL, pmcMesh->m_pMesh );
		if (FAILED(hr))		return hr;

		DWORD dwThisAttribId = 0;
		for ( ipattr = 0; ipattr < pmcMesh->m_cpattr; ipattr++ )
		{
			dwThisAttribId = pmcMesh->m_pAttrTable[ipattr].AttribId;
			pmcMesh->SetMaterial ( pd3dDevice, dwThisAttribId, pmtrlPiece, pmtrlSpecular );

			BOOL bDraw = m_bOriginDraw || ( pmtrlSpecular && pmtrlSpecular[dwThisAttribId].bEffUse );
			if ( bDraw )
			{
				RenderDefault ( pd3dDevice, pmcMesh, ipattr, dwThisAttribId, pmtrlPiece, pmtrlSpecular );
			}
		}
	}

	Init_Detail_Texture ();

	return S_OK;

	//	Note : ID3DXMesh 로 랜더링하기.
	//
_RETURN:
	if ( pmcMesh->m_numBoneComb == 3 && pmcMesh->m_pBoneMatrix )
	{
		D3DXMATRIX matCombined, matTemp;
		if ( pmcMesh->m_pBoneMatrix[2] )
		{
			//D3DXMatrixMultiply ( &matCombined, pmcMesh->m_pBoneMatrix[1], pmcMesh->m_pBoneMatrix[2] );
			//D3DXMatrixMultiply ( &matCombined, pmcMesh->m_pBoneMatrix[0], &matCombined );
			D3DXMatrixMultiply_MM ( matTemp, *pmcMesh->m_pBoneMatrix[1], *pmcMesh->m_pBoneMatrix[2] );
			D3DXMatrixMultiply_MM ( matCombined, *pmcMesh->m_pBoneMatrix[0], matTemp );
		}

		matCombined._42 += m_fHeight;
		m_matWorld = matCombined;		// 단독 Eff 에서 쓰일 것이다.

		hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &matCombined );
		if (FAILED(hr))	return hr;
	}

	DWORD dwThisAttribId = 0;
	for ( ipattr = 0; ipattr < pmcMesh->m_cpattr; ipattr++ )
	{
		dwThisAttribId = ipattr;
		pmcMesh->SetMaterial ( pd3dDevice, dwThisAttribId, pmtrlPiece, pmtrlSpecular );

			BOOL bDraw = m_bOriginDraw || ( pmtrlSpecular && pmtrlSpecular[dwThisAttribId].bEffUse );
			if ( bDraw )
			{
				RenderDefault ( pd3dDevice, pmcMesh, ipattr, dwThisAttribId, pmtrlPiece, pmtrlSpecular );
			}
	}

	Init_Detail_Texture ();

	return S_OK;
}

HRESULT DxSkinMesh::RenderDefault ( LPDIRECT3DDEVICE8 pd3dDevice, SMeshContainer *pmcMesh, int ipattr, DWORD dwThisAttribId,
								   SMATERIAL_PIECE *pmtrlPiece, DXMATERIAL_CHAR_EFF* pmtrlSpecular )
{
	HRESULT	hr = S_OK;

	pmcMesh->SetMaterial ( pd3dDevice, dwThisAttribId, pmtrlPiece, pmtrlSpecular );

	if ( !m_bShadow && pmcMesh->IsAlphaTex_HARD(dwThisAttribId) && m_bAlphaTex )
		DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );
	else if ( !m_bShadow && pmcMesh->IsAlphaTex_SOFT(dwThisAttribId) && m_bAlphaTex )
		DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );

	DWORD	dwAlphaBlendEnable, dwAlphaARG1, dwAlphaARG2, dwAlphaOP;
	if ( m_bAlpha )
	{
		pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
		pd3dDevice->GetTextureStageState ( 0, D3DTSS_ALPHAARG1,	&dwAlphaARG1 );
		pd3dDevice->GetTextureStageState ( 0, D3DTSS_ALPHAARG2,	&dwAlphaARG2 );
		pd3dDevice->GetTextureStageState ( 0, D3DTSS_ALPHAOP,	&dwAlphaOP );

		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );
	}

	if ( m_bLevel )			// 렙제가 된다면 이렇게 하는 것이 부하를 줄이게 해 준다.
	{
		LPDIRECT3DTEXTURE8 pTexture = NULL;

		if ( pmtrlPiece )
		{
			if ( pmtrlPiece[dwThisAttribId].m_pTexture )
				pTexture = pmtrlPiece[dwThisAttribId].m_pTexture;
		}
		if ( pTexture==NULL )
		{
			pTexture = pmcMesh->m_pMaterials[dwThisAttribId].pTexture;
		}

		pd3dDevice->SetTexture ( 1, pTexture );		
	}

	//DxTEX2Modulate ( pd3dDevice, 

	pmcMesh->m_pMesh->DrawSubset ( ipattr );

	if ( m_bLevel )			// 렙제가 된다면 이렇게 하는 것이 부하를 줄이게 해 준다.
	{
		pd3dDevice->SetTexture ( 1, NULL );
	}

	if ( m_bAlpha )
	{
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	dwAlphaARG1 );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	dwAlphaARG2 );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	dwAlphaOP );
	}

	pmcMesh->ReSetMaterial ( pd3dDevice );

	if ( !m_bShadow && pmcMesh->IsAlphaTex_HARD(dwThisAttribId) && m_bAlphaTex )
		DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );
	else if ( !m_bShadow && pmcMesh->IsAlphaTex_SOFT(dwThisAttribId) && m_bAlphaTex )
		DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );

	return hr;
}

HRESULT DxSkinMesh::RenderVS ( LPDIRECT3DDEVICE8 pd3dDevice, SMeshContainer *pmcMesh, LPD3DXBONECOMBINATION pBoneComb, int ipattr, 
							  DWORD dwThisAttribId, SMATERIAL_PIECE *pmtrlPiece, DXMATERIAL_CHAR_EFF* pmtrlSpecular )
{
	HRESULT	hr = S_OK;

	pmcMesh->SetMaterial ( pd3dDevice, dwThisAttribId, pmtrlPiece, pmtrlSpecular );

	if ( !m_bShadow && pmcMesh->IsAlphaTex_HARD(dwThisAttribId) && m_bAlphaTex )
		DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );
	else if ( !m_bShadow && pmcMesh->IsAlphaTex_SOFT(dwThisAttribId) && m_bAlphaTex )
		DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );

	DWORD	dwAlphaBlendEnable, dwAlphaARG1, dwAlphaARG2, dwAlphaOP;
	if ( m_bAlpha )
	{
		pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
		pd3dDevice->GetTextureStageState ( 0, D3DTSS_ALPHAARG1,	&dwAlphaARG1 );
		pd3dDevice->GetTextureStageState ( 0, D3DTSS_ALPHAARG2,	&dwAlphaARG2 );
		pd3dDevice->GetTextureStageState ( 0, D3DTSS_ALPHAOP,	&dwAlphaOP );

		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );
	}


//	pd3dDevice->SetVertexShader(m_dwTestVS[pmcMesh->m_maxFaceInfl - 1]);
//	pd3dDevice->SetPixelShader ( m_dwTestPS );

//	pd3dDevice->SetTexture ( 1, m_pTestTex );

	hr = m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
								pBoneComb[ipattr].VertexStart, pBoneComb[ipattr].VertexCount,
								pBoneComb[ipattr].FaceStart * 3, pBoneComb[ipattr].FaceCount);
	if(FAILED(hr))	return hr;

	pd3dDevice->SetPixelShader ( NULL );
	pd3dDevice->SetTexture ( 1, NULL );




	if ( m_bAlpha )
	{
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	dwAlphaARG1 );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	dwAlphaARG2 );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	dwAlphaOP );
	}

	pmcMesh->ReSetMaterial ( pd3dDevice );

	if ( !m_bShadow && pmcMesh->IsAlphaTex_HARD(dwThisAttribId) && m_bAlphaTex )
		DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );
	else if ( !m_bShadow && pmcMesh->IsAlphaTex_SOFT(dwThisAttribId) && m_bAlphaTex )
		DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );

	return hr;
}

HRESULT DxSkinMesh::UpdateFrames ( SFrame *pframeCur, D3DXMATRIX &matCur )
{
	HRESULT hr = S_OK;

	//	Note : pframeCur의 메트릭스를 계산. matCombined = matRot * matParent * matTrans
	//
	//pframeCur->matCombined = matCur;
	//D3DXMatrixMultiply ( &pframeCur->matCombined, &pframeCur->matRot, &matCur );
	D3DXMatrixMultiply_MM( pframeCur->matCombined, pframeCur->matRot, matCur );

	//	Note : 자식 프레임의 메트릭스 모두 계산.
	//
	SFrame *pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = UpdateFrames ( pframeChild, pframeCur->matCombined );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}
    
	return S_OK;
}


HRESULT DxSkinMesh::DrawFrames ( LPDIRECT3DDEVICE8 pd3dDevice, SFrame *pframeCur, UINT &cTriangles )
{
	HRESULT hr = S_OK;
	SMeshContainer *pmcMesh;
	SFrame *pframeChild;

	//	Note : 트렌스폼 설정.
	//
	if ( pframeCur->pmcMesh != NULL )
	{
		hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );
		if (FAILED(hr))	return hr;
	}

	//	Note : 메쉬 그리기.
	//
	pmcMesh = pframeCur->pmcMesh;
	while ( pmcMesh != NULL )
	{
		hr = DrawMeshContainer ( pd3dDevice, pmcMesh );
		if (FAILED(hr))
			return hr;

		cTriangles += pmcMesh->m_pMesh->GetNumFaces();

		pmcMesh = pmcMesh->m_pmcNext;
	}

	//	Note : 자식 프레임 그리기.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = DrawFrames ( pd3dDevice, pframeChild, cTriangles );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

HRESULT DxSkinMesh::DrawMeshClobal ( LPDIRECT3DDEVICE8 pd3dDevice, UINT &cTriangles )
{
	HRESULT hr;

	//	Note : 메쉬 그리기.
	//
	SMeshContainer *pmcMesh = pMeshGlobalHead;
	while ( pmcMesh != NULL )
	{
		hr = DrawMeshContainer ( pd3dDevice, pmcMesh );
		if ( FAILED(hr) )	return hr;

		cTriangles += pmcMesh->m_pMesh->GetNumFaces ();
		pmcMesh = pmcMesh->m_pmcGlobalNext;
	}

	return S_OK;
}

HRESULT DxSkinMesh::UpdateEffMesh ( LPDIRECT3DDEVICE8 pd3dDevice, SMeshContainer* pmcMesh )
{
	HRESULT hr = S_OK;
	if ( !pmcMesh->m_pSkinMesh )	return E_FAIL;

	//	Note : 현제 이펙트 메쉬가 준비되었는가 검사후 생성.
	//
	if ( !pmcMesh->IsEffMesh() )
	{
		hr = pmcMesh->GenerateEffMesh ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}

	D3DXMATRIX  Identity;
	DWORD       cBones  = pmcMesh->m_pSkinMesh->GetNumBones();

	// set up bone transforms
	for ( DWORD iBone = 0; iBone < cBones; ++iBone )
	{
		//D3DXMatrixMultiply
		//(
		//	&m_pBoneMatrices[iBone],                 // output
		//	&pmcMesh->m_pBoneOffsetMat[iBone], 
		//	pmcMesh->m_pBoneMatrix[iBone]
		//);
		D3DXMatrixMultiply_MM( m_pBoneMatrices[iBone], pmcMesh->m_pBoneOffsetMat[iBone], *pmcMesh->m_pBoneMatrix[iBone] );
	}

	hr = pmcMesh->UpdateEffMesh ( m_pBoneMatrices );
	if (FAILED(hr))		return hr;

	return hr;
}

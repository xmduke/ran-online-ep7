#include "stdafx.h"

#include "DxMethods.h"
#include "EDITMESHS.h"
#include "COLLISION.h"
#include "SerialFile.h"
#include "../DxCommon/TextureManager.h"
#include "DxSkinPieceContainer.h"

#include "SMeshContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SBoneInfluence::SBoneInfluence () :
	dwInfue(0)
{
}

void SBoneInfluence::SetInfue ( DWORD _dwInfue )
{
	dwInfue = _dwInfue;
	GASSERT(dwInfue<=MAX_BONE);
}

//------------------------------------------------------------------------------------[SVERTEXINFLU]
//
const DWORD SVERTEXINFLU::VERSION = 0x0101;

SVERTEXINFLU::SVERTEXINFLU () :
	m_dwIndex(COL_ERR),
	m_vVector(0,0,0),
	m_vNormal(0,0,0),
	
	m_dwNumBone(COL_ERR)
{
}

void SVERTEXINFLU::SetBoneNum ( DWORD _dwBone )
{
	if ( _dwBone==COL_ERR )
	{
		m_dwNumBone = _dwBone;
		return;
	}

	m_dwNumBone = _dwBone;
	GASSERT(m_dwNumBone<=MAX_BONE);
}

BOOL SVERTEXINFLU::SaveFile ( CSerialFile &SFile )
{
	DWORD dwSize = sizeof(DWORD) +
					sizeof(D3DXVECTOR3) +
					sizeof(D3DXVECTOR3) +
					sizeof(DWORD) +
					sizeof(DWORD)*m_dwNumBone +
					sizeof(float)*m_dwNumBone;
	
	if ( m_dwNumBone > 0 && m_dwNumBone!=COL_ERR )
	{
		GASSERT(m_pBone);
		GASSERT(m_pWeights);

        dwSize += sizeof(DWORD)*m_dwNumBone + sizeof(float)*m_dwNumBone;
	}

	SFile << VERSION;
	SFile << dwSize;

	SFile << m_dwIndex;
	SFile << m_vVector;
	SFile << m_vNormal;

	SFile << m_dwNumBone;

	if ( m_dwNumBone > 0 && m_dwNumBone!=COL_ERR )
	{
		SFile.WriteBuffer ( m_pBone, sizeof(DWORD)*m_dwNumBone );
		SFile.WriteBuffer ( m_pWeights, sizeof(float)*m_dwNumBone );
	}

	return TRUE;
}

BOOL SVERTEXINFLU::LoadFile ( basestream &SFile )
{
	DWORD dwVer;
	DWORD dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	if ( dwVer == SVERTEXINFLU::VERSION )
	{
		SFile >> m_dwIndex;
		SFile >> m_vVector;
		SFile >> m_vNormal;

		SFile >> m_dwNumBone;

		SetBoneNum(m_dwNumBone);	//	점검 차원에서 호출.
		if ( m_dwNumBone > 0 && m_dwNumBone != COL_ERR )
		{
			SFile.ReadBuffer ( m_pBone, sizeof(DWORD)*m_dwNumBone );
			SFile.ReadBuffer ( m_pWeights, sizeof(float)*m_dwNumBone );
		}
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell() + dwSize );
	}

	return TRUE;
}

//-----------------------------------------------------------------------------------[SMeshContainer]
//
SMeshContainer::~SMeshContainer ()
{
	if ( m_pMaterials )
	{
		//	Note : 텍스쳐 릴리즈.
		for ( DWORD i = 0; i<m_cMaterials; ++i )
		{
			m_pMaterials[i].RELEASE ();
		}

		//	Note : 메터리얼 배열 삭제.
		SAFE_DELETE_ARRAY(m_pMaterials);
	}

	GXRELEASE(m_pMesh);
	GXRELEASE(m_pEffVBuffer);
	SAFE_DELETE_ARRAY(m_pBoneInfluences);

	GXRELEASE(m_pSkinMesh);
	GXRELEASE(m_pBoneOffsetBuf);
	GXRELEASE(m_pBoneCombinationBuf);

	SAFE_DELETE_ARRAY(m_pBoneMatrix);

	SAFE_DELETE_ARRAY(m_pAttrTable);

	SAFE_DELETE_ARRAY(m_szName);

	SAFE_DELETE_ARRAY(m_rgiAdjacency);

	SAFE_DELETE(m_pmcNext);
}

HRESULT SMeshContainer::SetMaterial ( LPDIRECT3DDEVICE8 pd3dDevice, DWORD nIndex, SMATERIAL_PIECE *pmtrlPiece, DXMATERIAL_CHAR_EFF* pmtrlSpecular )
{
	if( m_cMaterials<=nIndex )
	{
		CDebugSet::ToLogFile( "SMeshContainer::SetMaterial() -- if( m_cMaterials<=nIndex )");
		return S_OK;
	}

	m_pMaterials[nIndex].d3dMaterial.Emissive.a = 0.f;
	m_pMaterials[nIndex].d3dMaterial.Emissive.r = 0.f;
	m_pMaterials[nIndex].d3dMaterial.Emissive.g = 0.f;
	m_pMaterials[nIndex].d3dMaterial.Emissive.b = 0.f;

	m_pMaterials[nIndex].d3dMaterial.Power = 14.f;						// 30.f - 디폴트

	DWORD dwZBias(0);
	D3DMATERIAL8*		pd3dMaterial = NULL;
	LPDIRECT3DTEXTURE8	pTexture	= NULL;

	if ( pmtrlSpecular )
	{
		if ( pmtrlSpecular[nIndex].bEffUse )
		{
			pd3dMaterial = &(pmtrlSpecular[nIndex].d3dMaterial);
			pTexture = pmtrlSpecular[nIndex].pEffTex;
			dwZBias = 2;

			D3DCOLOR dwColor = D3DCOLOR_COLORVALUE ( pd3dMaterial->Diffuse.r, pd3dMaterial->Diffuse.g,
											pd3dMaterial->Diffuse.b, pd3dMaterial->Diffuse.a );

			pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, dwColor );	// 음. ^^;
		}
	}
	else if ( pmtrlPiece )
	{
		if ( pmtrlPiece[nIndex].m_pTexture )	pTexture = pmtrlPiece[nIndex].m_pTexture;
	
		dwZBias = pmtrlPiece[nIndex].m_dwZBias;
	}

	// Base Texture
	if( !pTexture )
	{
		if( !m_pMaterials[nIndex].pTexture )
		{
			TextureManager::GetTexture( m_pMaterials[nIndex].strTexture.c_str(), m_pMaterials[nIndex].pTexture );
			
			if( m_pMaterials[nIndex].pTexture )
			{
				TextureManager::EM_TEXTYPE emType = TextureManager::GetTexType( m_pMaterials[nIndex].strTexture.c_str() );
				if( emType==TextureManager::EMTT_ALPHA_HARD )			m_pMaterials[nIndex].dwFlags |= DXMATERIAL::FLAG_ALPHA_HARD;
				else if( emType==TextureManager::EMTT_ALPHA_SOFT )		m_pMaterials[nIndex].dwFlags |= DXMATERIAL::FLAG_ALPHA_SOFT;
				else if( emType==TextureManager::EMTT_ALPHA_SOFT01 )	m_pMaterials[nIndex].dwFlags |= DXMATERIAL::FLAG_ALPHA_SOFT;
				else if( emType==TextureManager::EMTT_ALPHA_SOFT02 )	m_pMaterials[nIndex].dwFlags |= DXMATERIAL::FLAG_ALPHA_SOFT;
			}
		}
		
		pTexture = m_pMaterials[nIndex].pTexture;
	}

	pd3dDevice->SetRenderState ( D3DRS_ZBIAS, dwZBias );
	pd3dDevice->SetMaterial ( &m_pMaterials[nIndex].d3dMaterial );
	pd3dDevice->SetTexture ( 0, pTexture );

	return S_OK;
}

HRESULT SMeshContainer::ReSetMaterial ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	pd3dDevice->SetRenderState ( D3DRS_ZBIAS, 0 );

	return S_OK;
}

BOOL SMeshContainer::IsAlphaTex_HARD ( DWORD nIndex )
{
	GASSERT(m_cMaterials>nIndex);

	return m_pMaterials[nIndex].dwFlags & DXMATERIAL::FLAG_ALPHA_HARD;
}

BOOL SMeshContainer::IsAlphaTex_SOFT ( DWORD nIndex )
{
	GASSERT(m_cMaterials>nIndex);

	return m_pMaterials[nIndex].dwFlags & DXMATERIAL::FLAG_ALPHA_SOFT;
}

BOOL SMeshContainer::IsCollision ( const D3DXVECTOR3 &vPoint1, const D3DXVECTOR3 &vPoint2, D3DXVECTOR3 &vDetectPos, DWORD &dwDetectFace )
{
	HRESULT hr = S_OK;
	METHOD OldMethod = m_Method;
	float fDistNew = FLT_MAX, fDistOld = FLT_MAX;
	D3DXMATRIX matCombined;

	dwDetectFace = COL_ERR;
	vDetectPos = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

	if ( !m_pMesh && !m_pSkinMesh )		return FALSE;

	if ( m_pSkinMesh )
	{
		if ( m_pSkinMesh->GetOptions()&D3DXMESH_32BIT )	return FALSE;

		DWORD dwNumFaces = m_pSkinMesh->GetNumFaces ();
		DWORD dwNumVertices = m_pSkinMesh->GetNumVertices ();
		
		DWORD dwSkinFVF = D3DFVF_XYZ;
		DWORD dwSkinVertSize = D3DXGetFVFVertexSize ( dwSkinFVF );

		PBYTE pbData = new BYTE[dwSkinVertSize*dwNumVertices];
		hr = UpdateSkinnedMesh ( pbData, dwSkinFVF );
		if ( FAILED(hr) )
		{
			SAFE_DELETE_ARRAY(pbData);
			return FALSE;
		}

		WORD *pIndexBuffer;
		hr = m_pSkinMesh->LockIndexBuffer ( D3DLOCK_READONLY, (BYTE**)&pIndexBuffer );
		if ( FAILED(hr) )
		{
			SAFE_DELETE_ARRAY(pbData);
			return FALSE;
		}

		DWORD dwCollisionFace = COL_ERR;
		D3DXVECTOR3 vDetect = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
		for ( DWORD j=0; j<dwNumFaces; j++ )
		{
			PWORD pwIndex = pIndexBuffer + 3*j;

			D3DXVECTOR3 *pVector0 = (D3DXVECTOR3*) ( pbData + dwSkinVertSize * ( *(pwIndex+0) ) );
			D3DXVECTOR3 *pVector1 = (D3DXVECTOR3*) ( pbData + dwSkinVertSize * ( *(pwIndex+1) ) );
			D3DXVECTOR3 *pVector2 = (D3DXVECTOR3*) ( pbData + dwSkinVertSize * ( *(pwIndex+2) ) );

			D3DXVECTOR3 vCollisionPos;
			BOOL bCollision = COLLISION::IsLineTriangleCollision
			(
				(D3DXVECTOR3*)pVector0,
				(D3DXVECTOR3*)pVector1,
				(D3DXVECTOR3*)pVector2,
				(D3DXVECTOR3*)&vPoint1,
				(D3DXVECTOR3*)&vPoint2,
				&vCollisionPos
			);

			if ( bCollision )
			{
				D3DXVECTOR3 vDist = vPoint1 - vCollisionPos;
				fDistNew = D3DXVec3Length ( &vDist );
				
				if ( fDistNew <= fDistOld )
				{
					dwCollisionFace = j;
					vDetect = vCollisionPos;

					fDistOld = fDistNew;
				}
			}
		}

		m_pSkinMesh->UnlockIndexBuffer ();

		vDetectPos = vDetect;
		dwDetectFace = dwCollisionFace;

		SAFE_DELETE_ARRAY(pbData);
	}
	//	Note : 일반 메시.
	//
	else
	{
		if ( m_pMesh->GetOptions()&D3DXMESH_32BIT )	return FALSE;

		DWORD dwNumFaces = m_pMesh->GetNumFaces ();
		DWORD dwNumVertices = m_pMesh->GetNumVertices ();

		DWORD dwFVF = m_pMesh->GetFVF ();
		DWORD dwVertSize = D3DXGetFVFVertexSize ( dwFVF );

		BYTE *pbData = NULL;
		hr = m_pMesh->LockVertexBuffer ( D3DLOCK_READONLY, &pbData );
		if ( FAILED(hr) )	return FALSE;

		WORD *pIndexBuffer;
		hr = m_pMesh->LockIndexBuffer ( D3DLOCK_READONLY, (BYTE**)&pIndexBuffer );
		if ( FAILED(hr) )
		{
			m_pMesh->UnlockVertexBuffer ();
			return FALSE;
		}

		if ( m_numBoneComb == 3 && m_pBoneMatrix )
		{
			//	특정 본에 영향을 받아야하지만 그 본이 존제 안할 경우가 있을때 대비.
			D3DXMATRIX	matTemp;
			if ( m_pBoneMatrix[2] )
			{
				//D3DXMatrixMultiply ( &matCombined, m_pBoneMatrix[1], m_pBoneMatrix[2] );
				//D3DXMatrixMultiply ( &matCombined, m_pBoneMatrix[0], &matCombined );
				D3DXMatrixMultiply_MM ( matTemp, *m_pBoneMatrix[1], *m_pBoneMatrix[2] );
				D3DXMatrixMultiply_MM ( matCombined, *m_pBoneMatrix[0], matTemp );
			}
		}

		DWORD dwCollisionFace = COL_ERR;
		D3DXVECTOR3 vDetect = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
		for ( DWORD j=0; j<dwNumFaces; j++ )
		{
			PWORD pwIndex = pIndexBuffer+0 + 3*j;

			D3DXVECTOR3 Vector0 = *(D3DXVECTOR3*) ( pbData + dwVertSize*(*(pwIndex+0)) );
			D3DXVECTOR3 Vector1 = *(D3DXVECTOR3*) ( pbData + dwVertSize*(*(pwIndex+1)) );
			D3DXVECTOR3 Vector2 = *(D3DXVECTOR3*) ( pbData + dwVertSize*(*(pwIndex+2)) );

			D3DXVec3TransformCoord ( &Vector0, &Vector0, &matCombined );
			D3DXVec3TransformCoord ( &Vector1, &Vector1, &matCombined );
			D3DXVec3TransformCoord ( &Vector2, &Vector2, &matCombined );

			D3DXVECTOR3 vCollisionPos;
			BOOL bCollision = COLLISION::IsLineTriangleCollision
			(
				(D3DXVECTOR3*)&Vector0,
				(D3DXVECTOR3*)&Vector1,
				(D3DXVECTOR3*)&Vector2,
				(D3DXVECTOR3*)&vPoint1,
				(D3DXVECTOR3*)&vPoint2,
				&vCollisionPos
			);

			if ( bCollision )
			{
				D3DXVECTOR3 vDist = vPoint1 - vCollisionPos;
				fDistNew = D3DXVec3Length ( &vDist );
				
				if ( fDistNew <= fDistOld )
				{
					dwCollisionFace = j;
					vDetect = vCollisionPos;

					fDistOld = fDistNew;
				}
			}
		}

		vDetectPos = vDetect;
		dwDetectFace = dwCollisionFace;

		m_pMesh->UnlockVertexBuffer ();
		m_pMesh->UnlockIndexBuffer ();
	}

	return (dwDetectFace!=COL_ERR);
}

HRESULT SMeshContainer::UpdateSkinnedMesh ( PBYTE pbData, DWORD dwFVF, LPD3DXMATRIX pBoneMatrices )
{
	HRESULT hr = S_OK;

	GASSERT(pbData);

	DWORD dwVertSize = D3DXGetFVFVertexSize ( dwFVF );
	DWORD dwNumVertices = m_pSkinMesh->GetNumVertices();

	BYTE *pbSkinData = NULL;
	hr = m_pSkinMesh->LockVertexBuffer ( D3DLOCK_READONLY, &pbSkinData );
	if ( FAILED(hr) )	return E_FAIL;

	DWORD dwSkinFVF = m_pSkinMesh->GetFVF();
	DWORD dwSkinVertSize = D3DXGetFVFVertexSize ( dwSkinFVF );

	D3DXVECTOR3 *pVector = NULL;
	D3DXVECTOR3 *pSkinVector = NULL;

	//	Note : 값을 초기화 한다.
	//
	for ( DWORD i=0; i<dwNumVertices; i++ )
	{
		pVector = (D3DXVECTOR3*) ( pbData + dwVertSize*i );
		pVector->x = pVector->y = pVector->z = 0;
	}

	//	set up bone transforms
	//
	DWORD cBones  = m_pSkinMesh->GetNumBones();

	BOOL bBoneMatNew = FALSE;
	if ( !pBoneMatrices )
	{
		bBoneMatNew = TRUE;
		pBoneMatrices = new D3DXMATRIX[cBones];

		for ( DWORD iBone = 0; iBone < cBones; ++iBone )
		{
			D3DXMatrixMultiply_MM( pBoneMatrices[iBone], m_pBoneOffsetMat[iBone], *m_pBoneMatrix[iBone] );
		}
	}

	DWORD dwInfue;
	DWORD* pVertices;
	float* pWeights;

	D3DXVECTOR3 vVecWeight;

	for ( i=0; i<cBones; i++ )
	{
		if ( m_pBoneInfluences )
		{
			dwInfue = m_pBoneInfluences[i].dwInfue;
			pVertices = m_pBoneInfluences[i].pVertices;
			pWeights = m_pBoneInfluences[i].pWeights;
		}
		else
		{
			dwInfue = m_pSkinMesh->GetNumBoneInfluences (i);

			pVertices = new DWORD[dwInfue];
			pWeights = new float[dwInfue];
			m_pSkinMesh->GetBoneInfluence ( i, pVertices, pWeights );
		}

		for ( DWORD j=0; j<dwInfue; j++ )
		{
			pVector = (D3DXVECTOR3*) ( pbData + dwVertSize*pVertices[j] );
			pSkinVector = (D3DXVECTOR3*) ( pbSkinData + dwSkinVertSize*pVertices[j] );

			D3DXVec3TransformCoord ( &vVecWeight, pSkinVector, &pBoneMatrices[i] );

			*pVector += vVecWeight*pWeights[j];
		}

		if ( !m_pBoneInfluences )
		{
			SAFE_DELETE_ARRAY(pVertices);
			SAFE_DELETE_ARRAY(pWeights);
		}
	}

	if ( bBoneMatNew )	SAFE_DELETE_ARRAY(pBoneMatrices);
	m_pSkinMesh->UnlockVertexBuffer ();

	return S_OK;
}

BOOL SMeshContainer::GetClosedPoint ( const D3DXVECTOR3 &vPoint1, const D3DXVECTOR3 &vPoint2, D3DXVECTOR3 &vDetectPos, DWORD &dwDetectIndex )
{
	HRESULT hr = S_OK;
	METHOD OldMethod = m_Method;
	float fDistNew = FLT_MAX, fDistOld = FLT_MAX;
	D3DXMATRIX matCombined;

	dwDetectIndex = COL_ERR;
	vDetectPos = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

	if ( !m_pMesh && !m_pSkinMesh )		return FALSE;

	if ( m_pSkinMesh )
	{
		if ( m_pSkinMesh->GetOptions()&D3DXMESH_32BIT )	return FALSE;

		DWORD dwNumFaces = m_pSkinMesh->GetNumFaces ();
		DWORD dwNumVertices = m_pSkinMesh->GetNumVertices ();
		
		DWORD dwSkinFVF = D3DFVF_XYZ;
		DWORD dwSkinVertSize = D3DXGetFVFVertexSize ( dwSkinFVF );

		PBYTE pbData = new BYTE[dwSkinVertSize*dwNumVertices];
		hr = UpdateSkinnedMesh ( pbData, dwSkinFVF );
		if ( FAILED(hr) )
		{
			SAFE_DELETE_ARRAY(pbData);
			return FALSE;
		}

		WORD *pIndexBuffer;
		hr = m_pSkinMesh->LockIndexBuffer ( D3DLOCK_READONLY, (BYTE**)&pIndexBuffer );
		if ( FAILED(hr) )
		{
			SAFE_DELETE_ARRAY(pbData);
			return FALSE;
		}

		DWORD dwCollisionFace = COL_ERR;
		D3DXVECTOR3 vDetect = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
		for ( DWORD j=0; j<dwNumFaces; j++ )
		{
			PWORD pwIndex = pIndexBuffer + 3*j;

			D3DXVECTOR3 *pVector0 = (D3DXVECTOR3*) ( pbData + dwSkinVertSize * ( *(pwIndex+0) ) );
			D3DXVECTOR3 *pVector1 = (D3DXVECTOR3*) ( pbData + dwSkinVertSize * ( *(pwIndex+1) ) );
			D3DXVECTOR3 *pVector2 = (D3DXVECTOR3*) ( pbData + dwSkinVertSize * ( *(pwIndex+2) ) );

			D3DXVECTOR3 vCollisionPos;
			BOOL bCollision = COLLISION::IsLineTriangleCollision
			(
				(D3DXVECTOR3*)pVector0,
				(D3DXVECTOR3*)pVector1,
				(D3DXVECTOR3*)pVector2,
				(D3DXVECTOR3*)&vPoint1,
				(D3DXVECTOR3*)&vPoint2,
				&vCollisionPos
			);

			if ( bCollision )
			{
				D3DXVECTOR3 vDist = vPoint1 - vCollisionPos;
				fDistNew = D3DXVec3Length ( &vDist );
				
				if ( fDistNew <= fDistOld )
				{
					dwCollisionFace = j;
					vDetect = vCollisionPos;

					fDistOld = fDistNew;
				}
			}
		}

		m_pSkinMesh->UnlockIndexBuffer ();

		if ( dwCollisionFace == COL_ERR )
		{
			SAFE_DELETE_ARRAY(pbData);
			return FALSE;
		}

		DWORD dwClosed = COL_ERR;
		D3DXVECTOR3 vClosed = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

		fDistNew = FLT_MAX;
		fDistOld = FLT_MAX;
		for ( DWORD i=0; i<dwNumVertices; i++ )
		{
			D3DXVECTOR3 Vector0 = *(D3DXVECTOR3*) ( pbData + dwSkinVertSize*i );

			if ( !m_pSkinMesh )
				D3DXVec3TransformCoord ( &Vector0, &Vector0, &matCombined );

			D3DXVECTOR3 vDist = Vector0 - vDetect;
			fDistNew = D3DXVec3Length ( &vDist );
			
			if ( fDistNew < fDistOld )
			{
				dwClosed = i;
				vClosed = Vector0;

				fDistOld = fDistNew;
			}
		}

		vDetectPos = vClosed;
		dwDetectIndex = dwClosed;

		SAFE_DELETE_ARRAY(pbData);
	}
	//	Note : 일반 메시.
	//
	else
	{
		if ( m_pMesh->GetOptions()&D3DXMESH_32BIT )	return FALSE;

		DWORD dwNumFaces = m_pMesh->GetNumFaces ();
		DWORD dwNumVertices = m_pMesh->GetNumVertices ();

		DWORD dwFVF = m_pMesh->GetFVF ();
		DWORD dwVertSize = D3DXGetFVFVertexSize ( dwFVF );

		BYTE *pbData = NULL;
		hr = m_pMesh->LockVertexBuffer ( D3DLOCK_READONLY, &pbData );
		if ( FAILED(hr) )	return FALSE;

		WORD *pIndexBuffer;
		hr = m_pMesh->LockIndexBuffer ( D3DLOCK_READONLY, (BYTE**)&pIndexBuffer );
		if ( FAILED(hr) )
		{
			m_pMesh->UnlockVertexBuffer ();
			return FALSE;
		}

		if ( m_numBoneComb == 3 && m_pBoneMatrix )
		{
			//	특정 본에 영향을 받아야하지만 그 본이 존제 안할 경우가 있을때 대비.
			D3DXMATRIX	matTemp;
			if ( m_pBoneMatrix[2] )
			{
				//D3DXMatrixMultiply ( &matCombined, m_pBoneMatrix[1], m_pBoneMatrix[2] );
				//D3DXMatrixMultiply ( &matCombined, m_pBoneMatrix[0], &matCombined );
				D3DXMatrixMultiply_MM ( matTemp, *m_pBoneMatrix[1], *m_pBoneMatrix[2] );
				D3DXMatrixMultiply_MM ( matCombined, *m_pBoneMatrix[0], matTemp );
			}
		}

		DWORD dwCollisionFace = COL_ERR;
		D3DXVECTOR3 vDetect = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
		for ( DWORD j=0; j<dwNumFaces; j++ )
		{
			PWORD pwIndex = pIndexBuffer+0 + 3*j;

			D3DXVECTOR3 Vector0 = *(D3DXVECTOR3*) ( pbData + dwVertSize*(*(pwIndex+0)) );
			D3DXVECTOR3 Vector1 = *(D3DXVECTOR3*) ( pbData + dwVertSize*(*(pwIndex+1)) );
			D3DXVECTOR3 Vector2 = *(D3DXVECTOR3*) ( pbData + dwVertSize*(*(pwIndex+2)) );

			D3DXVec3TransformCoord ( &Vector0, &Vector0, &matCombined );
			D3DXVec3TransformCoord ( &Vector1, &Vector1, &matCombined );
			D3DXVec3TransformCoord ( &Vector2, &Vector2, &matCombined );

			D3DXVECTOR3 vCollisionPos;
			BOOL bCollision = COLLISION::IsLineTriangleCollision
			(
				(D3DXVECTOR3*)&Vector0,
				(D3DXVECTOR3*)&Vector1,
				(D3DXVECTOR3*)&Vector2,
				(D3DXVECTOR3*)&vPoint1,
				(D3DXVECTOR3*)&vPoint2,
				&vCollisionPos
			);

			if ( bCollision )
			{
				D3DXVECTOR3 vDist = vPoint1 - vCollisionPos;
				fDistNew = D3DXVec3Length ( &vDist );
				
				if ( fDistNew <= fDistOld )
				{
					dwCollisionFace = j;
					vDetect = vCollisionPos;

					fDistOld = fDistNew;
				}
			}
		}

		if ( dwCollisionFace == COL_ERR )
		{
			m_pMesh->UnlockVertexBuffer ();
			m_pMesh->UnlockIndexBuffer ();
			return FALSE;
		}

		DWORD dwClosed = COL_ERR;
		D3DXVECTOR3 vClosed = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

		fDistNew = FLT_MAX;
		fDistOld = FLT_MAX;
		for ( DWORD i=0; i<dwNumVertices; i++ )
		{
			D3DXVECTOR3 Vector0 = *(D3DXVECTOR3*) ( pbData + dwVertSize*i );

			if ( !m_pSkinMesh )
				D3DXVec3TransformCoord ( &Vector0, &Vector0, &matCombined );

			D3DXVECTOR3 vDist = Vector0 - vDetect;
			fDistNew = D3DXVec3Length ( &vDist );
			
			if ( fDistNew < fDistOld )
			{
				dwClosed = i;
				vClosed = Vector0;

				fDistOld = fDistNew;
			}
		}

		m_pMesh->UnlockVertexBuffer ();
		m_pMesh->UnlockIndexBuffer ();

		vDetectPos = vClosed;
		dwDetectIndex = dwClosed;
	}

	return (dwDetectIndex!=COL_ERR);
}

HRESULT SMeshContainer::GetVertexInfluences ( const DWORD dwIndex, SVERTEXINFLU *pVertInflu )
{
	HRESULT hr = S_OK;
	if ( !m_pMesh && !m_pSkinMesh )		return E_FAIL;
	if ( dwIndex == COL_ERR )			return E_FAIL;

	//	Note : 스킨 메시의 경우.
	//
	if ( m_pSkinMesh )
	{
		DWORD dwNumBone = m_pSkinMesh->GetNumBones ();

		DWORD dwMaxVerInflu = 0;
		hr = m_pSkinMesh->GetMaxVertexInfluences ( &dwMaxVerInflu );
		if ( FAILED(hr) )	return E_FAIL;

		DWORD dwCount0 = 0;
		PDWORD pdwBone0 = new DWORD[dwMaxVerInflu];
		PFLOAT pfWeights0 = new FLOAT[dwMaxVerInflu];

		for ( DWORD i=0; i<dwNumBone; i++ )
		{
			DWORD dwInfue = m_pSkinMesh->GetNumBoneInfluences (i);
			if ( dwInfue==0 )	continue;

			DWORD* pVertices = new DWORD[dwInfue];
			float* pWeights = new float[dwInfue];

			m_pSkinMesh->GetBoneInfluence ( i, pVertices, pWeights );
			for ( DWORD j=0; j<dwInfue; j++ )
			{
				if ( dwIndex == pVertices[j] )
				{
					GASSERT(dwMaxVerInflu>dwCount0);

					pdwBone0[dwCount0] = i;
					pfWeights0[dwCount0] = pWeights[j];
					++dwCount0;
				}
			}

			SAFE_DELETE_ARRAY(pVertices);
			SAFE_DELETE_ARRAY(pWeights);
		}

		DWORD dwFVFMesh = m_pSkinMesh->GetFVF ();
		DWORD dwVertexSize = D3DXGetFVFVertexSize ( dwFVFMesh );
		DWORD dwNumVertices = m_pSkinMesh->GetNumVertices ();

		if ( dwIndex >= dwNumVertices )
		{
			SAFE_DELETE_ARRAY(pdwBone0);
			SAFE_DELETE_ARRAY(pfWeights0);
			return E_FAIL;
		}

		BYTE *pbData = NULL;
		hr = m_pSkinMesh->LockVertexBuffer ( D3DLOCK_READONLY, &pbData );
		if ( FAILED(hr) )
		{
			SAFE_DELETE_ARRAY(pdwBone0);
			SAFE_DELETE_ARRAY(pfWeights0);
			return FALSE;
		}

		D3DXVECTOR3 vVector, vNormal;
			
		vVector = *( (D3DXVECTOR3*) ( pbData+dwVertexSize*dwIndex ) );
		
		if ( dwFVFMesh&D3DFVF_NORMAL )
			vNormal = *( (D3DXVECTOR3*) ( pbData+dwVertexSize*dwIndex + sizeof(D3DXVECTOR3) ) );

		m_pSkinMesh->UnlockVertexBuffer ();


		//	Note : 버텍스 정보.
		//
		pVertInflu->m_dwIndex = dwIndex;
		pVertInflu->m_vVector = vVector;
		pVertInflu->m_vNormal = vNormal;

		pVertInflu->SetBoneNum ( dwCount0 );
		memcpy ( pVertInflu->m_pBone, pdwBone0, sizeof(DWORD)*dwCount0 );
		memcpy ( pVertInflu->m_pWeights, pfWeights0, sizeof(float)*dwCount0 );


		SAFE_DELETE_ARRAY(pdwBone0);
		SAFE_DELETE_ARRAY(pfWeights0);
	}
	//	Note : 일반 메시의 경우.
	//
	else
	{
		DWORD dwFVFMesh = m_pMesh->GetFVF ();
		DWORD dwVertexSize = D3DXGetFVFVertexSize ( dwFVFMesh );
		DWORD dwNumVertices = m_pMesh->GetNumVertices ();
		if ( dwIndex >= dwNumVertices )		return E_FAIL;

		BYTE *pbData = NULL;
		hr = m_pMesh->LockVertexBuffer ( D3DLOCK_READONLY, &pbData );
		if ( FAILED(hr) )	return FALSE;

		D3DXVECTOR3 vNormal;
		D3DXVECTOR3 vVector = *( (D3DXVECTOR3*) (pbData+dwVertexSize*dwIndex) );
		
		if ( dwFVFMesh&D3DFVF_NORMAL )
			vNormal = *( (D3DXVECTOR3*) ( pbData+dwVertexSize*dwIndex + sizeof(D3DXVECTOR3) ) );

		m_pMesh->UnlockVertexBuffer ();

		//	Note : 버텍스 정보.
		//
		pVertInflu->m_dwIndex = dwIndex;
		pVertInflu->m_vVector = vVector;
		pVertInflu->m_vNormal = vNormal;
		pVertInflu->m_dwNumBone = COL_ERR;
	}

	return S_OK;
}

HRESULT SMeshContainer::CalculateVertexInfluences ( SVERTEXINFLU *pVertInflu, D3DXVECTOR3 &vVecOutput, D3DXVECTOR3 &vNorOutput, LPD3DXMATRIX pBoneMatrice )
{
	D3DXVECTOR3 vVecComb(0,0,0);
	D3DXVECTOR3 vVecNormal(0,0,0);
	D3DXMATRIX matCombined;

	if ( pVertInflu->m_dwNumBone==COL_ERR )
	{
		if ( !m_pSkinMesh && m_numBoneComb == 3 && m_pBoneMatrix )
		{
			//	특정 본에 영향을 받아야하지만 그 본이 존제 안할 경우가 있을때 대비.
			D3DXMATRIX	matTemp;
			if ( m_pBoneMatrix[2] )
			{
				D3DXMatrixMultiply_MM ( matTemp, *m_pBoneMatrix[1], *m_pBoneMatrix[2] );
				D3DXMatrixMultiply_MM ( matCombined, *m_pBoneMatrix[0], matTemp );
			}

			D3DXVec3TransformCoord ( &vVecComb, &pVertInflu->m_vVector, &matCombined );
			D3DXVec3TransformNormal ( &vVecNormal, &pVertInflu->m_vNormal, &matCombined );

			//	Note : 출력값.
			//
			vVecOutput = vVecComb;
			vNorOutput = vVecNormal;
			if ( pBoneMatrice )		*pBoneMatrice = matCombined;			

			return S_OK;
		}

		return E_FAIL;
	}
	else
	{
		if ( !m_pSkinMesh )		return E_FAIL;

		DWORD dwNumBone = m_pSkinMesh->GetNumBones ();

		//	Note : 영향력을 주는 트랜스폼을 모두 합산하여 버텍스 값을 구한다.
		//
		for ( DWORD i=0; i<pVertInflu->m_dwNumBone; i++ )
		{
			DWORD iBone = pVertInflu->m_pBone[i];

			if ( dwNumBone <= iBone )	return E_FAIL;

			D3DXMatrixMultiply_MM( matCombined, m_pBoneOffsetMat[iBone], *m_pBoneMatrix[iBone] );

			D3DXVECTOR3 vVecWeight,vVecWeightNor;
			D3DXVec3TransformCoord ( &vVecWeight, &pVertInflu->m_vVector, &matCombined );
			vVecComb += vVecWeight*pVertInflu->m_pWeights[i];
			D3DXVec3TransformNormal ( &vVecWeightNor, &pVertInflu->m_vNormal, &matCombined );
			vVecNormal += vVecWeightNor*pVertInflu->m_pWeights[i];
		}

		//	Note : 출력값.
		//
		vVecOutput = vVecComb;
		D3DXVec3Normalize ( &vVecNormal, &vVecNormal );
		vNorOutput = vVecNormal;

		//	(주의) 영향을 받는 본의 갯수가 여러개일 경우에 문제가 생김.
		if ( pBoneMatrice )		*pBoneMatrice = matCombined;
	}

	return S_OK;
}

HRESULT SMeshContainer::DebugRender ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr;
	if ( !m_pMesh && !m_pSkinMesh )		return E_FAIL;

	if ( m_pSkinMesh )
	{
		if ( m_pSkinMesh->GetOptions()&D3DXMESH_32BIT )	return E_FAIL;

		DWORD dwNumFaces = m_pSkinMesh->GetNumFaces ();
		DWORD dwNumVertices = m_pSkinMesh->GetNumVertices ();
		
		DWORD dwSkinFVF = D3DFVF_XYZ;
		DWORD dwSkinVertSize = D3DXGetFVFVertexSize ( dwSkinFVF );

		PBYTE pbData = new BYTE[dwSkinVertSize*dwNumVertices];
		hr = UpdateSkinnedMesh ( pbData, dwSkinFVF );
		if ( FAILED(hr) )
		{
			SAFE_DELETE_ARRAY(pbData);
			return E_FAIL;
		}

		WORD *pIndexBuffer;
		hr = m_pSkinMesh->LockIndexBuffer ( D3DLOCK_READONLY, (BYTE**)&pIndexBuffer );
		if ( FAILED(hr) )
		{
			SAFE_DELETE_ARRAY(pbData);
			return E_FAIL;
		}

		DWORD dwCollisionFace = COL_ERR;
		D3DXVECTOR3 vDetect = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
		for ( DWORD j=0; j<dwNumFaces; j++ )
		{
			PWORD pwIndex = pIndexBuffer + 3*j;

			D3DXVECTOR3 *pVector0 = (D3DXVECTOR3*) ( pbData + dwSkinVertSize * ( *(pwIndex+0) ) );
			D3DXVECTOR3 *pVector1 = (D3DXVECTOR3*) ( pbData + dwSkinVertSize * ( *(pwIndex+1) ) );
			D3DXVECTOR3 *pVector2 = (D3DXVECTOR3*) ( pbData + dwSkinVertSize * ( *(pwIndex+2) ) );

			EDITMESHS::RENDERLINE ( pd3dDevice, *pVector0, *pVector1 );
			EDITMESHS::RENDERLINE ( pd3dDevice, *pVector1, *pVector2 );
			EDITMESHS::RENDERLINE ( pd3dDevice, *pVector2, *pVector0 );
		}

		SAFE_DELETE_ARRAY(pbData);
		m_pSkinMesh->UnlockIndexBuffer ();
	}

	return S_OK;
}

HRESULT SMeshContainer::CalculateBoundingBox ( LPDIRECT3DDEVICE8 pd3dDevice, LPD3DXMATRIX pBoneMatrices, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	HRESULT hr;
	if ( !m_pMesh && !m_pSkinMesh )		return E_FAIL;

	if ( m_pSkinMesh )
	{
		if ( m_pSkinMesh->GetOptions()&D3DXMESH_32BIT )	return E_FAIL;

		DWORD dwNumFaces = m_pSkinMesh->GetNumFaces ();
		DWORD dwNumVertices = m_pSkinMesh->GetNumVertices ();
		
		DWORD dwSkinFVF = D3DFVF_XYZ;
		DWORD dwSkinVertSize = D3DXGetFVFVertexSize ( dwSkinFVF );

		PBYTE pbData = new BYTE[dwSkinVertSize*dwNumVertices];
		hr = UpdateSkinnedMesh ( pbData, dwSkinFVF, pBoneMatrices );
		if ( FAILED(hr) )
		{
			SAFE_DELETE_ARRAY(pbData);
			return E_FAIL;
		}

		vMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
		vMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

		LPD3DXVECTOR3 pVector = (LPD3DXVECTOR3) pbData;
		for ( DWORD i=0; i<dwNumVertices; ++i )
		{
			if ( pVector[i].x > vMax.x )	vMax.x = pVector[i].x;
			if ( pVector[i].y > vMax.y )	vMax.y = pVector[i].y;
			if ( pVector[i].z > vMax.z )	vMax.z = pVector[i].z;

			if ( pVector[i].x < vMin.x )	vMin.x = pVector[i].x;
			if ( pVector[i].y < vMin.y )	vMin.y = pVector[i].y;
			if ( pVector[i].z < vMin.z )	vMin.z = pVector[i].z;
		}


		SAFE_DELETE_ARRAY(pbData);
		m_pSkinMesh->UnlockIndexBuffer ();
	}

	return S_OK;
}

HRESULT	SMeshContainer::GenerateMesh ( LPD3DXBUFFER pAdjacency )
{
	if ( !m_pMesh )	return E_FAIL;

	HRESULT hr = S_OK;

	if ( FAILED( hr = m_pMesh->OptimizeInplace(
						D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
						(DWORD*)pAdjacency->GetBufferPointer(), NULL, NULL, NULL ) ) )
	{
		return E_FAIL;
	}

	//// Normalize 한다.
	//DWORD dwPosFVF	= m_pMesh->GetFVF()&D3DFVF_POSITION_MASK;
	//UINT dwPosSize	= D3DXGetFVFVertexSize( dwPosFVF );
	//UINT nSize		= D3DXGetFVFVertexSize( m_pMesh->GetFVF() );
	//BYTE		*pVertices;
	//D3DXVECTOR3	*pVector;
	//m_pMesh->LockVertexBuffer( 0L, (BYTE**)&pVertices );
	//for( DWORD i=0; i<m_pMesh->GetNumVertices(); ++i )
	//{
	//	pVector = (D3DXVECTOR3*)(pVertices + (i*nSize) + dwPosSize);
	//	D3DXVec3Normalize( pVector, pVector );
	//}
	//m_pMesh->UnlockVertexBuffer();
	
	m_pMesh->GetAttributeTable ( NULL, &m_dwAttribTableSize );

	SAFE_DELETE_ARRAY ( m_pAttrTable );
	m_pAttrTable = new D3DXATTRIBUTERANGE [ m_dwAttribTableSize ];
	m_pMesh->GetAttributeTable( m_pAttrTable, NULL );

	return S_OK;
}

//	Note : 디바이스 (Device) 해제에 문제가 있음. (2002.12.03), JDH
//
HRESULT SMeshContainer::GenerateMesh ( METHOD method )
{
	if ( !m_pSkinMesh )	return E_FAIL;

	HRESULT hr = S_OK;
	LPDIRECT3DDEVICE8 pDevice = NULL;
	DWORD cFaces = m_pSkinMesh->GetNumFaces();

	hr  = m_pSkinMesh->GetDevice(&pDevice);
	if (FAILED(hr))		goto e_Exit;

	D3DCAPS8 d3dCaps;
	pDevice->GetDeviceCaps ( &d3dCaps );
	DWORD VertexShaderVersion = d3dCaps.VertexShaderVersion;
	DWORD MaxVertexBlendMatrices = d3dCaps.MaxVertexBlendMatrices;
	DWORD MaxVertexBlendMatrixIndex = d3dCaps.MaxVertexBlendMatrixIndex;


	GXRELEASE(m_pMesh);
	if ( method == D3DNONINDEXED )
	{
		LPD3DXBONECOMBINATION   rgBoneCombinations;

		hr = m_pSkinMesh->ConvertToBlendedMesh
		(
			D3DXMESH_MANAGED | D3DXMESHOPT_VERTEXCACHE, 
			m_rgiAdjacency, 
			NULL, 
			&m_cpattr, 
			&m_pBoneCombinationBuf, 
			NULL,
			NULL,
			&m_pMesh
		);
		if (FAILED(hr))		goto e_Exit;

		//// Normalize 한다.
		//DWORD dwPosFVF	= m_pMesh->GetFVF()&D3DFVF_POSITION_MASK;
		//UINT dwPosSize	= D3DXGetFVFVertexSize( dwPosFVF );
		//UINT nSize		= D3DXGetFVFVertexSize( m_pMesh->GetFVF() );
		//BYTE		*pVertices;
		//D3DXVECTOR3	*pVector;
		//m_pMesh->LockVertexBuffer( 0L, (BYTE**)&pVertices );
		//for( DWORD i=0; i<m_pMesh->GetNumVertices(); ++i )
		//{
		//	pVector = (D3DXVECTOR3*)(pVertices + (i*nSize) + dwPosSize);
		//	D3DXVec3Normalize( pVector, pVector );
		//}
		//m_pMesh->UnlockVertexBuffer();


		//	Note : 면에 가장 많이 영향을 받는 본의 갯수를 측정.
		//
		if ( (m_pMesh->GetFVF()&D3DFVF_POSITION_MASK) != D3DFVF_XYZ )
		{
			DWORD thisFVF = m_pMesh->GetFVF();
			m_maxFaceInfl = 1 + (( thisFVF & D3DFVF_POSITION_MASK) - D3DFVF_XYZRHW) / 2;
		}
		else
		{
			m_maxFaceInfl = 1;
		}

		//	Note : 만약 디바이스가 두개 이하의 매트릭스 블랜드를 지원한다면
		//       HW vertex processing과 SW vertex processing 으로 분리 하여 처리.
		//
		if ( MaxVertexBlendMatrices <= 2 )
		{
			// calculate the index of the attribute table to split on
			rgBoneCombinations  = reinterpret_cast<LPD3DXBONECOMBINATION>(m_pBoneCombinationBuf->GetBufferPointer());

			for ( m_iAttrSplit=0; m_iAttrSplit<m_cpattr; m_iAttrSplit++ )
			{
				DWORD cInfl = 0;
				for ( DWORD iInfl = 0; iInfl < m_maxFaceInfl; iInfl++ )
				{
					if ( rgBoneCombinations[m_iAttrSplit].BoneId[iInfl] != UINT_MAX )
					{
						++cInfl;
					}
				}

				if ( cInfl > MaxVertexBlendMatrices )	break;
			}

			//	Note : D3DXMESH_SOFTWAREPROCESSING flag 를 삽입하여 새로이 메쉬를 만듬..
			//		if there is both HW and SW, add the Software Processing flag
			//
			if ( m_iAttrSplit < m_cpattr )
			{
				LPD3DXMESH pMeshTmp;

				hr = m_pMesh->CloneMeshFVF
				(
					D3DXMESH_MANAGED | D3DXMESH_SOFTWAREPROCESSING | m_pMesh->GetOptions(),
					m_pMesh->GetFVF(),
					pDevice, &pMeshTmp
				);
				if (FAILED(hr))		goto e_Exit;

				m_pMesh->Release();
				m_pMesh = pMeshTmp;
				pMeshTmp = NULL;
			}
		}
		else
		{
			m_iAttrSplit = m_cpattr;
		}
	}
	else if (method == D3DINDEXEDVS)
    {
		hr = m_pSkinMesh->GenerateSkinnedMesh
		(
			D3DXMESH_SYSTEMMEM | D3DXMESH_WRITEONLY,		// options
			0.0f,					// minimum bone weight allowed
			m_rgiAdjacency,			// adjacency of in-mesh
			NULL,					// adjacency of out-mesh
			NULL,					// face remap array
			NULL,					// vertex remap buffer
			&m_pMesh				// out-mesh
		);
		if (FAILED(hr))		goto e_Exit;

		// Normalize 한다.
		DWORD dwPosFVF	= m_pMesh->GetFVF()&D3DFVF_POSITION_MASK;
		UINT dwPosSize	= D3DXGetFVFVertexSize( dwPosFVF );
		UINT nSize		= D3DXGetFVFVertexSize( m_pMesh->GetFVF() );
		BYTE		*pVertices;
		D3DXVECTOR3	*pVector;
		m_pMesh->LockVertexBuffer( 0L, (BYTE**)&pVertices );
		for( DWORD i=0; i<m_pMesh->GetNumVertices(); ++i )
		{
			pVector = (D3DXVECTOR3*)(pVertices + (i*nSize) + dwPosSize);
			D3DXVec3Normalize( pVector, pVector );
		}
		m_pMesh->UnlockVertexBuffer();

		hr = m_pMesh->GetAttributeTable ( NULL, &m_cpattr );
		if (FAILED(hr))		goto e_Exit;

		SAFE_DELETE_ARRAY(m_pAttrTable);
		m_pAttrTable  = new D3DXATTRIBUTERANGE[m_cpattr];
		if ( m_pAttrTable == NULL )
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		hr = m_pMesh->GetAttributeTable ( m_pAttrTable, NULL );
		if (FAILED(hr))		goto e_Exit;

		hr = m_pSkinMesh->GetMaxFaceInfluences ( &m_maxFaceInfl );
		if (FAILED(hr))		goto e_Exit;
		
        // Get palette size
        m_paletteSize = min(23, m_pSkinMesh->GetNumBones());	// 69 개 먹는다.

        DWORD flags = D3DXMESHOPT_VERTEXCACHE;
        if (VertexShaderVersion >= D3DVS_VERSION(1, 1))
        {
            m_bUseSW = false;
            flags |= D3DXMESH_MANAGED;
        }
        else
        {
            m_bUseSW = true;
            flags |= D3DXMESH_SYSTEMMEM;
        }

        hr = m_pSkinMesh->ConvertToIndexedBlendedMesh
		(
			flags, m_rgiAdjacency, m_paletteSize, NULL, &m_cpattr, &m_pBoneCombinationBuf,
			NULL, NULL, &m_pMesh
		);
        if (FAILED(hr))		goto e_Exit;

        if ((m_pMesh->GetFVF() & D3DFVF_POSITION_MASK) != D3DFVF_XYZ)
        {
            m_maxFaceInfl = ((m_pMesh->GetFVF() & D3DFVF_POSITION_MASK) - D3DFVF_XYZRHW) / 2;
        }
        else
        {
            m_maxFaceInfl = 1;
        }

        // FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
        DWORD newFVF = (m_pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE3(1) | D3DFVF_TEXCOORDSIZE3(2) | D3DFVF_LASTBETA_UBYTE4;
        if (newFVF != m_pMesh->GetFVF())
        {
            LPD3DXMESH pMesh;
            hr = m_pMesh->CloneMeshFVF(m_pMesh->GetOptions(), newFVF, pDevice, &pMesh);
            if (!FAILED(hr))
            {
                m_pMesh->Release();
                m_pMesh = pMesh;
                pMesh = NULL;
            }

			//struct VERTEX
			//{
			//	D3DXVECTOR3	vPos, vNor;
			//	float		fBlend;
			//	DWORD		dwColor;
			//	D3DXVECTOR2	vTex1;
			//	D3DXVECTOR3	vTex2, vTex3;
			//};
			hr = D3DXComputeTangent ( m_pMesh, 0, m_pMesh, 1, 2, TRUE, NULL );		// 접선 U,V 벡터를 만든다.
        }
    }
	else if ( method == D3DINDEXED )
	{
		hr = m_pSkinMesh->GenerateSkinnedMesh
		(
			D3DXMESH_SYSTEMMEM | D3DXMESH_WRITEONLY,		// options
			0.0f,					// minimum bone weight allowed
			m_rgiAdjacency,			// adjacency of in-mesh
			NULL,					// adjacency of out-mesh
			NULL,					// face remap array
			NULL,					// vertex remap buffer
			&m_pMesh				// out-mesh
		);
		if (FAILED(hr))		goto e_Exit;

		// Normalize 한다.
		DWORD dwPosFVF	= m_pMesh->GetFVF()&D3DFVF_POSITION_MASK;
		UINT dwPosSize	= D3DXGetFVFVertexSize( dwPosFVF );
		UINT nSize		= D3DXGetFVFVertexSize( m_pMesh->GetFVF() );
		BYTE		*pVertices;
		D3DXVECTOR3	*pVector;
		m_pMesh->LockVertexBuffer( 0L, (BYTE**)&pVertices );
		for( DWORD i=0; i<m_pMesh->GetNumVertices(); ++i )
		{
			pVector = (D3DXVECTOR3*)(pVertices + (i*nSize) + dwPosSize);
			D3DXVec3Normalize( pVector, pVector );
		}
		m_pMesh->UnlockVertexBuffer();

		hr = m_pMesh->GetAttributeTable ( NULL, &m_cpattr );
		if (FAILED(hr))		goto e_Exit;

		SAFE_DELETE_ARRAY(m_pAttrTable);
		m_pAttrTable  = new D3DXATTRIBUTERANGE[m_cpattr];
		if ( m_pAttrTable == NULL )
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		hr = m_pMesh->GetAttributeTable ( m_pAttrTable, NULL );
		if (FAILED(hr))		goto e_Exit;

		hr = m_pSkinMesh->GetMaxFaceInfluences ( &m_maxFaceInfl );
		if (FAILED(hr))		goto e_Exit;

		DWORD maxFaceInfl;
		DWORD flags = D3DXMESHOPT_VERTEXCACHE;

		hr = m_pSkinMesh->GetMaxFaceInfluences ( &maxFaceInfl );
		if (FAILED(hr))		goto e_Exit;

		// 12 entry palette guarantees that any triangle (4 independent influences per vertex of a tri)
		// can be handled
		maxFaceInfl = min ( maxFaceInfl, 12 );

		if ( MaxVertexBlendMatrixIndex + 1 < maxFaceInfl )
		{
			// HW does not support indexed vertex blending. Use SW instead
			//m_paletteSize = min ( 256, m_pSkinMesh->GetNumBones() );				// 원본
			m_paletteSize = min(23, m_pSkinMesh->GetNumBones());					// 69 개 먹는다.
			m_bUseSW = true;
			flags |= D3DXMESH_SYSTEMMEM;
		}
		else
		{
			//m_paletteSize = min ( MaxVertexBlendMatrixIndex + 1, m_pSkinMesh->GetNumBones() );	// 원본
			m_paletteSize = min(23, m_pSkinMesh->GetNumBones());									// 69 개 먹는다.

			//m_bUseSW = false;
			//flags |= D3DXMESH_MANAGED;
			
			if (VertexShaderVersion >= D3DVS_VERSION(1, 1))
			{
				m_bUseSW = false;
				flags |= D3DXMESH_MANAGED;
			}
			else
			{
				m_bUseSW = true;
				flags |= D3DXMESH_SYSTEMMEM;
			}
		}

		hr = m_pSkinMesh->ConvertToIndexedBlendedMesh ( flags, m_rgiAdjacency, m_paletteSize, NULL,
			&m_cpattr, &m_pBoneCombinationBuf, NULL, NULL, &m_pMesh );
		if (FAILED(hr))		goto e_Exit;

		// Here we are talking of max vertex influence which we determine from 
		// the FVF of the returned mesh
		if ( (m_pMesh->GetFVF() & D3DFVF_POSITION_MASK) != D3DFVF_XYZ )
		{
			m_maxFaceInfl = ((m_pMesh->GetFVF() & D3DFVF_POSITION_MASK) - D3DFVF_XYZRHW) / 2;
		}
		else
		{
			m_maxFaceInfl = 1;
		}

		DWORD newFVF = (m_pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
        if (newFVF != m_pMesh->GetFVF())
        {
            LPD3DXMESH pMesh;
            hr = m_pMesh->CloneMeshFVF(m_pMesh->GetOptions(), newFVF, pDevice, &pMesh);
            if (!FAILED(hr))
            {
                m_pMesh->Release();
                m_pMesh = pMesh;
                pMesh = NULL;
            }
        }
	}
	else if ( method == SOFTWARE )
	{
		hr = m_pSkinMesh->GenerateSkinnedMesh
		(
			D3DXMESH_SYSTEMMEM | D3DXMESH_WRITEONLY,		// options
			0.0f,					// minimum bone weight allowed
			m_rgiAdjacency,			// adjacency of in-mesh
			NULL,					// adjacency of out-mesh
			NULL,					// face remap array
			NULL,					// vertex remap buffer
			&m_pMesh				// out-mesh
		);
		if (FAILED(hr))		goto e_Exit;

		//// Normalize 한다.
		//DWORD dwPosFVF	= m_pMesh->GetFVF()&D3DFVF_POSITION_MASK;
		//UINT dwPosSize	= D3DXGetFVFVertexSize( dwPosFVF );
		//UINT nSize		= D3DXGetFVFVertexSize( m_pMesh->GetFVF() );
		//BYTE		*pVertices;
		//D3DXVECTOR3	*pVector;
		//m_pMesh->LockVertexBuffer( 0L, (BYTE**)&pVertices );
		//for( DWORD i=0; i<m_pMesh->GetNumVertices(); ++i )
		//{
		//	pVector = (D3DXVECTOR3*)(pVertices + (i*nSize) + dwPosSize);
		//	D3DXVec3Normalize( pVector, pVector );
		//}
		//m_pMesh->UnlockVertexBuffer();

		hr = m_pMesh->GetAttributeTable ( NULL, &m_cpattr );
		if (FAILED(hr))		goto e_Exit;

		SAFE_DELETE_ARRAY(m_pAttrTable);
		m_pAttrTable  = new D3DXATTRIBUTERANGE[m_cpattr];
		if ( m_pAttrTable == NULL )
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		hr = m_pMesh->GetAttributeTable ( m_pAttrTable, NULL );
		if (FAILED(hr))		goto e_Exit;

		hr = m_pSkinMesh->GetMaxFaceInfluences ( &m_maxFaceInfl );
		if (FAILED(hr))		goto e_Exit;
	}
	
	m_Method = method;

//	GenerateEffMesh ( pDevice );

e_Exit:
	GXRELEASE(pDevice);

	return hr;
}

HRESULT SMeshContainer::GenerateEffMesh ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr = S_OK;
	if ( !m_pMesh && !m_pSkinMesh )	return S_FALSE;

	if ( m_pSkinMesh )
	{
		GXRELEASE(m_pEffVBuffer);
		SAFE_DELETE_ARRAY(m_pBoneInfluences);

		DWORD dwNumBones = m_pSkinMesh->GetNumBones ();
		m_pBoneInfluences = new SBoneInfluence[dwNumBones];
	
		for ( DWORD i=0; i<dwNumBones; i++ )
		{
			DWORD dwInfue = m_pSkinMesh->GetNumBoneInfluences (i);
			if ( dwInfue==0 )	continue;

			m_pBoneInfluences[i].SetInfue(dwInfue);
			m_pSkinMesh->GetBoneInfluence ( i, m_pBoneInfluences[i].pVertices, m_pBoneInfluences[i].pWeights );
		}

		DWORD dwNumVertices = m_pSkinMesh->GetNumVertices ();
		m_dwEffNumVerts = dwNumVertices;

		hr = pd3dDevice->CreateVertexBuffer( m_dwEffNumVerts * sizeof(D3DXSKINEFFVERTEX),		// 성환
			D3DUSAGE_WRITEONLY , D3DXSKINEFFVERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pEffVBuffer );
		if ( FAILED(hr) )
		{
			CDebugSet::ToLogFile( "SMeshContainer::GenerateEffMesh() -- CreateVertexBuffer() -- Failed" );
			return E_FAIL;
		}

		D3DXSKINEFFVERTEX *pEffVertex;
		hr = m_pEffVBuffer->Lock ( 0, 0, (BYTE**)&pEffVertex, NULL );
		if ( FAILED(hr) )
		{
			CDebugSet::ToLogFile( "SMeshContainer::GenerateEffMesh() -- Lock -- Failed" );
			return E_FAIL;
		}

		D3DXSKINVERTEX *pOrgVertex = NULL;
		hr = m_pSkinMesh->LockVertexBuffer ( D3DLOCK_READONLY, (BYTE**)&pOrgVertex );
		if ( FAILED(hr) )
		{
			m_pEffVBuffer->Unlock ();
			CDebugSet::ToLogFile( "SMeshContainer::GenerateEffMesh() -- LockVertexBuffer -- Failed" );
			return E_FAIL;
		}

		for ( DWORD i=0; i<dwNumVertices; i++ )
		{
			pEffVertex[i].vPos		= pOrgVertex[i].vPos;
			pEffVertex[i].vNormal	= pOrgVertex[i].vNormal;
			pEffVertex[i].tu0		= pOrgVertex[i].tu0;
			pEffVertex[i].tv0		= pOrgVertex[i].tv0;
			pEffVertex[i].tu1		= pOrgVertex[i].tu0;
			pEffVertex[i].tv1		= pOrgVertex[i].tu0;
		//	pEffVertex[i].tw1		= 0.0f;
		}

		m_pEffVBuffer->Unlock ();
		m_pSkinMesh->UnlockVertexBuffer ();
	}
	else if ( m_pMesh )
	{
		DWORD dwNumVertices = m_pMesh->GetNumVertices ();
		m_dwEffNumVerts = dwNumVertices;

		hr = pd3dDevice->CreateVertexBuffer( m_dwEffNumVerts * sizeof(D3DXSKINEFFVERTEX),
			D3DUSAGE_WRITEONLY , D3DXSKINEFFVERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pEffVBuffer );
		if ( FAILED(hr) )
		{
			CDebugSet::ToLogFile( "SMeshContainer::GenerateEffMesh() -- CreateVertexBuffer -- Failed" );
			return E_FAIL;
		}

		D3DXSKINEFFVERTEX *pEffVertex;
		hr = m_pEffVBuffer->Lock ( 0, 0, (BYTE**)&pEffVertex, NULL );
		if ( FAILED(hr) )
		{
			CDebugSet::ToLogFile( "SMeshContainer::GenerateEffMesh() -- Lock -- Failed" );
			return E_FAIL;
		}

		D3DXSKINVERTEX *pOrgVertex = NULL;
		hr = m_pMesh->LockVertexBuffer ( D3DLOCK_READONLY, (BYTE**)&pOrgVertex );
		if ( FAILED(hr) )
		{
			m_pEffVBuffer->Unlock ();
			CDebugSet::ToLogFile( "SMeshContainer::GenerateEffMesh() -- LockVertexBuffer -- Failed" );
			return E_FAIL;
		}

		for ( DWORD i=0; i<dwNumVertices; i++ )
		{
			pEffVertex[i].vPos		= pOrgVertex[i].vPos;
			pEffVertex[i].vNormal	= pOrgVertex[i].vNormal;
			pEffVertex[i].tu0		= pOrgVertex[i].tu0;
			pEffVertex[i].tv0		= pOrgVertex[i].tv0;
			pEffVertex[i].tu1		= pOrgVertex[i].tu0;
			pEffVertex[i].tv1		= pOrgVertex[i].tv0;
		//	pEffVertex[i].tw1		= 1.f;
		}

		m_pEffVBuffer->Unlock ();
		m_pMesh->UnlockVertexBuffer ();
	}

	return S_OK;
}

HRESULT SMeshContainer::UpdateEffMesh ( LPD3DXMATRIX pBoneMatrices )
{
	HRESULT hr;
	if ( !m_pEffVBuffer )	return E_FAIL;

	D3DXSKINEFFVERTEX *pEffVertex;
	hr = m_pEffVBuffer->Lock ( 0, 0, (BYTE**)&pEffVertex, NULL );
	if ( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "SMeshContainer::UpdateEffMesh() -- Lock() -- Failed" );
		return E_FAIL;
	}

	hr = UpdateSkinnedMesh ( (PBYTE)pEffVertex, D3DXSKINEFFVERTEX::FVF, pBoneMatrices );
	if ( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "SMeshContainer::UpdateEffMesh() -- UpdateSkinnedMesh() -- Failed" );
		return E_FAIL;
	}

	m_pEffVBuffer->Unlock ();

	return S_OK;
}

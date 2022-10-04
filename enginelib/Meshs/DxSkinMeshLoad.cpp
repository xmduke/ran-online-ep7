#include "stdafx.h"

#include "DxMethods.h"
#include "../DxCommon/TextureManager.h"
#include "DxBoneCollector.h"
#include "DxSkinMeshMan.h"

#include "DxSkinMesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//	Note : 모든 프레임의 버텍스 백터 값들을 합산.
//		재귀적 호출로 모든 프레임을 합산.
//		( 이 개체의 구심점을 찾기 위해 사용됨. )
//
HRESULT CalculateSum (
					SFrame *pframe,			//	계산할 현제 프레임.
					D3DXMATRIX *pmatCur,	//	부모 프레임의 트렌스폼 메트릭스.
					D3DXVECTOR3 *pvCenter,	//	여기에 백터 값을 합산.
					UINT *pcVertices )		//	합산된 백터의 갯수 측정.
{
	HRESULT hr = S_OK;
	PBYTE pbPoints = NULL;
	UINT cVerticesLocal = 0;
	PBYTE pbCur;
	D3DXVECTOR3 *pvCur;
	D3DXVECTOR3 vTransformedCur;
	UINT iPoint;
	SMeshContainer *pmcCur;
	SFrame *pframeCur;
	UINT cVertices;
	D3DXMATRIXA16 matLocal;
    
	//	Note : 현제 프레임의 트렌스폼 메트릭스.
	//
	//D3DXMatrixMultiply ( &matLocal, &pframe->matRot, pmatCur );
	D3DXMatrixMultiply_MM ( matLocal, pframe->matRot, *pmatCur );
    
	pmcCur = pframe->pmcMesh;
	while (pmcCur != NULL)
	{
		//	Note : 버텍스의 성분을 분석해 메모리 할당량을 계산.
		//
		DWORD fvfsize = D3DXGetFVFVertexSize ( pmcCur->m_pMesh->GetFVF() );
        
		cVertices = pmcCur->m_pMesh->GetNumVertices();
        
		hr = pmcCur->m_pMesh->LockVertexBuffer(0, &pbPoints);
		if (FAILED(hr))
			goto e_Exit;
        
		for ( iPoint=0, pbCur = pbPoints; iPoint < cVertices; iPoint++, pbCur += fvfsize)
		{
			pvCur = (D3DXVECTOR3*)pbCur;
            
			if ( (pvCur->x != 0.0) || (pvCur->y != 0.0) || (pvCur->z != 0.0) )
			{
				cVerticesLocal++;
                
				//	Note : 백터를 트렌스폼 한후에 합산.
				//
				D3DXVec3TransformCoord ( &vTransformedCur, pvCur, &matLocal );
                
				pvCenter->x += vTransformedCur.x;
				pvCenter->y += vTransformedCur.y;
				pvCenter->z += vTransformedCur.z;
			}
		}

		pmcCur->m_pMesh->UnlockVertexBuffer();
		pbPoints = NULL;

		pmcCur = pmcCur->m_pmcNext;
	}
    
	//	Note : 이 프레임에서 합산된 백터수를 더함.
	//
	*pcVertices += cVerticesLocal;
    
	//	Note : 자식 프레임을 제귀호출로 각각 백터 값을 합산.
	//
	pframeCur = pframe->pframeFirstChild;
	while ( pframeCur != NULL )
	{
		hr = CalculateSum ( pframeCur, &matLocal, pvCenter, pcVertices );
		if (FAILED(hr))
			goto e_Exit;
        
		pframeCur = pframeCur->pframeSibling;
	}
    
e_Exit:
	//	Note : 에러 발생시에 버텍스 버퍼 lock이 되어 있을 때를 대비.
	//
	if ( pbPoints != NULL )
	{
		pmcCur->m_pMesh->UnlockVertexBuffer();
	}
    
	return hr;
}


//	Note : 개체의 반경을 구함.
//
HRESULT CalculateRadius ( 
						SFrame *pframe,			//	계산할 프레임.
						D3DXMATRIX *pmatCur,	//	부모 프레임의 트렌스폼 메트릭스.
						D3DXVECTOR3 *pvCenter,	//	개체의 중심점.
                        float *pfRadiusSq)		//	반경.
{
	HRESULT hr = S_OK;
	PBYTE pbPoints = NULL;
	PBYTE pbCur;
	D3DXVECTOR3 *pvCur;
	D3DXVECTOR3 vDist;;
	UINT iPoint;
	UINT cVertices;
	SMeshContainer *pmcCur;
	SFrame *pframeCur;
	float fRadiusLocalSq;
	float fDistSq;
	D3DXMATRIXA16 matLocal;
    
	//	Note : 현제 프레임의 트렌스폼 메트릭스.
	//
	//D3DXMatrixMultiply ( &matLocal, &pframe->matRot, pmatCur );
	D3DXMatrixMultiply_MM ( matLocal, pframe->matRot, *pmatCur );
    
	pmcCur = pframe->pmcMesh;
	fRadiusLocalSq = *pfRadiusSq;	//	종전 반경으로 세팅.
	while ( pmcCur != NULL )
	{
		//	Note : 버텍스의 성분을 분석해 메모리 할당량을 계산.
		//
		DWORD fvfsize = D3DXGetFVFVertexSize ( pmcCur->m_pMesh->GetFVF() );
        
		cVertices = pmcCur->m_pMesh->GetNumVertices();
        
		hr = pmcCur->m_pMesh->LockVertexBuffer ( 0, &pbPoints );
		if (FAILED(hr))
			goto e_Exit;
        
		for ( iPoint=0, pbCur = pbPoints; iPoint < cVertices; iPoint++, pbCur += fvfsize )
		{
			pvCur = (D3DXVECTOR3*)pbCur;
            
			if ( (pvCur->x == 0.0) && (pvCur->y == 0.0) && (pvCur->z == 0.0) )
				continue;
            
			//	Note : 백터를 트렌스폼 한후에 개체 중심점에서의 멀리 떨어져 있다면 반경값을 갱신.
			//
			D3DXVec3TransformCoord ( &vDist, pvCur, &matLocal );
            
			vDist -= *pvCenter;
            
			fDistSq = D3DXVec3LengthSq ( &vDist );
            
			if ( fDistSq > fRadiusLocalSq )
			fRadiusLocalSq = fDistSq;
		}
        
        
		pmcCur->m_pMesh->UnlockVertexBuffer();
		pbPoints = NULL;
        
		pmcCur = pmcCur->m_pmcNext;
	}
    
	//	Note : 새로이 찾은 반경으로.
	//
	*pfRadiusSq = fRadiusLocalSq;
    
	//	Note : 자식 프레임을 제귀호출로 반경값을 찾음.
	//
	pframeCur = pframe->pframeFirstChild;
	while ( pframeCur != NULL )
	{
		hr = CalculateRadius ( pframeCur, &matLocal, pvCenter, pfRadiusSq );
		if (FAILED(hr))
			goto e_Exit;
        
		pframeCur = pframeCur->pframeSibling;
	}
    
e_Exit:
	//	Note : 에러 발생시에 버텍스 버퍼 lock이 되어 있을 때를 대비.
	//
	if (pbPoints != NULL)
	{
		pmcCur->m_pMesh->UnlockVertexBuffer();
	}
    
	return hr;
}

//	Note : 개체의 경계구 계산.
//
HRESULT DxSkinMesh::CalculateBoundingSphere ()
{
	HRESULT hr = S_OK;
	D3DXVECTOR3 vCenter(0,0,0);
	UINT cVertices = 0;
	float fRadiusSq = 0;
	D3DXMATRIXA16 matCur;
    
	//	Note : 중심점을 찾기 위해 모든 백터를 합산. 
	//
	D3DXMatrixIdentity (&matCur);
	hr = CalculateSum ( pframeRoot, &matCur, &vCenter, &cVertices );
	if (FAILED(hr))				goto e_Exit;
    
	if (cVertices > 0)
	{
		//	Note : 중심점 값으로.
		//
		vCenter /= (float)cVertices;
        
		//	Note : 줌심점에서 최대 거리에 있는 백터를 찾아 반경 값을 가져온다.
		//
		D3DXMatrixIdentity (&matCur);
		hr = CalculateRadius ( pframeRoot, &matCur, &vCenter, &fRadiusSq );
		if (FAILED(hr))			goto e_Exit;
	}
    
	fRadius = (float)sqrt((double)fRadiusSq); // square 값이므로.
	vCenter = vCenter;

e_Exit:
	return hr;
}

HRESULT DxSkinMesh::CalculateBoundingBox ( LPDIRECT3DDEVICE8 pd3dDevice, SMeshContainer* pmcMesh, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	HRESULT hr(S_OK);
	if ( !pmcMesh->m_pSkinMesh )	return E_FAIL;

	DWORD cBones  = pmcMesh->m_pSkinMesh->GetNumBones();

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

	hr = pmcMesh->CalculateBoundingBox ( pd3dDevice, m_pBoneMatrices, vMax, vMin );

	return hr;
}

//	Note : 개체 프레임들에서 스킨메쉬를 가지고 있다면
//		해당 메쉬에 작용하는 메트릭스 포인터를 보관.
//
HRESULT DxSkinMesh::FindBones ( SFrame *pframeCur )
{
	HRESULT hr = S_OK;
	SFrame *pframeChild;
	SMeshContainer *pmcMesh;
    
	pmcMesh = pframeCur->pmcMesh;
	while (pmcMesh != NULL)
    {
		if ( pmcMesh->m_pSkinMesh && !pmcMesh->m_szBoneNames.empty() )
		{
			//	Note : 이 메쉬 콘테이너에 연결된 '본 이름' 가져옴.
			//

			//	NEED : 연결된 본을 찾아서 해당 메트릭스 포인터를 보관.
			//
			for ( DWORD i = 0; i < pmcMesh->m_pSkinMesh->GetNumBones(); ++i )
			{
				DxBoneTrans* pBone = pSkeleton->FindBone ( pmcMesh->m_szBoneNames[i].c_str() );
				if ( !pBone )	return E_FAIL;

				pmcMesh->m_pBoneMatrix[i] = &(pBone->matCombined);
			}
		}
		pmcMesh = pmcMesh->m_pmcNext;
	}
    
	//	Note : 자식들을 재귀적으로 모두 탐색.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while (pframeChild != NULL)
	{
		hr = FindBones ( pframeChild );
		if (FAILED(hr))
			return hr;
        
		pframeChild = pframeChild->pframeSibling;
	}
    
	return S_OK;
}

//	Note : xFile을 열어서 스킨메쉬를 생성.
//
HRESULT DxSkinMesh::LoadMeshHierarchy ( char *szFile, char *szSkeletonFile, LPDIRECT3DDEVICE8 pd3dDevice, const BOOL bThread )
{
	GASSERT(szFile);
	GASSERT(szSkeletonFile);

	StringCchCopy( m_szName, MAX_PATH, szFile );
	StringCchCopy( m_szSkeleton, MAX_PATH, szSkeletonFile );
	TCHAR* pszFile = m_szName;


	pSkeleton = DxBoneCollector::GetInstance().Load ( m_szSkeleton, pd3dDevice );
	if ( !pSkeleton )	return E_FAIL;


	HRESULT hr = S_OK;
	LPDIRECTXFILE pxofapi = NULL;
	LPDIRECTXFILEENUMOBJECT pxofenum = NULL;
	LPDIRECTXFILEDATA pxofobjCur = NULL;
	DWORD dwOptions;
	int cchFileName;

	if (pszFile == NULL)	return E_INVALIDARG;
    
	//	Note : 루트 만듬.
	//
	pframeRoot = new SFrame();
	if ( pframeRoot == NULL )
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}

	int nStrLen = strlen(__FRAMEROOT)+1;
	pframeRoot->szName = new char[nStrLen];
	StringCchCopy( pframeRoot->szName, nStrLen, __FRAMEROOT );

	dwOptions = 0;
  
	//	Note : xFile 열기.
	//
	cchFileName = strlen(pszFile);
	if (cchFileName < 2)
	{
		hr = E_FAIL;
		goto e_Exit;
	}
    
	//	xFile 개체를 만듬.
	hr = DirectXFileCreate(&pxofapi);
	if (FAILED(hr))	goto e_Exit;
    
    //	xFile 템플리트 등록.
	hr = pxofapi->RegisterTemplates((LPVOID)D3DRM_XTEMPLATES,
		D3DRM_XTEMPLATE_BYTES);
	if (FAILED(hr))	goto e_Exit;
    
	char szPathName[MAX_PATH] = "";
	StringCchCopy( szPathName, MAX_PATH, DxSkinMeshMan::GetInstance().GetPath() );
	StringCchCat( szPathName, MAX_PATH, pszFile );

    //	실제 xFile을 열음.
	hr = pxofapi->CreateEnumObject((LPVOID)szPathName,
		DXFILELOAD_FROMFILE,
		&pxofenum);
	if (FAILED(hr))	goto e_Exit;
    
    
	//	Note : 최상위 오브젝트를 읽어옴.
	//
	while (SUCCEEDED(pxofenum->GetNextDataObject(&pxofobjCur)))
	{
		hr = LoadFrames ( pxofobjCur, dwOptions, m_dwFVF, pd3dDevice, pframeRoot, bThread );
        
		GXRELEASE(pxofobjCur);
        
		if (FAILED(hr))	goto e_Exit;
	}

    //	Note : 스킨메쉬에 연결된 본들을 찾음.
	//
	hr = FindBones ( pframeRoot );
	if (FAILED(hr))	goto e_Exit;
    
	//	Note : 개체의 경계구 설정.
	//
    hr = CalculateBoundingSphere();
    if (FAILED(hr))	goto e_Exit;
    
	//	Note : 본의 트랜스폼 메트릭스 값을 사용할 배열 생성.
	//		( METHOD::SOFTWARE ), 일 경우에.
	//
	SAFE_DELETE_ARRAY(m_pBoneMatrices);
	m_pBoneMatrices = new D3DXMATRIX[m_maxBones];

e_Exit:
    GXRELEASE(pxofobjCur);
    GXRELEASE(pxofenum);
    GXRELEASE(pxofapi);
    
    if (FAILED(hr))
    {
        SAFE_DELETE(pframeRoot);
    }
    
    return hr;
}

//	Note : 메쉬를 읽음.
//
HRESULT DxSkinMesh::LoadMesh ( LPDIRECTXFILEDATA pxofobjCur,
								DWORD options,
								DWORD fvf,
								LPDIRECT3DDEVICE8 pD3DDevice,
                                SFrame *pframeParent, const BOOL bThread )
{
	HRESULT hr = S_OK;
	SMeshContainer *pmcMesh = NULL;
	LPD3DXBUFFER pbufMaterials = NULL;
	LPD3DXBUFFER pbufAdjacency = NULL;
	DWORD cchName;
	UINT cFaces;
	UINT iMaterial;
	LPDIRECT3DDEVICE8 m_pDevice = pD3DDevice;
	LPDWORD pAdjacencyIn;

	//	Note : 메쉬 콘데이너 생성.
	//	
	pmcMesh = new SMeshContainer();
	if (pmcMesh == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}

	//	Note : 이름 가져옴.
	//
	hr = pxofobjCur->GetName(NULL, &cchName);
	if (FAILED(hr))
		goto e_Exit;

	if (cchName > 0)
	{
		pmcMesh->m_szName = new char[cchName];
		if (pmcMesh->m_szName == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}
	    
		hr = pxofobjCur->GetName(pmcMesh->m_szName, &cchName);
		if (FAILED(hr))
			goto e_Exit;
	}
	else if( pframeParent->szName )
	{
		cchName = strlen ( pframeParent->szName ) + 1;
		pmcMesh->m_szName = new char[cchName];
		StringCchCopy( pmcMesh->m_szName, cchName, pframeParent->szName );
	}

	//	Note : 스킨메쉬 읽음.
	//
	LPD3DXBUFFER pBoneNamesBuf;

	hr = D3DXLoadSkinMeshFromXof ( pxofobjCur, options, pD3DDevice, &pbufAdjacency, &pbufMaterials, &pmcMesh->m_cMaterials,
		&pBoneNamesBuf, &pmcMesh->m_pBoneOffsetBuf, &pmcMesh->m_pSkinMesh );
	if (FAILED(hr))
	{
		if (hr == D3DXERR_LOADEDMESHASNODATA)
			hr = S_OK;
		goto e_Exit;
	}

	if ( pBoneNamesBuf )
	{
		if ( pmcMesh->m_pSkinMesh )
		{
			char** pBoneName = static_cast<char**>(pBoneNamesBuf->GetBufferPointer());

			DWORD dwNum = pmcMesh->m_pSkinMesh->GetNumBones();
			for ( DWORD i=0; i<dwNum; ++i )
			{
				pmcMesh->m_szBoneNames.push_back ( std::string(pBoneName[i]) );
			}
		}

		GXRELEASE(pBoneNamesBuf);
	}

	//	Note : 페이스 갯수.
	//
	cFaces = pmcMesh->m_pSkinMesh->GetNumFaces();

	//	Note : 페이스 인덱스.
	//
	pAdjacencyIn = static_cast<LPDWORD> ( pbufAdjacency->GetBufferPointer() );

	pmcMesh->m_rgiAdjacency = new DWORD[cFaces * 3];

	if (pmcMesh->m_rgiAdjacency == NULL)
	{
		hr = E_OUTOFMEMORY;

		goto e_Exit;
	}

	memcpy(pmcMesh->m_rgiAdjacency, pAdjacencyIn, cFaces * 3 * sizeof(DWORD));


	//	Note : 이 스킨 메쉬에 사용된 본의 갯수.
	//
	DWORD dwMeshNumBones = pmcMesh->m_pSkinMesh->GetNumBones();
	if ( dwMeshNumBones )
	{
		//	Note : 메쉬당 본의 갯수중 가장 큰것을 측정.
		//
		m_maxBones = max(dwMeshNumBones, m_maxBones);
	    
		//	Note : 본 메트릭스 지정. 메모리 배열만 할당한뒤 모두 로드한 다음 FindBones ()으로 메트릭스
		//		포인터를 찾아낸다.
		//
		pmcMesh->m_pBoneMatrix = new D3DXMATRIX*[dwMeshNumBones];
		if (pmcMesh->m_pBoneMatrix == NULL)		goto e_Exit;

		pmcMesh->m_pBoneOffsetMat = reinterpret_cast<D3DXMATRIX*>(pmcMesh->m_pBoneOffsetBuf->GetBufferPointer());
	    
		//// Normal 값을 단위벡터로 유지시키기 위한 작업
		//float fLength	= 1.f;
		//D3DXVECTOR3	vDir(1.f,0.f,0.f);
		//D3DXVECTOR3	vScale(1.f,0.f,0.f);
		//D3DXMATRIX matTemp;
		//for( DWORD i=0; i<dwMeshNumBones; ++i )
		//{
		//	D3DXMatrixInverse( &matTemp, NULL, &pmcMesh->m_pBoneOffsetMat[i] );
		//	D3DXMatrixTranspose( &matTemp, &matTemp );
		//	D3DXVec3TransformNormal( &vScale, &vDir, &matTemp );
		//	fLength = D3DXVec3Length( &vScale );
		//	fLength = 1.f/fLength;
		//	pmcMesh->m_pBoneOffsetMat[i] *= fLength;
		//}

		//	Note : GenerateMesh
		//
		hr = pmcMesh->GenerateMesh ( DxSkinMesh::m_method );
		if (FAILED(hr))		goto e_Exit;
	}
	else
	{
		//	Note : 스킨메쉬가 아닐 경우 일반 메쉬로.
		//
		pmcMesh->m_pSkinMesh->GetOriginalMesh(&(pmcMesh->m_pMesh));
		pmcMesh->m_pSkinMesh->Release();
		pmcMesh->m_pSkinMesh = NULL;
		pmcMesh->m_cpattr = pmcMesh->m_cMaterials;

		//	Note : GenerateMesh
		//
		hr = pmcMesh->GenerateMesh ( pbufAdjacency );
		if (FAILED(hr))		goto e_Exit;
	}

	//	Note : 메터리얼값이 없다면 기본 메터리얼 지정.
	//
	if ((pbufMaterials == NULL) || (pmcMesh->m_cMaterials == 0))
	{
		pmcMesh->m_pMaterials = new DXMATERIAL[1];

		if ( pmcMesh->m_pMaterials == NULL )
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}
	    
		memset ( &pmcMesh->m_pMaterials[0].d3dMaterial, 0, sizeof(D3DMATERIAL8) );
		pmcMesh->m_pMaterials[0].d3dMaterial.Diffuse.r = 0.5f;
		pmcMesh->m_pMaterials[0].d3dMaterial.Diffuse.g = 0.5f;
		pmcMesh->m_pMaterials[0].d3dMaterial.Diffuse.b = 0.5f;
		pmcMesh->m_pMaterials[0].d3dMaterial.Specular = pmcMesh->m_pMaterials[0].d3dMaterial.Diffuse;
		pmcMesh->m_pMaterials[0].d3dMaterial.Power = 14.f;	// 30.f - 디폴트

		pmcMesh->m_pMaterials[0].pTexture = NULL;
	}
	//	Note : 메터리얼 값이 있을 경우.
	//
	else
	{
		pmcMesh->m_pMaterials = new DXMATERIAL[pmcMesh->m_cMaterials];

		if ( pmcMesh->m_pMaterials == NULL )
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		LPD3DXMATERIAL pMaterials = (LPD3DXMATERIAL)pbufMaterials->GetBufferPointer();
	    
		for (iMaterial = 0; iMaterial < pmcMesh->m_cMaterials; ++iMaterial)
		{
			//	Note : 메터리얼 값 지정.
			//
			pmcMesh->m_pMaterials[iMaterial].d3dMaterial = pMaterials[iMaterial].MatD3D;
			pmcMesh->m_pMaterials[iMaterial].d3dMaterial.Power = 14.f;	// 30.f - 디폴트

			//	Note : Ambient 값을 Diffuse 값과 같게 해준다.
			pmcMesh->m_pMaterials[iMaterial].d3dMaterial.Ambient = pmcMesh->m_pMaterials[iMaterial].d3dMaterial.Diffuse;
	        
			//	Note : 텍스쳐 로드.
			//
			pmcMesh->m_pMaterials[iMaterial].pTexture = NULL;
			if (pMaterials[iMaterial].pTextureFilename != NULL)
			{
				pmcMesh->m_pMaterials[iMaterial].strTexture = std::string(pMaterials[iMaterial].pTextureFilename);

				hr = TextureManager::LoadTexture( pMaterials[iMaterial].pTextureFilename,
										m_pDevice, pmcMesh->m_pMaterials[iMaterial].pTexture, 0, 2, bThread );
				if (FAILED(hr))
				{
					hr = S_OK;
					pmcMesh->m_pMaterials[iMaterial].pTexture = NULL;
				}

				if( pmcMesh->m_pMaterials[iMaterial].pTexture )
				{
					//	Note : 텍스쳐 읽기에 성공시에 이 메터리얼의 초기 값을 결정.
					//
					TextureManager::EM_TEXTYPE emType = TextureManager::GetTexType( pMaterials[iMaterial].pTextureFilename );

					if( emType==TextureManager::EMTT_ALPHA_HARD )			pmcMesh->m_pMaterials[iMaterial].dwFlags = DXMATERIAL::FLAG_ALPHA_HARD;
					else if( emType==TextureManager::EMTT_ALPHA_SOFT )		pmcMesh->m_pMaterials[iMaterial].dwFlags = DXMATERIAL::FLAG_ALPHA_SOFT;
					else if( emType==TextureManager::EMTT_ALPHA_SOFT01 )	pmcMesh->m_pMaterials[iMaterial].dwFlags = DXMATERIAL::FLAG_ALPHA_SOFT;
					else if( emType==TextureManager::EMTT_ALPHA_SOFT02 )	pmcMesh->m_pMaterials[iMaterial].dwFlags = DXMATERIAL::FLAG_ALPHA_SOFT;
				}
			}
		}
	}

	//	Note : 새로운 메쉬 콘테이너를 삽입.
	//
	pframeParent->AddMesh (pmcMesh );	//	SFrame에 등록.
	AddMeshGlobal ( pmcMesh );			//	DxSkinMesh에 등록.
	pmcMesh = NULL;
   
e_Exit:
    SAFE_DELETE(pmcMesh);
    
    GXRELEASE(pbufAdjacency);
    GXRELEASE(pbufMaterials);

    return hr;
}


//	Note : Frame ( 메쉬, 트렌스폼, 에니메이션 ) 을 읽음.
//
HRESULT DxSkinMesh::LoadFrames ( LPDIRECTXFILEDATA pxofobjCur,
								DWORD options,
								DWORD fvf,
								LPDIRECT3DDEVICE8 pD3DDevice,
                                SFrame *pframeParent, const BOOL bThread )
{
    HRESULT hr = S_OK;
    LPDIRECTXFILEDATA pxofobjChild = NULL;
    LPDIRECTXFILEOBJECT pxofChild = NULL;
    const GUID *type;
    DWORD cbSize;
    D3DXMATRIX *pmatNew;
    SFrame *pframeCur;
    DWORD cchName;
    
    //	Note : 오브젝트의 타입.
	//
    hr = pxofobjCur->GetType(&type);
    if (FAILED(hr))
        goto e_Exit;
    
    //	Note : 메쉬.
	//
    if (*type == TID_D3DRMMesh)
    {
		SFrame* pframe = NULL;
		if ( pframeParent->szName == NULL )
		{
			pframe = FindParentFrame ( pframeParent );
			if ( pframe && pframe->szName != NULL )
			{
				char *szMesh = "[Mesh]";
				size_t nStrLen = strlen(pframe->szName)+strlen(szMesh)+1;
				pframeParent->szName = new char[ nStrLen ];
				memset( pframeParent->szName, 0, sizeof(char)*nStrLen );

				StringCchCat( pframeParent->szName, nStrLen, pframe->szName );
				StringCchCat( pframeParent->szName, nStrLen, szMesh );
			}
		}

		hr = LoadMesh( pxofobjCur, options, fvf, pD3DDevice, pframeParent, bThread );
        if (FAILED(hr))		goto e_Exit;

		if ( pframe )
		{
			//	Note : 이것은 MAX 에서 처럼.
			//		트랜스폼 프래임과 메쉬가 들어 있는 프래임이 쌍으로 있을때에 유효하게 만들어진
			//		코드이다.
			//		만약, 다르게 구성되어 있는게 사용되면 에러가 발생할 수 있다.
			//
			//	m_pmcMesh->m_szBoneNames[0];	//	[LOCAL]
			//	m_pmcMesh->m_szBoneNames[1];	//	[LOCAL]
			//	m_pmcMesh->m_szBoneNames[2];	//	[COMBINED] <-- 이것만 실제로 BONE에서 영향을 받는것.

			GASSERT(pframeParent);
			if ( pframeParent->pmcMesh && !pframeParent->pmcMesh->m_pSkinMesh )
			{
				SFrame* pBoneFrame = FindParentFrame ( pframe );
				if ( pBoneFrame )
				{
					DxBoneTrans* pBoneTrans = pSkeleton->FindBone ( pBoneFrame->szName );

					if ( pBoneTrans )
					{
						pframeParent->pmcMesh->m_pBoneMatrix = new LPD3DXMATRIX[3];
						pframeParent->pmcMesh->m_numBoneComb = 3;

						pframeParent->pmcMesh->m_szBoneNames.push_back ( std::string("[LOCAL]") );
						pframeParent->pmcMesh->m_pBoneMatrix[0] = &pframeParent->matRot;

						pframeParent->pmcMesh->m_szBoneNames.push_back ( std::string("[LOCAL]") );
						pframeParent->pmcMesh->m_pBoneMatrix[1] = &pframe->matRot;

						pframeParent->pmcMesh->m_szBoneNames.push_back ( std::string(pBoneFrame->szName) );
						pframeParent->pmcMesh->m_pBoneMatrix[2] = &pBoneTrans->matCombined;
					}
				}
			}
		}
	}
    //	Note : 트렌스폼.
	//
    else if (*type == TID_D3DRMFrameTransformMatrix)
    {
        hr = pxofobjCur->GetData ( NULL, &cbSize, (PVOID*)&pmatNew );
        if (FAILED(hr))
            goto e_Exit;
        
        // update the parents matrix with the new one
        pframeParent->matRot = *pmatNew;
        pframeParent->matRotOrig = *pmatNew;
    }
	//	Note : 하위 프레임.
	//
    else if (*type == TID_D3DRMFrame)
    {
		char *szframeName = NULL;
        
		//	Note : 프레임의 이름 가져옴.
		//
        hr = pxofobjCur->GetName(NULL, &cchName);
        if (FAILED(hr))
            goto e_Exit;
        
        if (cchName > 0)
        {
            szframeName = new char[cchName];
            if (szframeName == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
            
            hr = pxofobjCur->GetName(szframeName, &cchName);
            if (FAILED(hr))
                goto e_Exit;
        }

		if ( !strcmp(pframeParent->szName,__FRAMEROOT) )
		{
			pframeCur = pframeParent;

			//	Note : __FRAMEROOT 이름을 제거하고 현제 프레임 이름을 지정.
			//
			delete[] pframeCur->szName;
			pframeCur->szName = szframeName;
		}
		else
		{
       		//	Note : 새 프레임 만듬.
			//
			pframeCur = new SFrame();
			if (pframeCur == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto e_Exit;
			}

			pframeCur->szName = szframeName;

			pframeParent->AddFrame(pframeCur);
		}
        
		//	Note : 순차적으로 탐색하면서 에니메이션을 읽어온다.
		//		QueryInterface()를 사용하여 오브젝트의 타입을 확인함.
		//
        while ( SUCCEEDED(pxofobjCur->GetNextObject(&pxofChild)) )
        {
            hr = pxofChild->QueryInterface(IID_IDirectXFileData,
                (LPVOID *)&pxofobjChild);

            if (SUCCEEDED(hr))
            {
                hr = LoadFrames( pxofobjChild, options, fvf, pD3DDevice, pframeCur, bThread );
                if (FAILED(hr))
                    goto e_Exit;
                
                GXRELEASE(pxofobjChild);
            }
            
            GXRELEASE(pxofChild);
        }
        
    }
    
e_Exit:
    GXRELEASE(pxofobjChild);
    GXRELEASE(pxofChild);
    return hr;
}

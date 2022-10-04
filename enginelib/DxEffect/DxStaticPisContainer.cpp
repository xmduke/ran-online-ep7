#include "stdafx.h"

#include <algorithm>
#include "StlFunctions.h"

#include "SerialFile.h"
#include "SubPath.h"
#include "DxMethods.h"

#include "DxStaticPisContainer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

DxStaticPisContainer& DxStaticPisContainer::GetInstance()
{
	static DxStaticPisContainer Instance;
	return Instance;
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x			S	t	a	t	i	c	M	e	s	h
// -----------------------------------------------------------------------------------------------------------------------------------------
char DxStaticPis::m_szPath[] = "";

const DWORD DxStaticPis::VERSION = 0x0100;

DxStaticPis::DxStaticPis() :
	m_pFrameMesh(NULL),
	m_pStaticMesh(NULL),
	m_nRefCount(0)
{
	m_szFrameName = "";
}

DxStaticPis::~DxStaticPis()
{
	CleanUp();
}

void DxStaticPis::CleanUp()
{
	SAFE_DELETE ( m_pFrameMesh );
	SAFE_DELETE ( m_pStaticMesh );

	m_szFrameName = "";
}

HRESULT DxStaticPis::NewFrameMesh ( LPDIRECT3DDEVICE8 pd3dDevice, const char* szName )
{
	HRESULT hr = S_OK;

	SAFE_DELETE ( m_pFrameMesh );
	m_pFrameMesh = new DxFrameMesh;

	//	Note : 읽을 x파일과 버텍스 구조 정보 ( FVF ) 를 준다.
	m_szFrameName = szName;
	m_pFrameMesh->SetFile ( szName );
	m_pFrameMesh->SetFVF ( D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1 );

	//	Note : 초기화 한다.
	hr = m_pFrameMesh->Create ( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	m_pFrameMesh->UpdateFrames();

	m_vMax = m_pFrameMesh->GetTreeMax();
	m_vMin = m_pFrameMesh->GetTreeMin();

	return S_OK;
}

void DxStaticPis::MakeStaticMesh ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	if ( !m_pFrameMesh )	return;

	// Note : AABB_Octree 생성
	SAFE_DELETE ( m_pStaticMesh );
	m_pStaticMesh = new DxStaticMesh;
	m_pStaticMesh->MakeAABBOCTree ( pd3dDevice, m_pFrameMesh );
}

void DxStaticPis::Render ( const LPDIRECT3DDEVICE8 pd3dDevice, const CLIPVOLUME &_sCV, const D3DXMATRIX& matWorld )
{
	if ( !m_pStaticMesh )					return;

	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );

	CLIPVOLUME sCV;
	D3DXClipVolumeTransform ( sCV, _sCV, matWorld );

	if ( m_pStaticMesh )	
	{
		m_pStaticMesh->Render_DYNAMIC_( pd3dDevice, sCV, NULL );
		m_pStaticMesh->Render_DYNAMIC_Alpha( pd3dDevice, sCV, NULL );
		m_pStaticMesh->Render_DYNAMIC_SoftAlpha( pd3dDevice, sCV, NULL );
	}
}

void DxStaticPis::Render ( const LPDIRECT3DDEVICE8 pd3dDevice, const CLIPVOLUME &_sCV, const D3DXMATRIX& matWorld, const char* szName )
{
	if ( !m_pStaticMesh )				return;

	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );

	CLIPVOLUME sCV;
	D3DXClipVolumeTransform ( sCV, _sCV, matWorld );

	if ( m_pStaticMesh )	m_pStaticMesh->Render ( pd3dDevice, szName, sCV );
}

BOOL DxStaticPis::IsCollisionLine ( const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, D3DXMATRIX& matWorld )
{
	if ( !m_pStaticMesh )	return FALSE;

	D3DXMATRIX  matRotate;
	matRotate = matWorld;
	matRotate._41 = 0.f;
	matRotate._42 = 0.f;
	matRotate._43 = 0.f;

	D3DXVECTOR3 vTemp1 = vP1;
	D3DXVECTOR3 vTemp2 = vP2;

	vTemp1.x -= matWorld._41;
	vTemp1.y -= matWorld._42;
	vTemp1.z -= matWorld._43;
	vTemp2.x -= matWorld._41;
	vTemp2.y -= matWorld._42;
	vTemp2.z -= matWorld._43;

	D3DXVec3TransformCoord ( &vTemp1, &vTemp1, &matRotate );
	D3DXVec3TransformCoord ( &vTemp2, &vTemp2, &matRotate );

	const char* szName = NULL;
	if ( m_pStaticMesh->IsCollisionLine( vTemp1, vTemp2, vColl, vNor, szName ) )
	{
		D3DXVec3TransformCoord ( &vColl, &vColl, &matWorld );
		D3DXVec3TransformNormal ( &vNor, &vNor, &matWorld );

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void DxStaticPis::Save ( char* szName )
{
	CSerialFile	SFile;

	BOOL bOpened = SFile.OpenFile ( FOT_WRITE, szName );
	if ( !bOpened )
	{
		MessageBox ( NULL, "File Creation", "ERROR", MB_OK );
		return;
	}

	SFile << VERSION;

	SFile.BeginBlock();
	{
		SFile << m_vMin;
		SFile << m_vMax;
		SFile << m_szFrameName;
	}
	SFile.EndBlock();


	BOOL bExist;
	bExist = m_pStaticMesh ? TRUE : FALSE;
	SFile << bExist;
	if ( m_pStaticMesh )	m_pStaticMesh->Save( SFile );
}

HRESULT DxStaticPis::Load ( LPDIRECT3DDEVICE8 pd3dDevice, const char* szName, BOOL bEdit )
{
	CleanUp();		// 초기화

	if ( !strcmp(szName,"") )	return E_FAIL;
	
	CSerialFile	SFile;
	BOOL bOpened = SFile.OpenFile ( FOT_READ, szName );
	if ( !bOpened )
	{
		MessageBox ( NULL, "File Creation", "ERROR", MB_OK );
		return E_FAIL;
	}

	DWORD dwVer;
	DWORD dwBufferSize;

	SFile >> dwVer;
	SFile >> dwBufferSize;

	if( dwVer==VERSION )
	{
		SFile >> m_vMin;
		SFile >> m_vMax;
		SFile >> m_szFrameName;
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet( dwCur+dwBufferSize );
	}

	// Note : m_pStaticMesh 내부에서 Version 체크가 일어난다.
	BOOL bExist;
	SFile >> bExist;
	if ( bExist )
	{
		m_pStaticMesh = new DxStaticMesh;	
		m_pStaticMesh->Load ( pd3dDevice, SFile, FALSE );
	}


	// EDIT 시에만 FrameMesh를 로드한다.
	if ( bEdit )	NewFrameMesh ( pd3dDevice, m_szFrameName.c_str() );

	return S_OK;
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//							D	x			S	t	a	t	i	c	M	e	s	h			C	o	n	t	a	i	n	e	r
// -----------------------------------------------------------------------------------------------------------------------------------------
DxStaticPisContainer::DxStaticPisContainer()
{
}

DxStaticPisContainer::~DxStaticPisContainer()
{
	CleanUp ();
}

void DxStaticPisContainer::CleanUp ()
{
	std::for_each ( m_mapStaticPis.begin(), m_mapStaticPis.end(), std_afunc::DeleteMapObject() );
	m_mapStaticPis.clear ();
}

DxStaticPis* DxStaticPisContainer::FindPiece ( const char *szFile )
{
	MAPSTATICPIS_ITER iter = m_mapStaticPis.find ( szFile );
	if ( iter!=m_mapStaticPis.end() )	return (*iter).second;

	return NULL;
}

DxStaticPis* DxStaticPisContainer::LoadPiece ( LPDIRECT3DDEVICE8 pd3dDevice, const char *szFile, const char *szDir )
{
	HRESULT hr=S_OK;

	DxStaticPis* pPiece;
	if ( pPiece = FindPiece ( szFile ) )
	{
		pPiece->m_nRefCount++;
		return pPiece;
	}

	std::string szFullName;
	if ( szDir )	szFullName = szDir;
	else			szFullName = "";
	szFullName += szFile;

	DxStaticPis* pNewPiece = new DxStaticPis;
	hr = pNewPiece->Load ( pd3dDevice, szFullName.c_str() );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pNewPiece);
		return NULL;
	}

	pNewPiece->m_nRefCount++;

	//	Note : 리스트에 등록.
	//
	m_mapStaticPis.insert ( std::make_pair(szFile,pNewPiece) );

	return pNewPiece;
}

void DxStaticPisContainer::ReleasePiece ( const char *szFile )
{
	MAPSTATICPIS_ITER iter = m_mapStaticPis.find ( szFile );
	if ( iter==m_mapStaticPis.end() )	return;

	(*iter).second->m_nRefCount--;
	if ( (*iter).second->m_nRefCount <= 0 )
	{
		delete (*iter).second;
		m_mapStaticPis.erase ( iter );
	}

	return;
}

//DxSingleMesh* DxStaticPisContainer::GetSingleMesh ( const char *szPis, const char *szMaterial )
//{
//	MAPSTATICPIS_ITER iter = m_mapStaticPis.find ( szPis );
//	if ( iter != m_mapStaticPis.end() )
//	{
//		MAPSINGLEMESH_ITER iter_s;
//		iter_s = (*iter).second->GetStaticMesh()->m_mapSingle.find ( szMaterial );
//		if ( iter_s != (*iter).second->GetStaticMesh()->m_mapSingle.end() )
//		{
//			return (*iter_s).second;
//		}
//
//		iter_s = (*iter).second->GetStaticMesh()->m_mapSingleAlpha.find ( szMaterial );
//		if ( iter_s != (*iter).second->GetStaticMesh()->m_mapSingleAlpha.end() )
//		{
//			return (*iter_s).second;
//		}
//	}
//
//	return NULL;
//}
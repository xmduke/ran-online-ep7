#include "stdafx.h"

#include "SerialFile.h"
#include "DxSkinMesh.h"
#include "DxSkinMeshMan.h"

#include "ReplaceContainer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW


DxSkinPieceContainer DxSkinPieceContainer::Instance;

const DWORD	DxSkinPiece::VERSION = 0x00000100;
const char	DxSkinPiece::m_szPIECETYPE[PIECE_SIZE][64] =
{
	"머리",
	"상체",
	"하체",
	"손",
	"오른손도구",
	"왼손도구",
	"발",
	"REV01",
	"REV02",
	"REV03",
	"REV04",
	"REV05",
};

DxSkinPiece::DxSkinPiece () :
	m_szFileName(NULL),
	
	m_dwRef(0),
	
	m_szXFileName(NULL),
	m_pSkinMesh(NULL),

	m_szSkeleton(NULL),
	m_pSkeleton(NULL),

	m_emType(PIECE_HEAD),
	m_szMeshName(NULL),
	m_pmcMesh(NULL),
	m_pNext(NULL)
{
}

DxSkinPiece::~DxSkinPiece ()
{
	SAFE_DELETE_ARRAY(m_szFileName);
	SAFE_DELETE_ARRAY(m_szXFileName);
	SAFE_DELETE_ARRAY(m_szSkeleton);
	SAFE_DELETE_ARRAY(m_szMeshName);

	SAFE_DELETE(m_pNext);

	// don't delete m_pmcMesh
}

HRESULT DxSkinPiece::SetSkinMesh ( const char* szXFileName, const char* szSkeleton, LPDIRECT3DDEVICE8 pd3dDevice )
{
	assert(szXFileName);
	assert(szSkeleton);
	assert(pd3dDevice);

	SAFE_DELETE(m_szSkeleton);
	m_szSkeleton = new char[strlen(szSkeleton)+1];
	strcpy ( m_szSkeleton, szSkeleton );
	m_pSkeleton = DxBoneCollector::Instance.LoadSkeleton ( m_szSkeleton, pd3dDevice );
	if ( !m_pSkeleton )		return E_FAIL;

	SAFE_DELETE(m_szXFileName);
	m_szXFileName = new char[strlen(szXFileName)+1];
	strcpy ( m_szXFileName, szXFileName );
	m_pSkinMesh = DxSkinMeshMan::Instance.LoadSkinMesh ( m_szXFileName, m_szSkeleton, pd3dDevice );
	if ( !m_pSkinMesh )		return E_FAIL;

	return S_OK;
}

HRESULT DxSkinPiece::LoadPiece ( const char* szFile, LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr;

	SAFE_DELETE(m_szFileName);
	m_szFileName = new char[strlen(szFile)+1];
	strcpy ( m_szFileName, szFile );

	char szPathName[MAX_PATH];
	strcpy ( szPathName, DxSkinPieceContainer::Instance.GetPath() );
	strcat ( szPathName, m_szFileName );

	CSerialFile SFile;
	if ( !SFile.OpenFile ( FOT_READ, (char*)szPathName ) )	return E_FAIL;

	DWORD dwVer;
	SFile >> dwVer;
	if ( dwVer != VERSION )		return E_FAIL;

	SFile >> m_dwRef;

	DWORD dwSize;

	SFile >> dwSize;
	char *szXFileName = new char[dwSize];
	SFile.ReadBuffer ( szXFileName, sizeof(char)*dwSize );

	SFile >> dwSize;
	char *szSkeleton = new char[dwSize];
	SFile.ReadBuffer ( szSkeleton, sizeof(char)*dwSize );

	//	Note : 스킨 메쉬 읽기.
	//
	hr = SetSkinMesh ( szXFileName, szSkeleton, pd3dDevice );
	SAFE_DELETE(szXFileName);
	SAFE_DELETE(szSkeleton);
	if ( FAILED(hr) )	return hr;

	SFile >> dwSize;
	m_emType = (EMPIECECHAR) dwSize;

	SFile >> dwSize;
	m_szMeshName = new char[dwSize];
	SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

	m_pmcMesh = m_pSkinMesh->FindMeshContainer ( m_szMeshName );
	if ( !m_pmcMesh )	return E_FAIL;

	return S_OK;
}

HRESULT DxSkinPiece::SavePiece ( const char* szFile )
{
	assert(szFile);
	
	SAFE_DELETE(m_szFileName);
	m_szFileName = new char[strlen(szFile)+1];
	strcpy ( m_szFileName, szFile );

	char szPathName[MAX_PATH];
	strcpy ( szPathName, DxSkinPieceContainer::Instance.GetPath() );
	strcat ( szPathName, m_szFileName );

	CSerialFile SFile;
	if ( !SFile.OpenFile ( FOT_WRITE, (char*)szPathName ) )	return E_FAIL;

	SFile << VERSION;

	SFile << m_dwRef;

	DWORD dwSize;

	if ( !m_szXFileName )	return E_FAIL;

	dwSize = strlen(m_szXFileName)+1;
	SFile << dwSize;
	SFile.WriteBuffer ( m_szXFileName, sizeof(char)*dwSize );

	if ( !m_szSkeleton )	return E_FAIL;

	dwSize = strlen(m_szSkeleton)+1;
	SFile << dwSize;
	SFile.WriteBuffer ( m_szSkeleton, sizeof(char)*dwSize );

	SFile << (DWORD) m_emType;

	if ( !m_szMeshName )	return E_FAIL;

	dwSize = strlen(m_szMeshName)+1;
	SFile << dwSize;
	SFile.WriteBuffer ( m_szMeshName, sizeof(char)*dwSize );

	return S_OK;
}

HRESULT DxSkinPiece::InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	return S_OK;
}

HRESULT DxSkinPiece::RestoreDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	return S_OK;
}

HRESULT DxSkinPiece::InvalidateDeviceObjects ()
{
	return S_OK;
}

HRESULT DxSkinPiece::DeleteDeviceObjects ()
{
	return S_OK;
}

HRESULT DxSkinPiece::Render ( LPDIRECT3DDEVICE8 pd3dDevice, D3DXMATRIX &matRot )
{
	assert(pd3dDevice);

	HRESULT hr = S_OK;
	if ( !m_pSkeleton || !m_pSkinMesh )		return S_FALSE;

	//	Note : 계층 메트릭스 업데이트.
	//
	m_pSkeleton->UpdateBones ( m_pSkeleton->pBoneRoot, matRot );

	//	Note : SkinMesh 전체 그리기.
	//
	DWORD dwFill;
	pd3dDevice->GetRenderState ( D3DRS_FILLMODE, &dwFill );
	pd3dDevice->SetRenderState ( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	m_pSkinMesh->Render ( matRot, pd3dDevice );
	pd3dDevice->SetRenderState ( D3DRS_FILLMODE, dwFill );

	//	Note : SkinMesh 선택부분 그리기.
	//
	if ( !m_pmcMesh )	return S_FALSE;

	DWORD	dwNormal;
	pd3dDevice->GetRenderState ( D3DRS_NORMALIZENORMALS, &dwNormal );
	pd3dDevice->SetRenderState ( D3DRS_NORMALIZENORMALS, TRUE );

	hr = m_pSkinMesh->DrawMeshContainer ( pd3dDevice, m_pmcMesh );
	if ( FAILED(hr) )	return hr;

	pd3dDevice->SetRenderState ( D3DRS_NORMALIZENORMALS, dwNormal );

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxSkinPieceContainer::DxSkinPieceContainer() :
	m_pd3dDevice(NULL),
	m_pSkinPieceHead(NULL)
{
}

DxSkinPieceContainer::~DxSkinPieceContainer()
{
}

HRESULT DxSkinPieceContainer::AddPiece ( DxSkinPiece* pSkinPiece )
{
	HRESULT	hr;
	assert( pSkinPiece );
	if ( !pSkinPiece->m_szFileName )	return E_FAIL;

	hr = pSkinPiece->SavePiece ( pSkinPiece->m_szFileName );
	if ( FAILED(hr) )	return E_FAIL;

	//	Note : 리스트에 삽입.
	//
	pSkinPiece->m_pNext = m_pSkinPieceHead;
	m_pSkinPieceHead = pSkinPiece;

	return S_OK;
}

DxSkinPiece* DxSkinPieceContainer::LoadPiece ( const char* szFile, LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr;
	assert(szFile);

	DxSkinPiece* pSkinPiece = NULL;

	pSkinPiece = FindPiece ( szFile );
	if ( pSkinPiece )	return pSkinPiece;

	//	Note : SkinPiece Load 수행.
	//
	pSkinPiece = new DxSkinPiece;

	hr = pSkinPiece->LoadPiece ( szFile, pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pSkinPiece);
		return NULL;
	}

	strcpy ( pSkinPiece->m_szFileName, szFile );

	//	Note : 리스트에 삽입.
	//
	pSkinPiece->m_pNext = m_pSkinPieceHead;
	m_pSkinPieceHead = pSkinPiece;

	return pSkinPiece;
}

HRESULT DxSkinPieceContainer::ReleasePiece ( const char* szFile )
{
	assert(szFile);
	if ( !m_pSkinPieceHead )	return S_FALSE;

	DxSkinPiece *pSkinPieceCur = m_pSkinPieceHead, *pSkinPieceBack = NULL;
	if ( !strcmp(pSkinPieceCur->m_szFileName,szFile) )
	{
		pSkinPieceCur->m_dwRef--;
		if ( pSkinPieceCur->m_dwRef == 0 )
		{
			m_pSkinPieceHead = m_pSkinPieceHead->m_pNext;
			pSkinPieceCur->m_pNext = NULL;
			SAFE_DELETE(pSkinPieceCur);
		}

		return S_OK;
	}

	pSkinPieceBack = m_pSkinPieceHead;
	pSkinPieceCur = m_pSkinPieceHead->m_pNext;
	while ( pSkinPieceCur )
	{
		if ( !strcmp(pSkinPieceCur->m_szFileName,szFile) )
		{
			pSkinPieceCur->m_dwRef--;
			if ( pSkinPieceCur->m_dwRef == 0 )
			{
				pSkinPieceBack->m_pNext = pSkinPieceCur->m_pNext;
				pSkinPieceCur->m_pNext = NULL;
				SAFE_DELETE(pSkinPieceCur);
			}

			return S_OK;
		}

		pSkinPieceBack = pSkinPieceCur;
		pSkinPieceCur = pSkinPieceCur->m_pNext;
	}

	return S_OK;
}

HRESULT DxSkinPieceContainer::DeletePiece ( const char* szFile )
{
	if ( !szFile )				return S_FALSE;
	if ( !m_pSkinPieceHead )	return S_FALSE;

	DxSkinPiece *pSkinPieceCur = m_pSkinPieceHead, *pSkinPieceBack = NULL;
	if ( !strcmp(pSkinPieceCur->m_szFileName,szFile) )
	{
		m_pSkinPieceHead = m_pSkinPieceHead->m_pNext;
		pSkinPieceCur->m_pNext = NULL;
		SAFE_DELETE(pSkinPieceCur);

		return S_OK;
	}

	pSkinPieceBack = m_pSkinPieceHead;
	pSkinPieceCur = m_pSkinPieceHead->m_pNext;
	while ( pSkinPieceCur )
	{
		if ( !strcmp(pSkinPieceCur->m_szFileName,szFile) )
		{
			pSkinPieceBack->m_pNext = pSkinPieceCur->m_pNext;
			pSkinPieceCur->m_pNext = NULL;
			SAFE_DELETE(pSkinPieceCur);

			return S_OK;
		}

		pSkinPieceBack = pSkinPieceCur;
		pSkinPieceCur = pSkinPieceCur->m_pNext;
	}

	return S_OK;
}

DxSkinPiece* DxSkinPieceContainer::FindPiece ( const char* szFile )
{
	assert(szFile);

	DxSkinPiece *pSkinPieceCur = m_pSkinPieceHead;
	while ( pSkinPieceCur )
	{
		if ( !strcmp(pSkinPieceCur->m_szFileName,szFile) )	return pSkinPieceCur;

		pSkinPieceCur = pSkinPieceCur->m_pNext;
	}

	return NULL;
}

HRESULT DxSkinPieceContainer::InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	DxSkinPiece *pSkinPieceCur = m_pSkinPieceHead;
	while ( pSkinPieceCur )
	{
		pSkinPieceCur->InitDeviceObjects ( pd3dDevice );

		pSkinPieceCur = pSkinPieceCur->m_pNext;
	}

	return S_OK;
}

HRESULT DxSkinPieceContainer::RestoreDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	DxSkinPiece *pSkinPieceCur = m_pSkinPieceHead;
	while ( pSkinPieceCur )
	{
		pSkinPieceCur->RestoreDeviceObjects ( pd3dDevice );

		pSkinPieceCur = pSkinPieceCur->m_pNext;
	}

	return S_OK;
}

HRESULT DxSkinPieceContainer::InvalidateDeviceObjects ()
{
	DxSkinPiece *pSkinPieceCur = m_pSkinPieceHead;
	while ( pSkinPieceCur )
	{
		pSkinPieceCur->InvalidateDeviceObjects ();

		pSkinPieceCur = pSkinPieceCur->m_pNext;
	}

	return S_OK;
}

HRESULT DxSkinPieceContainer::DeleteDeviceObjects()
{
	DxSkinPiece *pSkinPieceCur = m_pSkinPieceHead;
	while ( pSkinPieceCur )
	{
		pSkinPieceCur->DeleteDeviceObjects ();

		pSkinPieceCur = pSkinPieceCur->m_pNext;
	}

	return S_OK;
}

HRESULT DxSkinPieceContainer::FinalCleanup ()
{
	SAFE_DELETE(m_pSkinPieceHead);

	return S_OK;
}


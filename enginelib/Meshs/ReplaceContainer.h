// ReplaceContainer.h: interface for the CReplaceContainer class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "GLCharDefine.h"

#include "DxSkinMesh.h"
#include "DxSkinObject.h"
#include "DxSkinAniMan.h"

struct DxSkinPiece
{
	char*				m_szFileName;

	DWORD				m_dwRef;
	
	char*				m_szXFileName;
	DxSkinMesh*			m_pSkinMesh;

	char*				m_szSkeleton;
	DxSkeleton*			m_pSkeleton;

	EMPIECECHAR			m_emType;
	char*				m_szMeshName;
	PSMESHCONTAINER		m_pmcMesh;

	DxSkinPiece*		m_pNext;

	const static DWORD	VERSION;
	const static char	m_szPIECETYPE[PIECE_SIZE][64];


	HRESULT SetSkinMesh ( const char* szXFileName, const char* szSkeleton, LPDIRECT3DDEVICE8 pd3dDevice );

	HRESULT LoadPiece ( const char* szFile, LPDIRECT3DDEVICE8 pd3dDevice );
	HRESULT SavePiece ( const char* szFile );

	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice );
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects();

	HRESULT Render ( LPDIRECT3DDEVICE8 pd3dDevice, D3DXMATRIX &matRot );

	HRESULT DrawMeshContainer ( LPDIRECT3DDEVICE8 pd3dDevice, PSMESHCONTAINER pmcMesh, BOOL bShadow=FALSE )
	{
		if ( !pmcMesh )		return E_FAIL;

		if ( m_pSkinMesh )
		{
			return m_pSkinMesh->DrawMeshContainer ( pd3dDevice, pmcMesh, bShadow );
		}

		return E_FAIL;
	}

	DxSkinPiece ();
	~DxSkinPiece ();
};
typedef DxSkinPiece*	PDXSKINPIECE;

class DxSkinPieceContainer  
{
protected:
	char			m_szPath[MAX_PATH];

public:
	void			SetPath ( char* szPath )	{ strcpy ( m_szPath, szPath ); }
	char*			GetPath ()					{ return m_szPath; }

protected:
	LPDIRECT3DDEVICE8	m_pd3dDevice;
	DxSkinPiece			*m_pSkinPieceHead;

public:
	DxSkinPiece* GetSkinPieceList () { return m_pSkinPieceHead; }

public:
	HRESULT AddPiece ( DxSkinPiece* pSkinPiece );

	DxSkinPiece* LoadPiece ( const char* szFile, LPDIRECT3DDEVICE8 pd3dDevice );
	HRESULT ReleasePiece ( const char* szFile );
	HRESULT DeletePiece ( const char* szFile );
	DxSkinPiece* FindPiece ( const char* szFile );

public:
//	BOOL LoadFile ( char *szFile );
//	BOOL SaveFile ( char *szFile );

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice );
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects();
	HRESULT FinalCleanup();

protected:
	DxSkinPieceContainer();

public:
	virtual ~DxSkinPieceContainer();

public:
	static DxSkinPieceContainer Instance;
};


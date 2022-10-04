#pragma once

#include <map>

#include "DxStaticMesh.h"

class DxStaticPis	// Static, BSP, Effect Mesh~!!
{
protected:
	static const DWORD VERSION;

protected:
	static char		m_szPath[MAX_PATH];

public:
	static void		SetPath ( char* szPath )	{ strcpy ( m_szPath, szPath ); }
	static char*	GetPath ()					{ return m_szPath; }

protected:
	std::string	m_szFrameName;
	D3DXVECTOR3 m_vMax;
	D3DXVECTOR3 m_vMin;

public:
	const char*	GetFrameName ()		{ return m_szFrameName.c_str(); }
	D3DXVECTOR3	GetAABBMax ()		{ return m_vMax; }
	D3DXVECTOR3	GetAABBMin ()		{ return m_vMin; }

public:
	int		m_nRefCount;

protected:
	DxFrameMesh*		m_pFrameMesh;
	DxStaticMesh*		m_pStaticMesh;

public:
	DxFrameMesh*		GetFrameMesh()		{ return m_pFrameMesh; }
	DxStaticMesh*		GetStaticMesh()		{ return m_pStaticMesh; }

public:
	HRESULT	NewFrameMesh ( LPDIRECT3DDEVICE8 pd3dDevice, const char* szName );
	void	MakeStaticMesh ( LPDIRECT3DDEVICE8 pd3dDevice );

public:
	DxFrame*	GetFrameRoot ()
	{ 
		if (m_pFrameMesh)	return m_pFrameMesh->GetFrameRoot(); 
		else				return NULL;
	}

public:
	BOOL IsCollisionLine ( const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, D3DXMATRIX& matWorld );	

public:
	void Render ( const LPDIRECT3DDEVICE8 pd3dDevice, const CLIPVOLUME &_sCV, const D3DXMATRIX& matWorld );
	void Render ( const LPDIRECT3DDEVICE8 pd3dDevice, const CLIPVOLUME &_sCV, const D3DXMATRIX& matWorld, const char* szName );
	void CleanUp();

public:
	void Save ( char* szName );
	HRESULT Load ( LPDIRECT3DDEVICE8 pd3dDevice, const char* szName, BOOL bEdit=FALSE );

public:
	DxStaticPis();
	~DxStaticPis();
};

class DxStaticPisContainer
{
public:
	typedef std::map<std::string,DxStaticPis*>				MAPSTATICPIS;
	typedef std::map<std::string,DxStaticPis*>::iterator	MAPSTATICPIS_ITER;
	MAPSTATICPIS	m_mapStaticPis;

public:
	DxStaticPis* LoadPiece ( LPDIRECT3DDEVICE8 pd3dDevice, const char *szFile, const char *szDir=NULL );
	void ReleasePiece ( const char *szFile );

//	DxSingleMesh* GetSingleMesh ( const char *szPis, const char *szMaterial );

protected:
	DxStaticPis* FindPiece ( const char *szFile );

public:
	void CleanUp();

public:
	DxStaticPisContainer();
	~DxStaticPisContainer();

public:
	static DxStaticPisContainer& GetInstance();
};
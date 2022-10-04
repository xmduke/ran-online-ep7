#pragma once

#include <map>

#include "Collision.h"
#include "DxFrameMesh.h"
#include "NsCollisionMap.h"

#include "DxStaticMeshCreate.h"

class DxSBEMesh	// Static, BSP, Effect Mesh~!!
{
protected:
	D3DXVECTOR3 m_vMax;
	D3DXVECTOR3 m_vMin;

protected:
	DxFrameMesh*		m_pFrameMesh;
	DxStaticMesh*		m_pStaticMesh;
	DxAABBCollision*	m_pAABBCollision;

public:
	DxFrameMesh*		GetFrameMesh()		{ return m_pFrameMesh; }
	DxStaticMesh*		GetStaticMesh()		{ return m_pStaticMesh; }
	DxAABBCollision*	GetAABBCollision()	{ return m_pAABBCollision; }

public:
	void MakeStaticMesh ( LPDIRECT3DDEVICE8 pd3dDevice );
	void Render ( LPDIRECT3DDEVICE8 pd3dDevice, CLIPVOLUME *pCV );

protected:
	void CleanUp();

public:
	DxSBEMesh();
	~DxSBEMesh();
};

class CStaticMeshOBJ : public OBJAABB
{
public:
	D3DXVECTOR3 vMax;
	D3DXVECTOR3 vMin;

	DxStaticMesh*	pStaticMesh;

	CStaticMeshOBJ* pNext;

public:
	CStaticMeshOBJ();
	~CStaticMeshOBJ();
};

class CStaticMeshMAP
{
public:
	D3DXVECTOR3 vMax;
	D3DXVECTOR3 vMin;

	CStaticMeshMAP* pObj;

public:
	CStaticMeshMAP();
	~CStaticMeshMAP();
};
typedef CStaticMeshMAP*	PStaticMeshMAP;


class DxStaticMeshMan
{
public:
	CStaticMeshOBJ*	m_pMeshList;
	CStaticMeshMAP*	m_pMeshTree;

public:
	DxStaticMeshMan();
	~DxStaticMeshMan();
};


class DxStaticMeshContainer
{
public:
	typedef std::map<std::string,DxStaticMesh>				MAPSTATICMESH;
	typedef std::map<std::string,DxStaticMesh>::iterator	MAPSTATICMESH_ITER;

	MAPSTATICMESH m_mapStaticMesh;

public:
	void LoadMesh ( char* szName );
	void FindFile ( char* szName );

public:
	DxStaticMeshContainer();
	~DxStaticMeshContainer();
};


class DxStaticMeshExporter
{
private:
	static char			m_szOpt[MAX_PATH];
	static char			m_szExt[MAX_PATH];

public:
	void ExportXOPTtoMBE ( char* szName );

public:
	DxStaticMeshExporter();
	~DxStaticMeshExporter();
};

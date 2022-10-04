#include "stdafx.h"
#include "DxViewPort.h"
#include "SerialFile.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

#include "StlFunctions.h"

#include "DxStaticMeshMan.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x			S	t	a	t	i	c	M	e	s	h
// -----------------------------------------------------------------------------------------------------------------------------------------
DxSBEMesh::DxSBEMesh() :
	m_pFrameMesh(NULL),
	m_pStaticMesh(NULL),
	m_pAABBCollision(NULL)
{
}

DxSBEMesh::~DxSBEMesh()
{
	CleanUp();
}

void DxSBEMesh::CleanUp()
{
	SAFE_DELETE ( m_pFrameMesh );
	SAFE_DELETE ( m_pStaticMesh );
	SAFE_DELETE ( m_pAABBCollision );
}

void DxSBEMesh::MakeStaticMesh ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	//NSSTATICMESH::ExportXOPTtoSBE ( pd3dDevice, &m_sFrameMesh, &m_sStaticMesh, &m_sBspTree );

	// Note : AABB_Octree 持失
	SAFE_DELETE ( m_pStaticMesh );
	m_pStaticMesh = new DxStaticMesh;
	m_pStaticMesh->MakeAABBOCTree ( pd3dDevice, m_pFrameMesh );

	// Note : AABB_COLLISION 持失
	SAFE_DELETE ( m_pAABBCollision );
	m_pAABBCollision = new DxAABBCollision;
	m_pAABBCollision->MakeAABBCollision( m_pFrameMesh );
}

void DxSBEMesh::Render ( LPDIRECT3DDEVICE8 pd3dDevice, CLIPVOLUME *pCV )
{
	if ( m_pStaticMesh )	m_pStaticMesh->Render( pd3dDevice, pCV );
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//							D	x			S	t	a	t	i	c	M	e	s	h			M	a	n
// -----------------------------------------------------------------------------------------------------------------------------------------
DxStaticMeshMan::DxStaticMeshMan()
{
}

DxStaticMeshMan::~DxStaticMeshMan()
{
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//							D	x			S	t	a	t	i	c	M	e	s	h			C	o	n	t	a	i	n	e	r
// -----------------------------------------------------------------------------------------------------------------------------------------
DxStaticMeshContainer::DxStaticMeshContainer()
{
}

DxStaticMeshContainer::~DxStaticMeshContainer()
{
}

void LoadMesh ( char* szName )
{
}

void FindFile ( char* szName )
{
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//							D	x			S	t	a	t	i	c	M	e	s	h			E	x	p	o	r	t	e	r
// -----------------------------------------------------------------------------------------------------------------------------------------
char	DxStaticMeshExporter::m_szOpt[] = ".opt";
char	DxStaticMeshExporter::m_szExt[] = ".mbe";

DxStaticMeshExporter::DxStaticMeshExporter()
{
}

DxStaticMeshExporter::~DxStaticMeshExporter()
{
}

void DxStaticMeshExporter::ExportXOPTtoMBE ( char* szName )
{
	//// 1. X_File Load

	//// 2. BSP Tree Create

	//// 3. .sbm File Create
	//std::string strName;
	//std::string strNewName;
	//strName = szName;

	//int nSize = strName.size();
	//strName.resize ( nSize-2 );

	////strNewName = m_szPath;
	////strNewName += strName.c_str();
	////strNewName += m_szExt;

	//// 4. .sbm File Save
}
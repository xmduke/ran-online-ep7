// DxStaticMeshCreate.h: interface for the CCollision class.
//
//	class DxStaticMesh : FrameMesh 데이타를 넣으면 AABB 트리 적용하고 Octree로 세세하고 나눈다.
//
//	Name : Sung-Hwan Han
//	Begin :2004/10/21
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <map>

#include "NsOCTree.h"
#include "DxFrameMesh.h"

class DxStaticMesh
{
public:
	DxAABBOctree* m_pMeshList;
	DxAABBOctree* m_pMeshAlphaList;

protected:
	OBJAABBNode* m_pMeshTree;
	OBJAABBNode* m_pMeshAlphaTree;

public:
	void MakeAABBOCTree ( LPDIRECT3DDEVICE8 pd3dDevice, DxFrameMesh* pFrameMesh );

public:
	void FrameMove ( float fTime, float fElapsedTime );
	void Render( LPDIRECT3DDEVICE8 pd3dDevice, CLIPVOLUME *pCV );
	void RenderAlpha( LPDIRECT3DDEVICE8 pd3dDevice, CLIPVOLUME *pCV );

protected:
	void CleanUp();

public:
	DxStaticMesh();
	~DxStaticMesh();
};

namespace NSSTATICMESH
{
	struct STATICDATA
	{
		VERTEX*	pVertices;
		DWORD	dwFaceNUM;

		STATICDATA() :
			pVertices(NULL),
			dwFaceNUM(0)
		{
		};
		~STATICDATA()
		{
			SAFE_DELETE_ARRAY ( pVertices );
		};
	};

	typedef std::map<std::string,STATICDATA*>			MAPSTATICDATA;
	typedef std::map<std::string,STATICDATA*>::iterator	MAPSTATICDATA_ITER;

	void ExportMaterialList ( LPDIRECT3DDEVICE8 pd3dDevice, DxFrameMesh* pFrameMesh, DxStaticMesh* pStaticMesh );
};
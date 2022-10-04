#pragma once

#include <algorithm>
#include <map>
#include <list>

#include "Collision.h"

#include "NsOCTree.h"

class DxOptimizeMesh : public OBJAABB
{
public:
	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;
	LPDIRECT3DTEXTURE8	m_pTexture;
	VERTEX*				m_pVertex;		// 데이터 만들기 위해 거쳐가는 것
	DWORD				m_dwFace;
	DxOptimizeMesh*		m_pNext;

public:
	OBJOCTree*		m_pOcTree;

public:
	virtual void GetAABBSize ( D3DXVECTOR3 &_vMax, D3DXVECTOR3 &_vMin )
	{
		_vMax = m_vMax; _vMin = m_vMin;
	}
	virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
	{
		return COLLISION::IsWithInPoint ( vDivMax, vDivMin, (m_vMax+m_vMin)/2 );
	}
	virtual OBJAABB* GetNext () { return m_pNext; }
	virtual HRESULT Render ( LPDIRECT3DDEVICE8 pd3dDevice, CLIPVOLUME *pCV );

public:
	void Create ( LPDIRECT3DDEVICE8 pd3dDevice, const char* szName, DWORD dwFaceNUM, VERTEX* pVertices );
	void MakeOptimizeMesh ( LPDIRECT3DDEVICE8 pd3dDevice );

public:
	DxOptimizeMesh() :
		m_vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX),
		m_vMin(FLT_MAX,FLT_MAX,FLT_MAX),
		m_pVertex(NULL),
		m_dwFace(0),
		m_pTexture(NULL),
		m_pOcTree(NULL),
		m_pNext(NULL)
	{
	};
	~DxOptimizeMesh();
};
typedef DxOptimizeMesh* PDXOPTIMIZEMESH;

namespace NSOPTIMIZEMESH
{
	void MakeOctreeMesh ( LPDIRECT3DDEVICE8 pd3dDevice, VERTEX* pVertex, DWORD dwFaceNUM, POBJOCTREE &m_pOcTree );
	void MakeOptimizeMesh ( LPDIRECT3DDEVICE8 pd3dDevice, LPD3DXMESH& pMesh );
};
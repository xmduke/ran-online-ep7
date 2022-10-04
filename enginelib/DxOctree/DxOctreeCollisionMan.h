#ifndef	__DX_OCTREE_COLLISION_MAN__
#define	__DX_OCTREE_COLLISION_MAN__

//	클래스 선언
struct	DxFrame;
class	CSerialFile;
class	DxOctree;
struct	DxAABBNode;

class	DxOctreeCollision
{
public:
	//	Note	:	어느 박스인가?
	enum
	{
		TOP_LEFT_FRONT,			// 0
		TOP_LEFT_BACK,			// 1
		TOP_RIGHT_BACK,			// etc...
		TOP_RIGHT_FRONT,
		BOTTOM_LEFT_FRONT,
		BOTTOM_LEFT_BACK,
		BOTTOM_RIGHT_BACK,
		BOTTOM_RIGHT_FRONT
	};

public:
			DxOctreeCollision ();
	virtual	~DxOctreeCollision();

public:
	BOOL	OneTimeSceneInit ();
	BOOL	FinalCleanup ();

public:
	//	Note : 실제 데이타 입출력
	//
	BOOL	SetVertexArray ( D3DXVECTOR3* pVertexArray, DWORD nVertexArray );
	BOOL	GetVertexArray ( D3DXVECTOR3* pVertexArray, DWORD* pnVertexArray ) const;
	BOOL	DelVertexArray ();

public:
	//	Note : 자식제어
	//
	BOOL	NewChild ( int nIndex );	
	BOOL	DelChild ( int nIndex );
	DxOctreeCollision*	GetChild ( int nIndex );

public:
	//	Note : 민맥스 포인트
	//
	void	SetMinMaxPoint ( D3DXVECTOR3 vMin, D3DXVECTOR3 vMax );
	void	GetMinMaxPoint ( D3DXVECTOR3* pvMin, D3DXVECTOR3* pvMax );

public:
	//	Note : 리프노드?
	void	SetSubDivided	();
	void	ResetSubDivided	();
	BOOL	IsSubDivided	();

private:
	//	Note : 컬링 정보.
	//
	D3DXVECTOR3		m_vMax;	//	TOP_RIGHT_BACK
	D3DXVECTOR3		m_vMin;	//	BOTTOM_LEFT_FRONT	

	//	Note : 트리 정보.
	//
	BOOL			m_bSubDivided;

	//	Note : 버텍스
	//		   노드안에 들어오는 모든 버텍스
    D3DXVECTOR3*	m_pVertexArray;
	DWORD			m_nVertexArray;	

	//	Note : 8개 자식 노드
	//
	union
	{
		struct
		{
			DxOctreeCollision	*m_pNodes[8];
		};

		struct
		{
			DxOctreeCollision	*m_pNodes_0;
			DxOctreeCollision	*m_pNodes_1;
			DxOctreeCollision	*m_pNodes_2;
			DxOctreeCollision	*m_pNodes_3;
			DxOctreeCollision	*m_pNodes_4;
			DxOctreeCollision	*m_pNodes_5;
			DxOctreeCollision	*m_pNodes_6;
			DxOctreeCollision	*m_pNodes_7;
		};
	};
};

class	DxOctreeCollisionMan
{
public:
			DxOctreeCollisionMan ();
	virtual	~DxOctreeCollisionMan();

public:
	BOOL	Import ( DxOctree* pOctree );

public:
	BOOL	OneTimeSceneInit ();
	BOOL	FinalCleanup ();

private:
	BOOL	ExtractVertexFromOctree ( DxOctree* pOctree, DxOctreeCollision*	pRootNode );
	BOOL	GetNodeVertex ( DxFrame* pDxFrame, DxOctreeCollision* pNode );

private:
	BOOL	CheckIntegrity ( DxOctree* pOctree );
	BOOL	GetOctreeMeshInfo ( DxOctree* pOctree, DWORD* pNodeCount, DWORD* pIndicesCount, DWORD* pVerticesCount );
	BOOL	GetOctreeCollisioinInfo ( DxOctreeCollision* pNode, DWORD* pNodeCount, DWORD* pIndicesCount, DWORD* pVerticesCount );

private:
	//	<--	AABB Tree 만들기	
	HRESULT MakeAABBNode ( DxAABBNode *pNode, D3DXMATRIX &matComb, DWORD *pCellIndex, DWORD nCellIndex, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );	
	HRESULT GetCenterDistNode ( D3DXMATRIX &matComb, DWORD	*pCellIndex, DWORD nCellIndex, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	HRESULT GetSizeNode ( D3DXMATRIX &matComb, DWORD *pCellIndex, DWORD nCellIndex,	D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	BOOL	IsWithInTriangle ( D3DXMATRIX &matComb, LPD3DXVECTOR3 pvT1, LPD3DXVECTOR3 pvT2, LPD3DXVECTOR3 pvT3, float fDivision, DWORD dwAxis );
	void	IsCollision ( DxAABBNode *pAABBCur, D3DXVECTOR3 &vP1, D3DXVECTOR3 &vP2, D3DXVECTOR3 &vCollision, DWORD &CollisionID );
	void	GetAllCollisionCell ( DWORD *pCollisionCellID, DWORD& CollisionIDCount, D3DXVECTOR3& vAMax, D3DXVECTOR3& vAMin, DxAABBNode *pAABBCur );	
	//	-->	AABB Tree 만들기

private:
	DxOctreeCollision*	m_pRootNode;		//	Vertex Tree
	DxAABBNode*			m_pAABBTreeRoot;	//	AABB 트리
};
#endif	//	__DX_OCTREE_COLLISION_MAN__
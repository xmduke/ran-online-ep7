#ifndef	__C_HEAP__
#define	__C_HEAP__

#include "CList.h"

template<class TYPE >
class	CHeap
{
protected:
	struct	NODE
	{
		typedef NODE* PNODE;

		TYPE	Data;		//	실제 데이타

		union
		{
			struct {	NODE	*pParent; };	//	부모
			struct {	NODE	*pNext; };		//	Garbage 에서의 리스트.
		};

		NODE	*pLChild;	//	왼쪽 자식
		NODE	*pRChild;	//	오른쪽 자식
		
		NODE ()	:
			pParent(NULL),
			pLChild(NULL),
			pRChild(NULL)
		{
		}

		void GarbageCollection ( PNODE &pNode )
		{
			if ( pLChild )	pLChild->GarbageCollection ( (PNODE)pNode );
			if ( pRChild )	pRChild->GarbageCollection ( (PNODE)pNode );

			this->pNext = pNode;
			pNode = this;
		}

		~NODE ()
		{
			SAFE_DELETE(pLChild);
			SAFE_DELETE(pRChild);
		}
	};	

	//	<--	노드리스트 설명
	//	<**	노드리스트는 간단히 말하자면,
	//		힙에서 루트와 최종 리프를 빠른 시간내에
	//		찾아주게 하는 꽁수의 결정체다.
	//		이 리스트에서 Head를 가져오게 되면 루트가 되는 것이고,
	//		테일을 가져오면, 최종 리프가 되는 것이다.
	//		그것은 힙과 리스트간 구조상의 절묘한 조화인 것이다.
	//	**>
	CMList<NODE*>	m_NodeList;

	NODE			*m_pGarList;

	//	-->	노드리스트 설명

	NODE	*m_pRoot;
	NODE	*m_pNode;

	//	<--	데이타 비교 함수 포인터
	int		(*m_pfCompare)( TYPE &DataA, TYPE &DataB );
	//	-->	데이타 비교 함수 포인터

public:
	CHeap();
	virtual	~CHeap();

	//	<--	데이타 핸들링
	void	SetCompareFunc ( void* Func );
	BOOL	Insert ( TYPE &Data );
	BOOL	Delete ( TYPE &Data );	
	//	-->	데이타 핸들링

	//	<--	힙의 구조와는 무관한 메쏘드
	BOOL	Adjust	( TYPE &Data );
	BOOL	FindNode( TYPE &Data );
	//	-->	힙의 구조와는 무관한 메쏘드

	void	Clear		();
	DWORD	GetAmount	() { return m_NodeList.GetAmount(); }

protected:	
	void	Swap ( TYPE &DataA, TYPE &DataB );	
};

template<class TYPE>
inline CHeap<TYPE>::CHeap () :
m_pRoot(NULL),
m_pGarList(NULL),
m_pNode(NULL)
{
	Clear ();
}

template<class TYPE>
inline CHeap<TYPE>::~CHeap ()
{
	Clear ();

	NODE *pGarCur = (NODE*) m_pGarList, *pGarNext=NULL;
	while ( pGarCur )
	{
		pGarNext = pGarCur->pNext;

		pGarCur->pLChild = NULL;
		pGarCur->pRChild = NULL;
		SAFE_DELETE(pGarCur);

		pGarCur = pGarNext;
	}
}

template<class TYPE>
inline void CHeap<TYPE>::SetCompareFunc( void* Func )
{
	m_pfCompare = ( int (__cdecl*)( TYPE &, TYPE & ) ) Func;
}

template<class TYPE>
inline BOOL	CHeap<TYPE>::Insert ( TYPE &Data )
{
	static NODE* pNewNode;

	if ( m_pGarList )
	{
		pNewNode = m_pGarList;
		m_pGarList = m_pGarList->pNext;

		pNewNode->pParent = NULL;
		pNewNode->pLChild = NULL;
		pNewNode->pRChild = NULL;
	}
	else
	{
		pNewNode = new NODE;
	}

	pNewNode->Data = Data;
	m_NodeList.AddTail ( pNewNode );
	    
	//	<--	부모 가져오기
	if ( m_NodeList.GetAmount() == 1 )
	{
		m_pRoot = pNewNode;
		m_NodeList.SetHead();
		return TRUE;
	}

	static NODE	*pParent = NULL;
	m_NodeList.GetCurrent ( pParent );
	if ( pParent->pLChild && pParent->pRChild )
	{
		m_NodeList.GoNext ();
		m_NodeList.GetCurrent ( pParent );
	}
	//	-->	부모 가져오기

	//	<--	부모 자식 연결
	if ( !pParent->pLChild )
	{
		pParent->pLChild = pNewNode;
	}
	else
	{
		pParent->pRChild = pNewNode;
	}
	pNewNode->pParent = pParent;
	//	-->	부모 자식 연결

	//	<--	자료 정렬
	pParent = NULL;
	static  NODE	*pChild = NULL;

	m_NodeList.GetTail ( pChild );
	pParent = pChild->pParent;

	while ( pParent )
	{
		if ( 0 < m_pfCompare ( pParent->Data , pChild->Data ) )
		{
			Swap ( pParent->Data, pChild->Data );
		}

		pChild = pParent;
		pParent = pChild->pParent;
	}
	//	-->	자료 정렬

	return TRUE;
}

template<class TYPE>
inline BOOL	CHeap<TYPE>::Delete ( TYPE &Data )
{
	static NODE* pLeaf;

	//	<--	루트 따기 ( 데이타만 )
	Data = m_pRoot->Data;
	//	-->	루트 따기

	if ( m_NodeList.GetAmount() == 1 )
	{
		Clear ();

		return TRUE;
	}
	
	//	<--	루트에 리프 집어 넣기 ( 데이타만 )
	pLeaf = NULL;
	m_NodeList.GetTail ( pLeaf );
	m_pRoot->Data = pLeaf->Data;
	//	<--	루트에 리프 집어 넣기 ( 데이타만 )

	//	<--	자료 정렬
	static NODE	*pParent = NULL;
	static NODE	*pChild	= NULL;
	static NODE	*pLChild = NULL;
	static NODE	*pRChild = NULL;
    
	pParent = m_pRoot;
	pLChild = pParent->pLChild;
	pRChild = pParent->pRChild;
	while ( pLChild || pRChild )
	{
		//	<--	자식중 작은 놈을 선택
		if ( pRChild )		//	자식 둘 다 있을 때
		{
			if ( 0 < m_pfCompare ( pLChild->Data, pRChild->Data ) )
			{
				pChild = pRChild;
			}
			else
			{
				pChild = pLChild;
			}
		}
		else if ( pLChild )	//	자식 왼쪽만 있을 때
		{
			pChild = pLChild;
		}
		//	-->	자식중 큰 놈을 선택

		//	<--	자식과 부모의 비교
		if ( 0 < m_pfCompare ( pParent->Data, pChild->Data ) )
		{
			Swap ( pParent->Data, pChild->Data );
		}
		//	-->	자식과 부모의 비교

		pParent = pChild;
		pLChild = pParent->pLChild;
		pRChild = pParent->pRChild;
	}
	//	-->	자료 정렬

	//	<--	리프 노드 삭제
	pParent = pLeaf->pParent;
	if ( pParent->pRChild )
	{
		pParent->pRChild = NULL;
	}
	else if ( pParent->pLChild )
	{
        pParent->pLChild = NULL;
	}

	//	Note : 휴면 노드 등록
	//
	pLeaf->pNext = m_pGarList;
	m_pGarList = pLeaf;
	pLeaf = NULL;

	m_NodeList.DelTail ();
	//	-->	리프 노드 삭제

	return TRUE;
}

template<class TYPE>
inline void	CHeap<TYPE>::Clear ()
{	
	if ( m_pRoot )
	{
		m_pRoot->GarbageCollection ( m_pGarList );
		m_pRoot	= NULL;
	}

	m_pNode = NULL;

	m_NodeList.RemoveAll ();
}

template<class TYPE>
inline void	CHeap<TYPE>::Swap ( TYPE &DataA, TYPE &DataB )
{
	TYPE	Temp;
	
	Temp = DataA;
	DataA = DataB;
	DataB = Temp;
}

template<class TYPE>
inline BOOL CHeap<TYPE>::FindNode ( TYPE &Data )
{
	if ( !m_pRoot )
	{
		return FALSE;
	}

	NODE **pStack = new NODE*[ m_NodeList.GetAmount() + 1 ];	//	0번은 쓰지 않으므로 한 개 더 잡는다.

	pStack[0] = NULL;
	pStack[1] = m_pRoot;
	DWORD	StackCount = 1;	

    while ( StackCount )
	{		
		//	<--	Pop
		NODE *pNODE = pStack[StackCount];
		StackCount--;
		//	-->	Pop

		if ( !m_pfCompare ( pNODE->Data, Data ) )
		{
			delete [] pStack;
			pStack = NULL;

			m_pNode = pNODE;

			return TRUE;
		}

		//	<--	Push
		if ( pNODE->pLChild && ( 0 < m_pfCompare ( pNODE->pLChild->Data, Data ) ) )
		{
			StackCount++;
			pStack[StackCount] = pNODE->pLChild;

		}
		if ( pNODE->pRChild && ( 0 < m_pfCompare ( pNODE->pRChild->Data, Data ) ) )
		{
			StackCount++;
			pStack[StackCount] = pNODE->pRChild;			
		}
		//	-->	Push
	}
    
	delete [] pStack;
	pStack = NULL;

	m_pNode = NULL;

	return FALSE;
}

template<class TYPE>
inline BOOL CHeap<TYPE>::Adjust ( TYPE &Data )
{
	if ( m_pNode )
	{
		//	<--	현재 들어있는것보다 큰 값이 들어온 경우
		if ( 0 < m_pfCompare ( m_pNode->Data, Data ) )
		{
			m_pNode->Data = Data;

			//	<--	자료 정렬
			NODE	*pParent = m_pNode->pParent;	
			NODE	*pChild = m_pNode;

			while ( pParent )
			{
				if ( 0 < m_pfCompare ( pParent->Data , pChild->Data ) )
				{
					Swap ( pParent->Data, pChild->Data );
				}

				pChild = pParent;
				pParent = pChild->pParent;
			}
			//	-->	자료 정렬

			return TRUE;
		}
		//	-->	현재 들어있는것보다 큰 값이 들어온 경우
		else
		{
			m_pNode->Data = Data;

			//	<--	자료 정렬
			NODE	*pParent = NULL;
			NODE	*pChild	= NULL;
			NODE	*pLChild = NULL;
			NODE	*pRChild = NULL;
		    
			pParent = m_pNode;
			pLChild = pParent->pLChild;
			pRChild = pParent->pRChild;
			while ( pLChild || pRChild )
			{
				//	<--	자식중 큰 놈을 선택
				if ( pRChild )		//	자식 둘 다 있을 때
				{
					if ( 0 < m_pfCompare ( pLChild->Data, pRChild->Data ) )
					{
						pChild = pRChild;
					}
					else
					{
						pChild = pLChild;
					}
				}
				else if ( pLChild )	//	자식 왼쪽만 있을 때
				{
					pChild = pLChild;
				}
				//	-->	자식중 큰 놈을 선택

				//	<--	자식과 부모의 비교
				if ( 0 < m_pfCompare ( pParent->Data, pChild->Data ) )
				{
					Swap ( pParent->Data, pChild->Data );
				}
				//	-->	자식과 부모의 비교

				pParent = pChild;
				pLChild = pParent->pLChild;
				pRChild = pParent->pRChild;
			}
			//	-->	자료 정렬

			return TRUE;            
		}
	}

	return FALSE;
}



#endif	//	__C_HEAP__
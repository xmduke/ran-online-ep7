//	Note : 최초 제작자 Per Nilsson. www.codeguru.com 에 
//		[http://www.codeguru.com/algorithms/BinTree.html]
//
//	수정 ( 2002.11.22 ) - JDH
//

#ifndef _BINTREE_H_
#define _BINTREE_H_



class CBinTreeNode;

//	Note : TraverseCallBack , 는 다음과 같은 형식으로 선언됨,
//		void somefunction(CBinTreeNode*,void*);  
//		CBinTree::Traverse method 에 인수로 사용됨.
//		바이너리 트리의 노드를 모두 탐색하고자 할때 사용.
//
typedef void (*TraverseCallBack) ( CBinTreeNode*, void* );

//	Note : CBinTreeNode, 는 바이너리 트리의 노드의 기져 클래스로
//		사용되며 트리의 노드가 되기 위해서는 이 클래스를 상속 받아야 한다.
//
class CBinTreeNode
{
	CBinTreeNode* m_LeftChild;
	CBinTreeNode* m_RightChild;

	CBinTreeNode* m_Parent;

public:
	CBinTreeNode () :
		m_LeftChild(NULL),
		m_RightChild(NULL),
		m_Parent(NULL)
	{
	}
	virtual ~CBinTreeNode () {}

public:
	CBinTreeNode* GetLeftChild () const		{ return m_LeftChild; }
	CBinTreeNode* GetRightChild () const	{ return m_RightChild; }
	CBinTreeNode* GetParent () const		{ return m_Parent; }

public:
	void SetLeftChild ( CBinTreeNode* p )	{ m_LeftChild = p; }
	void SetRightChild ( CBinTreeNode* p )	{ m_RightChild = p; }
	void SetParent ( CBinTreeNode* p )		{ m_Parent = p; }
};

//	Note : CBinTree. 바니너리 트리 기져 클래스.
//		반드시 상속 받아서 사용되며 순수 가상 함수들을 만들어주어야한다.
//
class CBinTree
{
protected:
	CBinTreeNode*		m_Root;

	TraverseCallBack	m_Func;

	void*				m_Param;
	LPVOID				m_pVoidSearchNode;
	CBinTreeNode*		m_pSearchNode;

	int					m_Comparisons;
	int					m_Count;
	int					m_Height;
	int					m_HeightTmp;

public:
	//	TraverseOrder. Input parameter to the Traverse function.
	//	Specifies in what way the tree should be traversed.
    //	Ascending   : 1,2,3,4,5....
	//	Descedning  : 9,8,7,6,5....
	//	ParentFirst : The parent node will be handeled before its children.
	//				Typically use when the structure is saved, so that
	//				the (possibly balanced) structure wont be altered.
	//	ParentLast  : The parent node will be handeled after its children.
	//				Typically use when tree is deleted; got to delete the 
	//				children before deleting their parent.
	//
	enum TraverseOrder
	{
		Ascending	= 0,
		Descending	= 1,
		ParentFirst	= 2,
		ParentLast	= 3
	};

	//	Constructor.
	CBinTree () :
		m_Root(NULL),
		m_Comparisons(0),
		m_Count(0),
		m_Height(0)
	{
	}


	//	Insert. Adds a node to the tree at the right place.
	void Insert ( CBinTreeNode* pNode );

	//	Return the first CBinTreeNode in the tree where
	//	Compare (node,pSearchNode)==0, or NULL if not found.
	CBinTreeNode* Find ( CBinTreeNode* pSearchNode );
	CBinTreeNode* Find ( LPVOID pSearchNode );

	//	Remove a node.Return non-zero if the node could
	//	be found in the tree.
	//	The first node where Compare (node,pSearchNode)==0
	//	gets zapped.
	BOOL RemoveNode ( CBinTreeNode* pSearchNode );
	BOOL RemoveNode ( LPVOID pSearchNode );

	//	Returns the number of comparisons required for the last
	//	call to Find. Gives a hint on how balanced the tree is.
	int GetComparisons () const { return m_Comparisons; }

	//	Traverse will call the supplied function, func,  for every node in the tree,
	//	passing it a pointer to the node, so you can act opon it.
	//	func: The callback function, like void somefunction(CBinTreeNode*,void*);
	//	The pParam will also be passed to the function and is a pointer to something.
	//	You decide to what, or ignore if you dont need it.
	void Traverse ( TraverseOrder to, TraverseCallBack func, void* pParam=NULL );

	//	Number of nodes in the tree.
	int GetCount () const { return m_Count; }

	//	The height of the tree, indicates how balanced it is.
	//	The height is the maximum number of comparisons needed to be
	//	made (worst case) when searching for an element.
	int GetHeight () const { return m_Height; }

	//	Balance minimizes the height, optimizing it.
	void Balance ();

	//	These two thingies are temp. stuff used in balancing.
	CBinTreeNode**	m_BalArray; // Array of pointers to nodes
	int				m_BalArrayCount; 

protected:
	//	Compare:
	//	p1 < p2 shall return -1
	//	p1 = p2 shall return  0
	//	p1 > p2 shall return  1
	//	You have to redefine it in a subclass, CBinTree can't know
	//	what data is significant for comparison in your node 
	virtual int Compare ( CBinTreeNode* p1, CBinTreeNode* p2 ) const = 0;
	virtual int Compare ( CBinTreeNode* p1, LPVOID p2 ) const			{ _ASSERTE(0&&"가상함수가 제작되지 않았습니다."); return 0; }

	//	Remove all nodes without deleting them.
	//	Not really hard now is it. 
	virtual void Clear () { m_Root = NULL; m_Count=0;m_Height=0;}

	//	Override if you want to take some special actions when a 
	//	node gets removed from the tree.
	virtual void OnRemoveNode ( CBinTreeNode* pNode ) {};
	
	//	Called by Insert.
	void InsertBelow ( CBinTreeNode* pParent,CBinTreeNode* pNewNode );

	//	Called by Traverse. All similar except for the order in which they call the children.
	void DoTraverse_Ascending ( CBinTreeNode* pNode );
	void DoTraverse_Descending ( CBinTreeNode* pNode );
	void DoTraverse_ParentFirst ( CBinTreeNode* pNode );
	void DoTraverse_ParentLast ( CBinTreeNode* pNode );


	//	Called by Find. Does the real work.
	CBinTreeNode* DoTraverse_Find ( CBinTreeNode* pNode );
	CBinTreeNode* DoTraverse_FindEx ( CBinTreeNode* pNode );

	//	Called by Balance. 
	void GetFromOrderedArray ( int low, int hi );
};

inline BOOL IsUnBalance ( int nCount, int nHeight, int dxLevel )
{
	int nLevel = (int) ( log10(static_cast<float>(nCount)) / log10(static_cast<float>(2)) + 1 );
	return ( nHeight > nLevel+dxLevel );
}

#endif // _BINTREE_H_


//	Note : 
//		수정 (2002.11.22) - JDH
//		CBinTree::Balance () 함수내에
//			delete mBalArray; -> delete[] mBalArray;
//
#include "stdafx.h"

#include "BinTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void CBinTree::Insert ( CBinTreeNode* pNode )
{
	if ( m_Root==NULL )
	{
		m_Root = pNode;
		m_Count = 1;
		m_Height = 1;
		m_Root->SetParent ( NULL );
	}
	else
	{
		m_HeightTmp = 1;
		InsertBelow ( m_Root, pNode );
		m_Count++;

		if ( m_HeightTmp>m_Height )	m_Height = m_HeightTmp;
	}
}

void CBinTree::InsertBelow ( CBinTreeNode* m_Parent, CBinTreeNode* m_NewNode )
{
	int i = Compare ( m_NewNode, m_Parent );

	m_HeightTmp++;
	switch (i)
	{
	case -1: 
		// mNewNode < mParent
		if ( m_Parent->GetLeftChild()==NULL )
		{
			// No left child? Okie then, mNewNode is the new left child 
			m_Parent->SetLeftChild ( m_NewNode );
			m_NewNode->SetParent ( m_Parent );
		}
		else
		{
			InsertBelow ( m_Parent->GetLeftChild(), m_NewNode );
		};
		break;

	case 0:
	case 1:
		// mNewNode >= mParent
		if ( m_Parent->GetRightChild()==NULL )
		{
			// No right child? Okie then, mNewNode is the new right child 
			m_Parent->SetRightChild ( m_NewNode );
			m_NewNode->SetParent ( m_Parent );
		}
		else
		{
			InsertBelow ( m_Parent->GetRightChild(), m_NewNode );
		};
		break;

	default:
		GASSERT(FALSE);
	};
}

void CBinTree::Traverse ( TraverseOrder to, TraverseCallBack func, void* pParam )
{
  m_Func = func;
  m_Param = pParam;

  switch (to)
  {
  case Ascending:
    DoTraverse_Ascending ( m_Root );
	break;

  case Descending:
    DoTraverse_Descending ( m_Root );
	break;

  case ParentFirst:
    DoTraverse_ParentFirst ( m_Root );
	break;

  case ParentLast:
    DoTraverse_ParentLast ( m_Root );
	break;

  default:
	  GASSERT(FALSE);
  };
}

void CBinTree::DoTraverse_Ascending ( CBinTreeNode* pNode )
{
	if (!pNode)	return;

	DoTraverse_Ascending ( pNode->GetLeftChild() );
	
	m_Func ( pNode, m_Param );

	DoTraverse_Ascending ( pNode->GetRightChild() );
}

void CBinTree::DoTraverse_Descending ( CBinTreeNode* pNode )
{
	if (!pNode)	return;

	DoTraverse_Descending ( pNode->GetRightChild() );
	
	m_Func ( pNode, m_Param );

	DoTraverse_Descending ( pNode->GetLeftChild() );
}

void CBinTree::DoTraverse_ParentFirst ( CBinTreeNode* pNode )
{
	if (!pNode)	return;

	m_Func ( pNode, m_Param );

	DoTraverse_ParentFirst ( pNode->GetLeftChild() );
	DoTraverse_ParentFirst ( pNode->GetRightChild() );
}

void CBinTree::DoTraverse_ParentLast(CBinTreeNode* pNode)
{
	if (!pNode)	return;

	DoTraverse_ParentLast ( pNode->GetLeftChild() );
	DoTraverse_ParentLast ( pNode->GetRightChild() );
	m_Func ( pNode, m_Param );
}

CBinTreeNode* CBinTree::Find ( CBinTreeNode* pSearchNode )
{
	m_pSearchNode = pSearchNode;
	m_Comparisons = 0;

	return DoTraverse_Find ( m_Root );
}

// DoTraverse_Find will, unlike the other DoTraverse_xxx, not 
// go through _all_ nodes, but stop when node is found or 
// is decided can't be found.

CBinTreeNode* CBinTree::DoTraverse_Find ( CBinTreeNode* node )
{
	// Reached a dead end, node couldn't be found.
	if (!node)	return NULL;

	m_Comparisons++;
	int iComp = Compare ( node, m_pSearchNode );

	// Found the node we were looking for, return it.
	if ( iComp == 0 )	return node;

	// node > mpSearchNode, look if it is by the left 
	if ( iComp > 0 )
		return DoTraverse_Find ( node->GetLeftChild() );

	// node < mpSearchNode, look if it is by the right
	// if (iComp < 0)
	return DoTraverse_Find ( node->GetRightChild() );
}

// tcb_Balance: TraverseCallBack
// Add the node into the array.
// pParam is the tree (so we can get the array)
void tcb_Balance ( CBinTreeNode* pNode, void* pParam )
{
	CBinTree* pTree = (CBinTree*) pParam;
	pTree->m_BalArray[pTree->m_BalArrayCount] = pNode;
	pTree->m_BalArrayCount++;
}

// Bring balance to the force.
void CBinTree::Balance ()
{
    // Setup an array that will hold the nodes
	m_BalArray = new CBinTreeNode*[m_Count];
	m_BalArrayCount=0;

	// Put the nodes into the array in ascending order (ie sorted)
    Traverse ( Ascending, tcb_Balance, this );

    // Clarifying the array now holds all the elements
	GASSERT ( m_Count == m_BalArrayCount );

	// Remove the nodes from the tree (easily done).
	// We will put 'em back soon enough.
	CBinTree::Clear ();

	// Reset the nodes so they don't have any children,
	// they will be given new as nodes get inserted back into to the tree.
	for ( int i=0; i<m_BalArrayCount; i++ )
	{
		m_BalArray[i]->SetLeftChild(NULL);
		m_BalArray[i]->SetRightChild(NULL);
		m_BalArray[i]->SetParent(NULL);
	}

	// Insert the nodes back to the tree in a balanced fashion.
	GetFromOrderedArray ( 0, m_BalArrayCount-1 );

    // Clarifying all elements have been inserted back from the array
	GASSERT(m_Count == m_BalArrayCount);

	delete[] m_BalArray;
}

// DoBalance.
// Insert the node in the middle position between 
// low and hi from the mBalArray array. 
// Recurse and the array elements < middlePos and > middlePos.
void CBinTree::GetFromOrderedArray ( int low, int hi )
{
	if (hi<low)	return;

	int middlePos;
	middlePos = low+(hi-low)/2;

	Insert ( m_BalArray[middlePos] );

	GetFromOrderedArray ( low, middlePos-1 );
	GetFromOrderedArray ( middlePos+1, hi );
}

BOOL CBinTree::RemoveNode ( CBinTreeNode* pSearchNode )
{
	CBinTreeNode* pNode = Find ( pSearchNode );
	if (!pNode)	return FALSE;

	int iCount = m_Count;

	CBinTreeNode* pParent = pNode->GetParent ();

	// Ok, so it has a parent, then we'll simply just disconnect it.
	if (pParent)
	{
		if ( pParent->GetLeftChild() == pNode )
		{
			pParent->SetLeftChild (NULL);
		}
		else
		{
			GASSERT(pParent->GetRightChild()==pNode);
			pParent->SetRightChild (NULL);
		}
	}
	else
	{
		// No parent? Then we're deleting the root node.
		GASSERT(pNode==m_Root);
		m_Root = NULL;
	}

	// Disconnected, now we reconnect its children (if any)
	// just by adding them as we add any other node. Their
	// respective children will come along, since Insert doesnt
	// tamper with the inserted node's children.
	if ( pNode->GetLeftChild() )	Insert ( pNode->GetLeftChild() );
	if ( pNode->GetRightChild() )	Insert ( pNode->GetRightChild() );

	m_Count = iCount-1;

	// Give the subclass a chance to do stuff to the removed node.
	OnRemoveNode ( pNode );

	return TRUE;
}

BOOL CBinTree::RemoveNode ( LPVOID pSearchNode )
{
	CBinTreeNode* pNode = Find ( pSearchNode );
	if (!pNode)	return FALSE;

	int iCount = m_Count;

	CBinTreeNode* pParent = pNode->GetParent ();

	// Ok, so it has a parent, then we'll simply just disconnect it.
	if (pParent)
	{
		if ( pParent->GetLeftChild() == pNode )
		{
			pParent->SetLeftChild (NULL);
		}
		else
		{
			GASSERT(pParent->GetRightChild()==pNode);
			pParent->SetRightChild (NULL);
		}
	}
	else
	{
		// No parent? Then we're deleting the root node.
		GASSERT(pNode==m_Root);
		m_Root = NULL;
	}

	// Disconnected, now we reconnect its children (if any)
	// just by adding them as we add any other node. Their
	// respective children will come along, since Insert doesnt
	// tamper with the inserted node's children.
	if ( pNode->GetLeftChild() )	Insert ( pNode->GetLeftChild() );
	if ( pNode->GetRightChild() )	Insert ( pNode->GetRightChild() );

	m_Count = iCount-1;

	// Give the subclass a chance to do stuff to the removed node.
	OnRemoveNode ( pNode );

	return TRUE;
}

CBinTreeNode* CBinTree::Find ( LPVOID pSearchNode )
{
	m_pVoidSearchNode = pSearchNode;
	m_Comparisons = 0;

	return DoTraverse_FindEx ( m_Root );	
}

CBinTreeNode* CBinTree::DoTraverse_FindEx ( CBinTreeNode* node )
{
	// Reached a dead end, node couldn't be found.
	if (!node)	return NULL;

	m_Comparisons++;
	int iComp = Compare ( node, m_pVoidSearchNode );

	// Found the node we were looking for, return it.
	if ( iComp == 0 )	return node;

	// node > mpSearchNode, look if it is by the left 
	if ( iComp > 0 )
		return DoTraverse_FindEx ( node->GetLeftChild() );

	// node < mpSearchNode, look if it is by the right
	// if (iComp < 0)
	return DoTraverse_FindEx ( node->GetRightChild() );
}
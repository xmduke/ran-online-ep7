#include "StdAfx.h"

#include "patchbintree.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

CPatchBinNode::CPatchBinNode ()
{
	m_pFile = NULL;
	m_bUse = FALSE;
}

CPatchBinNode::~CPatchBinNode ()
{
	SAFE_DELETE ( m_pFile );
	m_bUse = FALSE;
}

BOOL		CPatchBinNode::SetFile ( SFILENODE* pFile )
{
	if ( m_pFile )
	{
		return FALSE;
	}

	m_pFile = new SFILENODE;
	strcpy ( m_pFile->FileName, pFile->FileName );
	strcpy ( m_pFile->SubPath, pFile->SubPath );
	m_pFile->Ver = pFile->Ver;

	return TRUE;
}

SFILENODE*	CPatchBinNode::GetFile ( void )
{
	return m_pFile;
}

void		CPatchBinNode::SetUseFlag ( void )
{
	m_bUse = TRUE;
}

BOOL		CPatchBinNode::GetUseFlag ( void )
{
	return m_bUse;
}


CPatchBinTree::CPatchBinTree(void)
{
}

CPatchBinTree::~CPatchBinTree(void)
{
	Clear ();
}

// tcb_Clear : TraverseCallBack. Delete the node
static void tcb_Clear ( CBinTreeNode* p, void* pParam )
{	
	SAFE_DELETE(p);
}

// tcb_Traverse : TraverseCallBack. Delete the node
static void tcb_GetNode ( CBinTreeNode* p, void* pParam )
{
	CPatchBinNode* pNode = (CPatchBinNode*)p;
	if ( pNode->GetUseFlag () )
	{
		return;
	}

	SFILENODE* pFile = pNode->GetFile();

	FILEVECTOR* pVector = (FILEVECTOR*)pParam;
	pVector->push_back ( pFile );
}

void CPatchBinTree::Clear ()
{
	// ParentLast, so child is deleted prior its parent
	Traverse ( ParentLast, tcb_Clear, this );
	CBinTree::Clear ();
}

int CPatchBinTree::Compare ( CBinTreeNode* p1,CBinTreeNode* p2 ) const
{
	assert(p1);
	assert(p2);

	SFILENODE* pFile1 = ((CPatchBinNode*)p1)->GetFile();
	SFILENODE* pFile2 = ((CPatchBinNode*)p2)->GetFile();

	int Value1 = stricmp ( pFile1->FileName, pFile2->FileName );
	int Value2 = stricmp ( pFile1->SubPath, pFile2->SubPath );


	BOOL	bDiff = Value1 || Value2;
	if ( !bDiff )
	{
		return bDiff;
	}
	else
	{
		return Value1;
	}
}


void CPatchBinTree::Add ( SFILENODE* pFile )
{
    CPatchBinNode* pNode = new CPatchBinNode;
	pNode->SetFile ( pFile );
	CBinTree::Insert ( (CPatchBinNode*)pNode );
}

BOOL CPatchBinTree::Del ( SFILENODE* pFile )
{
	CPatchBinNode searchNode;	// The int part of the node is
								// ignored in comparison
	searchNode.SetFile ( pFile );

	return RemoveNode ( &searchNode );
}

CPatchBinNode* CPatchBinTree::Find ( SFILENODE* pFile )
{
	CPatchBinNode searchNode;	// The int part of the node is
								// ignored in comparison
	searchNode.SetFile ( pFile );

	CPatchBinNode* pRes = (CPatchBinNode*) CBinTree::Find ( &searchNode );
  
	return pRes;
}

void CPatchBinTree::GetNotUseItem ( FILEVECTOR* pNotUseItems )
{
	if ( !pNotUseItems )
	{
		return;
	}

	Traverse ( ParentFirst, tcb_GetNode, pNotUseItems );
}
#include "StdAfx.h"
#include ".\patchset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPatchSetNode::CPatchSetNode(void) :
	m_pFile(NULL),
	m_bUse(FALSE)
{
}

CPatchSetNode::~CPatchSetNode(void)
{
	SAFE_DELETE ( m_pFile );
	m_bUse = FALSE;
}

BOOL CPatchSetNode::SetFile ( SFILENODE* pFile )
{
	if ( m_pFile )
		return FALSE;

	m_pFile = new SFILENODE;

	StringCchCopy ( m_pFile->FileName, FILENAME_SIZE, pFile->FileName );
	StringCchCopy ( m_pFile->SubPath, SUBPATH_SIZE, pFile->SubPath );
	m_pFile->Ver = pFile->Ver;
	StringCchCopy ( m_pFile->szMD5, MD5_SIZE, pFile->szMD5 );

	return TRUE;
}

////////////////////////////////////////////////////////////////////////

CPatchSet::CPatchSet(void)
{
}

CPatchSet::~CPatchSet(void)
{
	Clear();
}

void CPatchSet::Clear()
{
	SETPATCH_ITER iter = m_setPatch.begin();
	SETPATCH_ITER iter_end = m_setPatch.end();
	
	for( ; iter != iter_end; ++iter )
	{
		delete *iter;
	}

	m_setPatch.clear();
}

void CPatchSet::Add( SFILENODE* pFile )
{
	CPatchSetNode * pNode = new CPatchSetNode;
	pNode->SetFile( pFile );
	m_setPatch.insert( pNode ); // MEMO : 중복 가능성 체크...?
}

CPatchSetNode* CPatchSet::Find( SFILENODE* pFile )
{
	CPatchSetNode searchNode;
	searchNode.SetFile( pFile );

	SETPATCH_ITER iter = m_setPatch.find( &searchNode );
	if( iter == m_setPatch.end() )
		return NULL;
	else
		return *iter;
}

void CPatchSet::GetNotUseItem( FILEVECTOR* pNotUseItems )
{
	if( !pNotUseItems )
		return;

	SETPATCH_ITER iter = m_setPatch.begin();
	SETPATCH_ITER iter_end = m_setPatch.end();
	
	for( ; iter != iter_end; ++iter )
	{
		if( ! (*iter)->GetUseFlag() )
		{
			pNotUseItems->push_back( (*iter)->GetFile() );
		}
	}
}
#pragma once

#include "dxutil.h"
#include "patch.h"
#include "bintree.h"

class CPatchBinNode : public CBinTreeNode
{
public:
    CPatchBinNode ();
	virtual	~CPatchBinNode ();

public:
	BOOL		SetFile ( SFILENODE* pFile );
	SFILENODE*	GetFile ( void );

	void		SetUseFlag ( void );
	BOOL		GetUseFlag ( void );

protected:
	SFILENODE*	m_pFile;
	BOOL		m_bUse;
};


class CPatchBinTree : public CBinTree
{
public:
	CPatchBinTree(void);
	virtual ~CPatchBinTree(void);

protected:
	int Compare ( CBinTreeNode* p1,CBinTreeNode* p2 ) const;
	virtual void OnRemoveNode ( CPatchBinNode* pNode )	{ SAFE_DELETE(pNode); };

public:
	void Add ( SFILENODE* pFile );
	BOOL Del ( SFILENODE* pFile );
	void Clear ();

	CPatchBinNode* Find ( SFILENODE* pFile );
	void GetNotUseItem ( FILEVECTOR* pNotUseItems );
};
#pragma once

#include "PatchVariable.h"

class CPatchSetNode
{
public:
    CPatchSetNode(void);
	~CPatchSetNode(void);

public:
	BOOL		SetFile ( SFILENODE* pFile );
	SFILENODE*	GetFile ( void )				{ return m_pFile; }

	void		SetUseFlag ( void )				{ m_bUse = TRUE; }
	BOOL		GetUseFlag ( void )				{ return m_bUse; }

protected:
	SFILENODE*	m_pFile;
	BOOL		m_bUse;
};

class CCompare
{
public:
	bool operator() ( CPatchSetNode *p1, CPatchSetNode *p2 ) const
	{
		SFILENODE * pFile1 = p1->GetFile();
		SFILENODE * pFile2 = p2->GetFile();

		int Value1 = stricmp( pFile1->FileName, pFile2->FileName );
		int Value2 = stricmp( pFile1->SubPath, pFile2->SubPath );

        return (Value1<0) || ( !(0<Value1) && (Value2<0) );
	}
};

class CPatchSet
{
	typedef std::set<CPatchSetNode*, CCompare>	SETPATCH;
	typedef SETPATCH::iterator					SETPATCH_ITER;

protected:
	SETPATCH m_setPatch;

public:
	void Add( SFILENODE* pFile );
	//BOOL Del( SFILENODE* pFile );
	void Clear();

	CPatchSetNode* Find( SFILENODE* pFile );
	void GetNotUseItem( FILEVECTOR* pNotUseItems );

public:
	CPatchSet(void);
	~CPatchSet(void);
};
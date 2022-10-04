#include "StdAfx.h"
#include ".\gldescriptionfile.h"
#include "gltexfile.h"
#include "GLOGIC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

GLDescriptionFile::GLDescriptionFile(void)
{
}

GLDescriptionFile::~GLDescriptionFile(void)
{
}

bool GLDescriptionFile::LOADFILE( const char * szFileName )
{
	if( strlen(szFileName) == 0 )	return FALSE;

	std::string strPath;
	strPath = GLOGIC::GetPath();
	strPath += szFileName;	

	gltexfile cFILE;
	cFILE.reg_sep( '\t' );
	cFILE.reg_sep( '[' );
	cFILE.reg_sep( ']' );
		
	if( !cFILE.open( strPath, TRUE ) )		return FALSE;

	//WORD dwVERSION = 0;
	//cFILE.getflag( "VERSION", 1, 1, dwVERSION );

	std::string strName, strDes;

	int nNUM = cFILE.getflagnum ( "NAME" );
	for ( int i=0; i<nNUM; ++i )
	{
		cFILE.getflag( i, "NAME", 1, 2, strName );
		cFILE.getflag( i, "NAME", 2, 2, strDes );

		// ¸Ê¿¡ Ãß°¡
		m_mapDes.insert( std::make_pair( strName, strDes ) );
	}
}

const char * GLDescriptionFile::GetDes( const char * szName )
{
	DESMAP_CITER cIter = m_mapDes.find( szName );

	if( cIter != m_mapDes.end() )
	{
		return cIter->second.c_str();
	}

	return NULL;
}
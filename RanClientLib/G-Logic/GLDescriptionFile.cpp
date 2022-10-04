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

GLDescriptionFile & GLDescriptionFile::GetInstance()
{
	static GLDescriptionFile Instance;
	return Instance;
}

void GLDescriptionFile::CLEAR()
{
	if( !m_mapName.empty() ) m_mapName.clear();
	if( !m_mapDesc.empty() ) m_mapDesc.clear();
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

	std::string strKey, strSub;

	int nNUM = cFILE.getflagnum ( "NAME" );
	for ( int i=0; i<nNUM; ++i )
	{
		cFILE.getflag( i, "NAME", 1, 2, strKey );
		cFILE.getflag( i, "NAME", 2, 2, strSub );

		// ¸Ê¿¡ Ãß°¡
		m_mapName.insert( std::make_pair( strKey, strSub ) );
	}

	nNUM = cFILE.getflagnum ( "DESC" );
	for ( int i=0; i<nNUM; ++i )
	{
		cFILE.getflag( i, "DESC", 1, 2, strKey );
		cFILE.getflag( i, "DESC", 2, 2, strSub );

		// ¸Ê¿¡ Ãß°¡
		m_mapDesc.insert( std::make_pair( strKey, strSub ) );
	}

	return TRUE;
}

const char * GLDescriptionFile::GetDesc( const char * szName )
{
	DESMAP_ITER Iter = m_mapDesc.find( szName );
	
	if( Iter != m_mapDesc.end() )
	{
		return Iter->second.c_str();
	}

	return NULL;
}
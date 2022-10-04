#pragma once
#include <map>

class GLDescriptionFile
{
	typedef std::map<std::string, std::string>	DESMAP;
	typedef DESMAP::iterator					DESMAP_ITER;
	typedef DESMAP::const_iterator				DESMAP_CITER;

	DESMAP m_mapDes;

public:
	bool LOADFILE( const char * szFileName );
	const char * GetDes( const char * szName );

public:
	GLDescriptionFile(void);
	~GLDescriptionFile(void);
};
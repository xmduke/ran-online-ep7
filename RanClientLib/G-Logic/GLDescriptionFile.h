#pragma once
#include <map>

class GLDescriptionFile
{
	typedef std::map<std::string, std::string>	DESMAP;
	typedef DESMAP::iterator					DESMAP_ITER;
	typedef DESMAP::const_iterator				DESMAP_CITER;

	DESMAP	m_mapName;
	DESMAP	m_mapDesc;

public:
	void CLEAR(void);
	bool LOADFILE( const char * szFileName );
	const char * GetDesc( const char * szName );

public:
	GLDescriptionFile(void);
	~GLDescriptionFile(void);

public:
	static GLDescriptionFile & GetInstance ();
};
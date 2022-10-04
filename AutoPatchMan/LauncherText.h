#pragma	once

#include "../EngineUIlib/GUInterface/GameTextLoader.h"

//	게임 텍스트
struct SLAUNCHERTEXT
{
	/////////////////////////////////////////////////////////////////////////////
	static const WORD progVersion = 0x0001;
	/////////////////////////////////////////////////////////////////////////////

	NS_TEXT_LOADER::STRFLAG		mapFlags;
};

const char * ID2LAUNCHERTEXT ( const char* szKeyword, int nIndex = 0 );

class CLauncherTextMan
{
public:

private:
	CLauncherTextMan ();
	virtual ~CLauncherTextMan ();

public:
	const char* GetPath ()				{ return m_szPath; }
	void SetPath( const char* szPath )	{ StringCchCopy(m_szPath,MAX_PATH,szPath); }

public:
	const CString & GetLauncherText( std::string strKey, int Index = 0 );
	const CString &	GetText( std::string strKey, int Index);
	int	GetNumber( std::string strKey, int Index);

public:
	void Clear ();
	BOOL LoadText ( const char* szFileName,BOOL bXML = FALSE );	
	BOOL DefaultSetText();

private:
	char m_szPath[MAX_PATH];

private:
    SLAUNCHERTEXT	m_LauncherText;

public:
	static CLauncherTextMan& GetInstance();
};
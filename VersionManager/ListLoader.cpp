#include "StdAfx.h"
#include "ListLoader.h"
#include "CCfg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CListLoader::CListLoader(const char* filename)
{
	SetDefault();
	Load(filename);
}

CListLoader::CListLoader(void)
{
	SetDefault();
}

CListLoader::~CListLoader(void)
{

}

void CListLoader::SetDefault(void)
{

}

int CListLoader::Load(const char* filename)
{
	// Data type problem
	// program directory + cfg + filename	
		
	FILE *oFile;
	char line[300];

	// Open for read 
	if ((oFile = ::fopen(filename, "r" )) == NULL)
	{
		return -1;
	}	

	// Read a line and process
	while (::fgets(line, 200, oFile))
	{
		Process(line);
	}
	
	// Close file
	if (::fclose( oFile ))
	{
		return -1;
	}	
	return 0;
}

int CListLoader::Process(char* sLine)
{
	// 토큰으로 분리할 분리자...
	// space, comma, tab, new line
	char seps[]   = ",\t\n";
	char *token;

	// Establish string and get the first token
	token = ::strtok(sLine, seps);	
	CString strTarget;
	strTarget.Format("%s", CCfg::GetInstance()->m_szTargetPath);

	while (token != NULL)
	{	
		// 주석일때는 무시...
		if (::strcmp(token, "//") == 0) 
		{
			return 0;
		} 
		//////////////////////////////////////////////////////////////////////
		// database setting value 
		else if (::strcmp(token, "source_path") == 0) 
		{
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szSourcePath, token);
			else
				return 0;
		}
		else
		{
			CString strTemp;
			if (token != NULL)
			{
				if (*token == '/' || *token == '\\') // 디렉토리임
				{					
					FolderList tmpFolder;
					tmpFolder.strName.Format("%s", token);
                    tmpFolder.strName.Replace('/', '\\');
					m_vFolderList.push_back(tmpFolder);
					m_strCrtPath.Format("%s", token);
                    m_strCrtPath.Replace('/', '\\');
				}
				else // 파일임
				{
					CompressFile tmpFile;
					
                    tmpFile.strFileName.Format("%s", token);
					
                    tmpFile.strSrc		= m_szSourcePath + m_strCrtPath;
                    tmpFile.strSrc.Replace('/', '\\');
                    tmpFile.strSrc      = tmpFile.strSrc + tmpFile.strFileName;

					tmpFile.strFileName += ".cab";

                    tmpFile.strPurePath = m_strCrtPath;
                    
					tmpFile.strTarget	= strTarget + m_strCrtPath;
                    tmpFile.strTarget.Replace('/', '\\');
                    tmpFile.strTarget = strTarget + m_strCrtPath;

					// Get next token
					token = ::strtok( NULL, seps );
					if (token != NULL)
					{
						// MD5						
						tmpFile.strMD5.Format( _T("%s"), token );
						//MessageBox( NULL, tmpFile.strMD5, "TEST", MB_OK );
					}

					// tmpFile.nDir	= m_nDir;
					m_vFile.push_back(tmpFile);
				}
			}
		}
		// Get next token
		// token = ::strtok( NULL, seps );
		return 0;
	}
	return 0;
}
#ifndef S_LOG_FILE_H_
#define S_LOG_FILE_H_

#include <time.h>
#include <vector>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef C_BUFFER_SIZE
#define C_BUFFER_SIZE 1024
#endif

#ifndef MAX_FILE_NAME_LENGTH
#define MAX_FILE_NAME_LENGTH 256
#endif

#ifndef MAX_PATH
#define MAX_PATH          260
#endif

class CLogFile 
{
public:
	CLogFile(CString strFileName);
	~CLogFile();

protected:		
	CString m_strFullFileName;
	CString m_strFileName;
	FILE*	m_pFile;
		
public:		
	void Write(char *szStr, ...);
	void WriteWithTime(char *szStr, ...);

protected:	
	CString GetAppPath();	
};


#endif // S_LOG_FILE_H_
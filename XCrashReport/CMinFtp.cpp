#include "stdafx.h"
#include "CMinFtp.h"
#include "CConsoleMessage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

CMinFtp::CMinFtp()
{
	m_pIsession     = NULL;
	m_pConnect      = NULL;
	m_pTbytes       = NULL;
	m_pfilesize     = NULL;
	m_pbForceTerminate = NULL;
    ::memset(LocalFullPath, 0, sizeof ( char ) * 128);
}

CMinFtp::~CMinFtp()
{		
	if (m_pConnect)
		delete m_pConnect;
	if (m_pIsession)
		delete m_pIsession;
}

void CMinFtp::SetProgressValue ( ULONGLONG* pTbytes, ULONGLONG* pfilesize )
{
	m_pTbytes   = pTbytes;
	m_pfilesize = pfilesize;
}

void CMinFtp::SetForceTerminateToggle ( BOOL* pForceTerminate )
{
	m_pbForceTerminate = pForceTerminate;
}

void CMinFtp::SetServerName(const char* strName)
{
    strcpy(ServerName, strName);
}

void CMinFtp::SetServerPort(int nPort)
{
	ServerPort = nPort;
}

void CMinFtp::SetUserID(const char* strID)
{
	strcpy(UserID, strID);
}

void CMinFtp::SetUserPass(const char* strPass)
{
	strcpy(UserPass, strPass);
}

void CMinFtp::SetServer(const char* strName, int nPort, const char* strID, const char* strPass)
{
	SetServerName(strName);
	SetServerPort(nPort);	
	SetUserID(strID);
	SetUserPass(strPass);
}

int CMinFtp::Connect()
{
    return Connect(ServerName, ServerPort, UserID, UserPass);
}

int CMinFtp::Connect(const char* strName, int nPort, const char* strID, const char* strPass)
{
	SetServerName(strName);
	SetServerPort(nPort);	
	SetUserID(strID);
	SetUserPass(strPass);

	m_pIsession = new CInternetSession("Patch Program");
	if(m_pIsession == NULL)
	{
		return NET_ERROR;
	}
	
	try {	
		m_pConnect = m_pIsession->GetFtpConnection(
				ServerName, 
				UserID, 
				UserPass, 
				ServerPort,
				TRUE);
	}
	catch(CInternetException* pEx)
	{
		TCHAR szCause[255];
		pEx->GetErrorMessage(szCause, 255);
		CConsoleMessage::GetInstance()->Write("ERROR:CMinFtp::Connect %s", szCause);
		pEx->Delete();
		m_pIsession->Close();
		delete m_pIsession;
		m_pIsession = NULL;
		return NET_ERROR;		
	}
	return NET_OK;
}

void CMinFtp::DisConnect()
{
	m_pConnect->Close();
}

// ftp 의 현재 디렉토리를 변경한다.
int CMinFtp::SetCurrentDirectory(LPCTSTR pstrDirName)
{
	int nRetCode = 0;
	if (m_pConnect)
		nRetCode = m_pConnect->SetCurrentDirectory(pstrDirName);

	if (nRetCode == 0)
		return NET_ERROR;
	else
		return NET_OK;
}

// 현재 ftp 디렉토리 위치를 반환한다.
int CMinFtp::GetCurrentDirectory( LPTSTR pstrDirName, LPDWORD lpdwLen)
{
	int nRetCode = 0;
	if (m_pConnect)
		nRetCode = GetCurrentDirectory(pstrDirName, lpdwLen);

	if (nRetCode == 0)
		return NET_ERROR;
	else
		return NET_OK;
}

// ftp 의 현재 디렉토리에서 해당 파일을 가져온다.
int CMinFtp::GetFile(const char* filename)
{
	int			nRetCode = 0;	// Return code
	int			nPercent = 0;
	ULONGLONG	m_filesize = 0; // File Size	
	HANDLE		hFile = NULL;	//New File	
	UINT		m_iReadBufSize = 0; 
	UCHAR		m_rBuf[512];	// Receive Buffer
	DWORD		dwWritten;		// Received Bytes
	ULONGLONG	dwTbytes = 0;	// Received Total Bytes
	CString		str;			// Temp String


	if ( !m_pbForceTerminate )
	{
		return NET_ERROR;
	}

	
	//Open From FTP Site.
	CInternetFile* pInternetFile = NULL;
	CFtpFileFind finder(m_pConnect);

	nRetCode = finder.FindFile(filename, INTERNET_FLAG_RELOAD);
	if (nRetCode == 0)
	{
        // CMinFtp::GetFile() finder.FindFile Error
		return NET_ERROR;
	}
	finder.FindNextFile();

	TRY {	
		pInternetFile = m_pConnect->OpenFile(filename, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 1);
	}
	CATCH (CInternetException, pEx)
	{	
		char szError[256];
		pEx->GetErrorMessage(szError, 256);
		CConsoleMessage::GetInstance()->Write("ERROR:CMinFtp::GetFile m_pConnect->OpenFile %s", szError);
		delete m_pIsession;
		m_pIsession = NULL;
		return NET_ERROR;
	}
	END_CATCH

	if (pInternetFile == NULL)
	{		
		return NET_ERROR;	
	}
	
	//m_filesize = pInternetFile->GetLength();
	m_filesize = finder.GetLength();
	if ( m_pfilesize )
	{
		*m_pfilesize = m_filesize;
	}
	
	// Create NewFile
	char szFullPathFile[256];
	strcpy ( szFullPathFile, LocalFullPath );
	strcat ( szFullPathFile, filename );
	hFile = CreateFile(szFullPathFile, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	while ( !(*m_pbForceTerminate) )
	{
		pInternetFile->SetWriteBufferSize(512);
		pInternetFile->SetReadBufferSize(512);		
				
		// Read File to Buffer step by 100 bit.
		m_iReadBufSize = pInternetFile->Read(m_rBuf, 512);		
		
		if (m_iReadBufSize == 0)
		{
			// end of file.
			break;
		}
		else
		{
			// Write file from buffer.
			WriteFile(hFile, m_rBuf, m_iReadBufSize, &dwWritten, NULL);
			dwTbytes += dwWritten;			
			// Move Progress Bar.
			if ( m_pTbytes )
			{
				*m_pTbytes = dwTbytes;
			}
		}		
	}

	// Close File
	CloseHandle(hFile);
	pInternetFile->Close();
	delete pInternetFile;

	if ( *m_pbForceTerminate )	//	강제 종료
	{
		return NET_ERROR;
	}
	else
	{
		return NET_OK;
	}
}

// ftp 의 현재 디렉토리에 해당 파일을 업로드한다.
int CMinFtp::PutFile(CString strPath, const char* filename)
{	
    HANDLE hFile = NULL; //Opened File	
	
    // 파일을 Read Open 한다. 
   	hFile = CreateFile(strPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        TRACE("CreateFile:FILE OPEN ERROR \n");
        return NET_ERROR;
    }    

    //Open From FTP Site.
	CInternetFile* pInternetFile = NULL;
	
	TRY {	
		pInternetFile = m_pConnect->OpenFile(filename, GENERIC_WRITE, FTP_TRANSFER_TYPE_BINARY, 1);
	}
	CATCH (CInternetException, pEx) {
        TRACE("m_pConnect->OpenFile Exception \n");
		char szError[256];
		pEx->GetErrorMessage(szError, 256);
		CConsoleMessage::GetInstance()->Write("ERROR:CMinFtp::PutFile m_pConnect->OpenFile %s", szError);
        CloseHandle(hFile);
		return NET_ERROR;
	}
	END_CATCH

	if (pInternetFile == NULL)
	{		
        CloseHandle(hFile);
		return NET_ERROR;	
	}
	
    char inBuffer[1024];
    DWORD dwBytesRead;
    BOOL bResult;

	while (true)
	{
		/*pInternetFile->SetWriteBufferSize(512);
		pInternetFile->SetReadBufferSize(512);*/

        // Attempt a synchronous read operation.
        bResult = ReadFile(hFile, inBuffer, 1024, &dwBytesRead, NULL); 
        // Check for end of file. 
        if (bResult &&  dwBytesRead == 0) 
        { 
            // we're at the end of the file
            break;
        }
				
		// Write File to Buffer step by 512 bytes.
        TRY 
		{
            pInternetFile->Write(inBuffer, dwBytesRead);            
        }
        CATCH (CInternetException, pEx) 
		{
            
            DWORD dError = GetLastError(); //pEx->m_dwError;            
            TRACE("pInternetFile->Write Exception \n");
			char szError[256];
			pEx->GetErrorMessage(szError, 256);
			CConsoleMessage::GetInstance()->Write("ERROR:CMinFtp::PutFile pInternetFile->Write %s", szError);
            CloseHandle(hFile);
            pInternetFile->Close();
            delete pInternetFile;	
		    return NET_ERROR;
	    }
	    END_CATCH
        Sleep(0);
	}
	// Close File
	CloseHandle(hFile);
    pInternetFile->Flush();
	pInternetFile->Close();
	delete pInternetFile;	
    return NET_OK;
}

void CMinFtp::SetLocalDir( char* szLocalFullPath )
{
	strcpy ( LocalFullPath, szLocalFullPath );
}

BOOL CMinFtp::CreateDirectory(CString strName)
{
    return m_pConnect->CreateDirectory(strName);
}

void CMinFtp::Command(CString strCmd)
{    
    m_pConnect->Command(strCmd, CFtpConnection::CmdRespNone, FTP_TRANSFER_TYPE_ASCII, 1);
}
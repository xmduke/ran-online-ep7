#ifndef S_CMINFTP_H_
#define S_CMINFTP_H_

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif

#include <afxinet.h>

#ifndef NET_OK
#define NET_OK 1
#endif

#ifndef NET_ERROR
#define NET_ERROR -1
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif

// This patch process use MFC
// StdAfx.h must have below line
// #include "stdafx.h"
// Windows NT Use version 4.0. Implemented as ANSI and Unicode functions. 
// Windows Use Windows 95 and later. Implemented as ANSI and Unicode functions. 
// Header Wininet.h 
// Import library Wininet.lib 
// Minimum availability Internet Explorer 3.0 (ANSI only), 5 (ANSI and Unicode) 
#pragma comment (lib,"Wininet.lib")
// #pragma message ("Auto linking Windows Internet library")
// #pragma message ("Client must have Internet Explorer 3.0 (ANSI only), 5 (ANSI and Unicode)")

class CMinFtp
{
public :	
	CMinFtp();
	~CMinFtp();
protected :
	char	ServerName[30];		// ftp server name
	int		ServerPort;			// ftp server port, default 21
	char	UserID[30];			// ftp server user id
	char	UserPass[30];			// ftp server user password

	char	LocalFullPath[128];		// ftp local path

	CInternetSession*	m_pIsession;

public :
    CFtpConnection*		m_pConnect;
	ULONGLONG*			m_pTbytes;
	ULONGLONG*			m_pfilesize;

private:
    BOOL*	m_pbForceTerminate;

public :
    int     Connect();
	int		Connect(const char* strName = "ftp.ran-online.co.kr", 
					int nPort = 21, 
					const char* strID = "Anonymous", 
					const char* strPass = "Anonymous@ran-online.co.kr");
	
	void	DisConnect();
	
	void	SetServer(const char* strName = "ftp.ran-online.co.kr", 
						int nPort = 21, 
						const char* strID = "Anonymous", 
						const char* strPass = "Anonymous@ran-online.co.kr");
	void	SetServerName(const char* strName = "ftp.ran-online.co.kr");
	void	SetServerPort(int nPort = 21);
	void	SetUserID(const char* strID = "Anonymous");
	void	SetUserPass(const char* strPass = "Anonymous@ran-online.co.kr");	
	int		SetCurrentDirectory(LPCTSTR pstrDirName);
	void	SetLocalDir( char* szLocalFullPath );
	void	SetProgressValue ( ULONGLONG* pTbytes, ULONGLONG* pfilesize );
	void	SetForceTerminateToggle ( BOOL* pForceTerminate );
	
	int		GetCurrentDirectory(LPTSTR pstrDirName, LPDWORD lpdwLen);
    BOOL    CreateDirectory(CString strName);
	int		GetFile(const char* filename);	
    int		PutFile(CString strPath, const char* filename);	
    void    Command(CString strCmd);
};

#endif // S_CMINFTP_H_
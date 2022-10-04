// By 경대
//
#pragma once

#include <wininet.h>

class CRanFtp
{
protected:
	HINTERNET m_hInternet;
	HINTERNET m_hFtp;

	CString			m_strServer;
	INTERNET_PORT	m_nPort;
	CString			m_strUser;
	CString 		m_strPass;

	CString			m_strDate;
	CString			m_strTime;

public: // FTP 관련 함수
	BOOL Connect( LPCTSTR lpszAgent, LPCTSTR lpszServerName, INTERNET_PORT nServerPort,
				LPCTSTR lpszUsername, LPCTSTR lpszPassword );

	void DisConnect();
	BOOL SetCurrentDirectory( LPCTSTR lpszDirectory );
	BOOL GetCurrentDirectory( CString & strDirName ) const;
	BOOL PutFile( LPCTSTR lpszLocalFile, LPCTSTR lpszNewRemoteFile );
	BOOL CreateDirectory( LPCTSTR lpszDirectory );

public:
	BOOL GetIPAdress( CString & strIPAdress );
	BOOL GetServerTime(LPCTSTR pszHostName, int nPort = 123);
	BOOL GetServerDateString( CString & strServerDate )
	{
		if( m_strDate.IsEmpty() ) return FALSE;
		strServerDate = m_strDate;
		return TRUE;
	}

	BOOL GetServerTimeString( CString & strServerTime )
	{
		if( m_strTime.IsEmpty() ) return FALSE;
		strServerTime = m_strTime;
		return TRUE;
	}
	
public:
	CRanFtp(void);
	~CRanFtp(void);
};
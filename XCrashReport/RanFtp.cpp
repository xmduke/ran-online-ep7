#include "stdafx.h"
#include ".\ranftp.h"
#include "iphlpapi.h"
#include "sntp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CRanFtp::CRanFtp(void)
{
}

CRanFtp::~CRanFtp(void)
{
}

BOOL CRanFtp::Connect( LPCTSTR lpszAgent, LPCTSTR lpszServerName, INTERNET_PORT nServerPort,
					LPCTSTR lpszUsername, LPCTSTR lpszPassword )
{
	m_hInternet = InternetOpen( lpszAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
	if ( m_hInternet == NULL ) return FALSE;

	m_hFtp = InternetConnect( m_hInternet, lpszServerName, INTERNET_DEFAULT_FTP_PORT,
							lpszUsername, lpszPassword, INTERNET_SERVICE_FTP, 0, 0 );
	if ( m_hFtp == NULL )
	{
		InternetCloseHandle(m_hInternet);
		return FALSE;
	}

	m_strServer = lpszServerName;
	m_nPort = nServerPort;
	m_strUser = lpszUsername;
	m_strPass = lpszPassword;

	return TRUE;
}

void CRanFtp::DisConnect()
{
	InternetCloseHandle(m_hFtp);
	InternetCloseHandle(m_hInternet);
	m_hFtp = NULL;
	m_hInternet = NULL;
}

BOOL CRanFtp::SetCurrentDirectory( LPCTSTR pstrDirName )
{
	//ASSERT_VALID(this);
	ASSERT(m_hFtp != NULL);
	ASSERT(AfxIsValidString(pstrDirName));

	return FtpSetCurrentDirectory(m_hFtp, pstrDirName);
}

BOOL CRanFtp::GetCurrentDirectory( CString & strDirName) const
{
	//ASSERT_VALID(this);
	ASSERT(m_hFtp != NULL);

	DWORD dwLen = INTERNET_MAX_PATH_LENGTH;
	LPTSTR pstrTarget = strDirName.GetBufferSetLength(dwLen);
	BOOL bRet = FtpGetCurrentDirectory(m_hFtp, pstrTarget, &dwLen);

	if (bRet)
		strDirName.ReleaseBuffer(dwLen);
	else
		strDirName.ReleaseBuffer(0);

	return bRet;
}

BOOL CRanFtp::PutFile( LPCTSTR lpszLocalFile, LPCTSTR lpszNewRemoteFile )
{
	ASSERT(m_hFtp != NULL);
	ASSERT(AfxIsValidString(lpszNewRemoteFile));
	ASSERT(AfxIsValidString(lpszLocalFile));
	//ASSERT((dwFlags & INTERNET_FLAG_ASYNC) == 0);

	return FtpPutFile( m_hFtp, lpszLocalFile, lpszNewRemoteFile, 
						FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, 0 );			
}

BOOL CRanFtp::CreateDirectory( LPCTSTR pstrDirName )
{
	ASSERT(m_hFtp != NULL);
	ASSERT(AfxIsValidString(pstrDirName));

	return FtpCreateDirectory( m_hFtp, pstrDirName );
}

BOOL CRanFtp::GetIPAdress( CString & strIPAdress )
{
	IP_ADAPTER_INFO		*pAdapterInfo;
	IP_ADAPTER_INFO		*pAdapter;

	pAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO) );
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS)
	{
		free(pAdapterInfo);
		//pAdapterInfo = (IP_ADAPTER_INFO *) malloc ( sizeof(ulOutBufLen) );
		return FALSE;
	}

	DWORD dwRetVal;
	if (( dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen ) ) != NO_ERROR )
	{
		free(pAdapterInfo);
		return FALSE;
	}
	pAdapter = pAdapterInfo;

	//while (pAdapter) {
	//	pAdapter->IpAddressList.IpAddress.String;

	//	pAdapter = pAdapter->Next;
	//}

	if ( !pAdapter ) return FALSE;

	strIPAdress = pAdapter->IpAddressList.IpAddress.String;

	free (pAdapterInfo);
	return TRUE;
}

BOOL CRanFtp::GetServerTime(LPCTSTR pszHostName, int nPort)
{
	//Initialise the winsock stack
	WSADATA wsaData;
	BYTE wsMajorVersion = 1;
	BYTE wsMinorVersion = 1;
	WORD wVersionRequested = MAKEWORD(wsMinorVersion, wsMajorVersion);   

	if (WSAStartup(wVersionRequested, &wsaData) != 0) 
	{
		//_tprintf(_T("Failed to load winsock stack\n"));
		return FALSE;
	}

	if (LOBYTE(wsaData.wVersion) != wsMajorVersion || HIBYTE(wsaData.wVersion) != wsMinorVersion)
	{
		//_tprintf(_T("Winsock stack does not support version which this program requires\n"));
		return FALSE;
	}

	//Do the actual NTP Query
	CSNTPClient sntp;
	NtpServerResponse response;

	if (sntp.GetServerTime(pszHostName, response, nPort))
	{
		//_tprintf(_T("Time was successfully retreived from NTP server\n"));

		//SYSTEMTIME st1 = response.m_OriginateTime;
		SYSTEMTIME st2 = response.m_ReceiveTime;
		//SYSTEMTIME st3 = response.m_TransmitTime;
		//SYSTEMTIME st4 = response.m_DestinationTime;

		//_tprintf(_T("                            DD/MM/YYYY  HH:MM:SS.MS\n"));
		//_tprintf(_T("Client Originate Date was   %02d/%02d/%04d, %02d:%02d:%02d.%03d\n"), st1.wDay, st1.wMonth, st1.wYear, st1.wHour, st1.wMinute, st1.wSecond, st1.wMilliseconds);
		//_tprintf(_T("Server Receive Date was     %02d/%02d/%04d, %02d:%02d:%02d.%03d\n"), st2.wDay, st2.wMonth, st2.wYear, st2.wHour, st2.wMinute, st2.wSecond, st2.wMilliseconds);
		m_strDate.Format( "%04d_%02d_%02d", st2.wYear, st2.wMonth, st2.wDay );
		m_strTime.Format( "%02d_%02d_%02d", st2.wHour, st2.wMinute, st2.wSecond );

		//_tprintf(_T("Server Transmit Date was    %02d/%02d/%04d, %02d:%02d:%02d.%03d\n"), st3.wDay, st3.wMonth, st3.wYear, st3.wHour, st3.wMinute, st3.wSecond, st3.wMilliseconds);
		//_tprintf(_T("Client Destination Date was %02d/%02d/%04d, %02d:%02d:%02d.%03d\n"), st4.wDay, st4.wMonth, st4.wYear, st4.wHour, st4.wMinute, st4.wSecond, st4.wMilliseconds);
  		//_tprintf(_T("Round trip delay was %f seconds\n"), response.m_RoundTripDelay);
  		//_tprintf(_T("Local clock offset was %f seconds\n"), response.m_LocalClockOffset);
	}
	else
	{
		WSACleanup();
		//_tprintf(_T("Failed to retreive time from NTP server, GetLastError returned %d\n"), GetLastError());
		return FALSE;
	}

	//Don't forget to release out use of the winsock stack
	WSACleanup();
	return TRUE;
}
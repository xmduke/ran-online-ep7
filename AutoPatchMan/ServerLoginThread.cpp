#include "StdAfx.h"
#include ".\serverloginthread.h"
#include "Resource.h"

#include "s_NetClient.h"
#include "LauncherText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CServerLoginThread, CLPatchThread)

CServerLoginThread::CServerLoginThread( LPVOID pParam, const CString & strLoginAddress, DWORD nThreadID ) :
	m_strLoginAddress( strLoginAddress ),
	m_nDlgThreadID(nThreadID)
{
	VERIFY( pParam != NULL);
	m_pNetClient = (CNetClient *)pParam;
}

void CServerLoginThread::ThreadMain()
{
	if( IsForceTerminate() ) return;

	int nRetCode = m_pNetClient->ConnectLoginServer( m_strLoginAddress.GetString() );
	if( nRetCode != NET_OK )
	{
		switch ( nRetCode )
		{
			case CNetClient::NET_ERR_CREATE_EVENT:	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 20 ), 0 );  break;
			case CNetClient::NET_ERR_EVENT_SELECT:	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 21 ), 0 );  break;
			case CNetClient::NET_ERR_CONNECT:		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 22 ), 0 );  break;
			case CNetClient::NET_ERR_EVENT_WAIT:	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 23 ), 0 );  break;
			case CNetClient::NET_ERR_CONTACT_EVENT: ::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 24 ), 0 );  break;
			case CNetClient::NET_ERR_CREATE_THREAD: ::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 25 ), 0 );  break;

			default: ::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 26 ), 0 ); 
		}

		SetFail();
		return;
	}

	if( IsForceTerminate() ) return;

	nRetCode = m_pNetClient->SndVersion();
	if( nRetCode <= NET_ERROR )
	{
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING,  (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 27 ), 0 );  // 버전 전송에 실패
		return;
	}
}

BEGIN_MESSAGE_MAP(CServerLoginThread, CLPatchThread)
END_MESSAGE_MAP()
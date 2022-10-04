#include "StdAfx.h"
#include ".\webbrowserthread.h"

#include "AutoPatchManDlg.h"

//#include "./webbrowser2.h"
#include "./explorer1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CWebBrowserThread, CLPatchThread)

CWebBrowserThread::CWebBrowserThread( LPVOID pParam, const CString & strURL ) :
	m_strURL( strURL )
{
	VERIFY( pParam != NULL);
	m_pBrowser = (CExplorer1 *)pParam;
}

void CWebBrowserThread::ThreadMain()
{
	if ( IsForceTerminate() ) return;

	try 
	{
		// http://msdn.microsoft.com/library/default.asp?url=/workshop/browser/webbrowser/reference/ifaces/iwebbrowser2/navigate.asp
		// S_OK The operation was successful.
		// E_INVALIDARG One or more parameters are invalid.
		// E_OUTOFMEMORY Out of memory.
		// S_OK 가 아닌경우를 체크해야 할 것 같다.
		m_pBrowser->Navigate( m_strURL.GetString(), NULL, NULL, NULL, NULL );	
	} 
	catch( COleDispatchException* pEx )
	{
		return;
	}
}

BEGIN_MESSAGE_MAP(CWebBrowserThread, CLPatchThread)
END_MESSAGE_MAP()
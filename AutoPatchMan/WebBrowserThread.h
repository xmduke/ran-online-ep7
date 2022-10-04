#pragma once

#include "LPatchThread.h"

class CExplorer1;

class CWebBrowserThread : public CLPatchThread
{
	DECLARE_DYNCREATE(CWebBrowserThread)

protected:
	CExplorer1 * m_pBrowser;
	CString m_strURL;

public:
	CWebBrowserThread( LPVOID pParam, const CString & strURL );

protected:
	virtual void ThreadMain();

public:
	CWebBrowserThread(){}
	virtual ~CWebBrowserThread(){}

protected:
	DECLARE_MESSAGE_MAP()
};

#include "stdafx.h"
#include "BrowserThread.h"
#include "webbrowser2.h"
#include "RANPARAM.h"
#include "ExceptionHandler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

unsigned __stdcall BrowserThread ( LPVOID pParam )
{
	__try
	{
		CWebBrowser2 * const pBrowser = (CWebBrowser2*)pParam;	
		pBrowser->Navigate(RANPARAM::strNewsURL.GetString(),NULL,NULL,NULL,NULL);
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), 
				_T("ExceptionAttacher.cpp - AfxWinMain")))
	{
		// Do nothing here - RecordExceptionInfo() has already done
		// everything that is needed. Actually this code won't even
		// get called unless you return EXCEPTION_EXECUTE_HANDLER from
		// the __except clause.
	}

	return 0;
}
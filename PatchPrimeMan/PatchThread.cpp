#include "stdafx.h"
#include "PatchThread.h"
#include "ExceptionHandler.h"

#include "PatchPrime.h"

#include <afxinet.h>
#include "s_NetGlobal.h"
#include "s_CPatch.h"
#include "CompactFdi.h"
#include "StringUtils.h"

#include "GlobalVariable.h"
#include "LogControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

unsigned __stdcall PatchThread ( LPVOID pData )
{
	__try
	{
		S_PATCH_THREAD_PARAM* pParam = (S_PATCH_THREAD_PARAM*)pData;
		const bool bUseHttp = pParam->bUseHttp;

		if ( bUseHttp )	PatchByHTTP ( pParam );
		else			PatchByFTP ( pParam );
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
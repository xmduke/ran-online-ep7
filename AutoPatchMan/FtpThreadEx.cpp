#include "stdafx.h"
#include <process.h>
#include "FtpThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace	NS_FTP_THREAD
{	
	BOOL	bFAIL = FALSE;
	HANDLE	hThread = NULL;
	HANDLE	hTerminateEvent = NULL;
	DWORD	dwThreadID = 0;

	BOOL	BEGIN ( S_FTP_THREAD_PARAM* pParam )
	{	
		bFAIL = FALSE;

		if ( hTerminateEvent || hThread )
		{
			return FALSE;
		}

        hTerminateEvent = CreateEvent  ( NULL, TRUE, FALSE, NULL );
		//hThread = CreateThread ( NULL, 0, ConnectFtpThread, pParam, 0, &dwThreadID );
		hThread = ::_beginthreadex(
						NULL,
						0,
						ConnectFtpThread,
						pParam,
						0,
						(unsigned int*) &dwThreadID );

		return TRUE;
	}	

	void	STOP ()
	{
		SetEvent ( hTerminateEvent );
	}

	void	END ()
	{			
		if ( hThread )
		{
			const	DWORD	miliSec = 1000L;
			if ( WaitForSingleObject ( hThread, 1 * miliSec ) == WAIT_TIMEOUT )
			{
				TerminateThread ( hThread, 1L );
			}

			CloseHandle ( hTerminateEvent );
			hTerminateEvent = NULL;

			CloseHandle ( hThread );
			hThread = NULL;
		}
	}


	BOOL	IsRunning ()
	{
		if ( hThread )
		{
			BOOL bRun = FALSE;
			bRun = WaitForSingleObject ( hThread, 0 ) != WAIT_OBJECT_0;
			return bRun;
		}
		return FALSE;
	}
	BOOL	IsForceTerminate ()
	{
		return WaitForSingleObject ( hTerminateEvent, 0 ) == WAIT_OBJECT_0;
	}

	void	SetFail ()
	{
		bFAIL = TRUE;
	}

	BOOL	IsFail ()
	{
		return bFAIL;
	}
};
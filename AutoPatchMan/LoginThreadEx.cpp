#include "stdafx.h"
#include "Resource.h"
#include "LoginThread.h"
#include "LogControl.h"

#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace	NS_LOGIN_THREAD
{	
	BOOL	bFAIL = FALSE;
	HANDLE	hThread = NULL;
	HANDLE	hTerminateEvent = NULL;
	//DWORD	dwThreadID = 0;
	unsigned threadID;

	BOOL	BEGIN ( CNetClient* pNetClient )
	{	
		if ( hTerminateEvent || hThread )
		{
			return FALSE;
		}

        hTerminateEvent = CreateEvent  ( NULL, TRUE, FALSE, NULL );
		if( hTerminateEvent == NULL ) return FALSE;

		hThread = (HANDLE)_beginthreadex( NULL, 0, ConnectLoginThread, pNetClient, 0, &threadID );
		if( hThread == NULL )
		{
			CloseHandle( hTerminateEvent );
			return FALSE;
		}

		bFAIL = FALSE;

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
				//NS_LOG_CONTROL::Write ( IDS_MESSAGE_028 ); // 강제 종료
				TerminateThread ( hThread, 1L ); // MEMO : 다른 방법은 없을까...?
			}

			CloseHandle ( hThread );
			hThread = NULL;
		}

		if( hTerminateEvent )
		{
			CloseHandle ( hTerminateEvent );
			hTerminateEvent = NULL;
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
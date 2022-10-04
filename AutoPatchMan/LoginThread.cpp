#include "stdafx.h"
#include "Resource.h"
#include "LoginThread.h"
#include "s_NetClient.h"
#include "RANPARAM.h"
#include "LogControl.h"
#include "ExceptionHandler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

unsigned __stdcall ConnectLoginThread ( LPVOID pData )
{
	__try
	{
		CNetClient* pNetClient = (CNetClient*)pData;

		if ( NS_LOGIN_THREAD::IsForceTerminate () )
		{
			return 0;
		}

		int nRetCode = pNetClient->ConnectLoginServer(RANPARAM::LoginAddress);
		if ( nRetCode <= NET_ERROR )
		{
			NS_LOGIN_THREAD::SetFail ();
			switch ( nRetCode )
			{
				case CNetClient::NET_ERR_CREATE_EVENT: NS_LOG_CONTROL::Write ( IDS_MESSAGE_020 ); break;
				case CNetClient::NET_ERR_EVENT_SELECT: NS_LOG_CONTROL::Write ( IDS_MESSAGE_021 ); break;
				case CNetClient::NET_ERR_CONNECT: NS_LOG_CONTROL::Write ( IDS_MESSAGE_022 ); break;
				case CNetClient::NET_ERR_EVENT_WAIT: NS_LOG_CONTROL::Write ( IDS_MESSAGE_023 ); break;
				case CNetClient::NET_ERR_CONTACT_EVENT: NS_LOG_CONTROL::Write ( IDS_MESSAGE_024 ); break;
				case CNetClient::NET_ERR_CREATE_THREAD: NS_LOG_CONTROL::Write ( IDS_MESSAGE_025 ); break;
				default: NS_LOG_CONTROL::Write ( IDS_MESSAGE_026 );
			}
			
			return -1;
		}

		if ( NS_LOGIN_THREAD::IsForceTerminate () )
		{
			return 0;
		}

		nRetCode = pNetClient->SndVersion();
		if ( nRetCode <= NET_ERROR )
		{
			NS_LOG_CONTROL::Write ( IDS_MESSAGE_027 ); // 버전 전송에 실패
			return -1;
		}
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
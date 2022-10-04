#include "stdafx.h"
#include "Resource.h"
#include "FtpThread.h"
#include "RANPARAM.h"
#include "LogControl.h"
#include "GlobalVariable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

unsigned int WINAPI	ConnectFtpThread( LPVOID pData )
{
	S_FTP_THREAD_PARAM* pParam = (S_FTP_THREAD_PARAM*)pData;
	CPatch* pFtpPatch = pParam->pFtpPatch;
	std::string strFtpAddress = pParam->strFtpAddress;

	int nRetCode = NET_OK;

#ifdef	__ALPHA__
	nRetCode = pFtpPatch->Connect( strFtpAddress.c_str (),
						21,
						g_szAlphaFtpID,
						g_szAlphaFtpPW,
						RANPARAM::bUsePassiveDN );
#else
	nRetCode = pFtpPatch->Connect( strFtpAddress.c_str (),
						21,
						NS_GLOBAL_VAR::g_szBetaFtpID,
						NS_GLOBAL_VAR::g_szBetaFtpPW,
						RANPARAM::bUsePassiveDN );
#endif
    	
	if (nRetCode == NET_ERROR)
	{
		NS_LOG_CONTROL::Write ( IDS_MESSAGE_045 );
		NS_FTP_THREAD::SetFail ();
		return -1;
	}

	return 0;
}
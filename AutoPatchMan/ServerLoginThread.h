#pragma once

#include "LPatchThread.h"

class	CNetClient;

class CServerLoginThread : public CLPatchThread
{
	DECLARE_DYNCREATE(CServerLoginThread)

protected:
	CNetClient * m_pNetClient;
	CString m_strLoginAddress;

	DWORD m_nDlgThreadID;

public:
	CServerLoginThread( LPVOID pParam, const CString & strLoginAddress, DWORD nThreadID );

protected:
	virtual void ThreadMain();

public:
	CServerLoginThread(){}
	virtual ~CServerLoginThread(){}

protected:
	DECLARE_MESSAGE_MAP()
};

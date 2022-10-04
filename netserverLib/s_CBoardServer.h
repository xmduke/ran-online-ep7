///////////////////////////////////////////////////////////////////////////////
// s_CBoardServer.h
//
// class CBoardServer
//
// * History
// 2003.08.19 jgkim Create
//
// Copyright 2002-2004 (c) Mincoms. All rights reserved.                 
// 
// * Note
//
///////////////////////////////////////////////////////////////////////////////
#ifndef S_CBoardServer_H_
#define S_CBoardServer_H_

#include "s_CServer.h"
#include "s_CClientBoard.h"
#include "s_COverlapped.h"
#include "s_CConsoleMessage.h"
#include "s_CCfg.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBoardServer : public CServer
{
public :
	CBoardServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo);
	~CBoardServer();

public :
	// Data member for login server
	SOCKET				m_sSession;	// Socket for session server
	CClientBoard*		m_pClientManager;

public :
	virtual int	Start();
	virtual int	Stop();	
	virtual	int	Pause();
	virtual	int Resume();
	virtual	int	ReStart();
		
			int	StartClientManager();
	
	virtual int	WorkProc();
	virtual int	UpdateProc();	
	virtual int ListenProc();

	virtual	int DatabaseProc()     { return 0; }; // Database thread function	
	virtual int LogDatabaseProc()  { return 0; }; // Log database thread function
	virtual int UserDatabaseProc() { return 0; }; // User database thread function

	void	Send			(DWORD nClient, LPPER_IO_OPERATION_DATA PerIoData, DWORD dwSize);
	int		SendClient		(DWORD dwClient, LPVOID pBuffer);
	void	SendAllClient	(LPVOID pBuffer);
	void	CloseClient		(DWORD dwClient);
	void	CloseAllClient	();

	///////////////////////////////////////////////////////////////////////////////
	// Message process function
	int		InsertMsg (int nClient, void* pMsg) { return 0; };	
	int		MsgProcess			(MSG_LIST* pMsg);
	void	MsgSndPingAnswer	(MSG_LIST* pMsg);

	void	ServerMsgProcess	(MSG_LIST* pMsg);

    ///////////////////////////////////////////////////////////////////////////////
	// Session Server Communication Member Function
	// If not use Session Server, this functions are unnecessary
	int		SessionConnect		(unsigned long cServerIP, int nPort);
	int		SessionConnect		(const char* cServerIP, int nPort);
	int		SessionCloseConnect	(void);	
	
	void	SendSession			(LPVOID pBuffer);	
	void	SessionMsgProcess	(MSG_LIST* pMsg);
	
	void	SessionMsgPingAnswer(NET_MSG_GENERIC* nmg); // Ping Answer
	void	SessionMsgSvrCmd	(NET_MSG_GENERIC* nmg);
	
	void	SessionSndPing		(void); // Send ping
	void	SessionSndSvrInfo	(void);
	void	SessionSndSvrCurState(void);
	
	// Execute Command Text
	int		ExecuteCommand(char* strCmd);
};

static DWORD WINAPI CServerListenProc  (CBoardServer *pThis) { return pThis->ListenProc();   }
static DWORD WINAPI CServerWorkProc    (CBoardServer *pThis) { return pThis->WorkProc();     }
static DWORD WINAPI CServerUpdateProc  (CBoardServer *pThis) { return pThis->UpdateProc();   }

static DWORD WINAPI CServerDatabaseProc    (CBoardServer *pThis) { return pThis->DatabaseProc(); }
static DWORD WINAPI CServerLogDatabaseProc (CBoardServer *pThis) { return pThis->LogDatabaseProc(); }
static DWORD WINAPI CServerUserDatabaseProc(CBoardServer *pThis) { return pThis->UserDatabaseProc(); }

#endif // S_CBoardServer_H_
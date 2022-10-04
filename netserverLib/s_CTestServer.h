///////////////////////////////////////////////////////////////////////////////
// s_CTestServer.h
// class CTestServer
//
// * History
// 2005.02.16 jgkim Create
//
// Copyright 2002-2005 (c) Mincoms. All rights reserved.                 
// 
// * Note 
// 
///////////////////////////////////////////////////////////////////////////////

#ifndef S_TESTSERVER_H_
#define S_TESTSERVER_H_

#include "s_CServer.h"
#include "s_CClientSession.h"
#include "s_CDbAction.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTestServer : public CServer
{
public :
	CTestServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo);
	~CTestServer();

protected :
	CClientSession*		m_pClientManager;
	G_SERVER_CUR_INFO_LOGIN m_sServerChannel[MAX_SERVER_GROUP][MAX_CHANNEL_NUMBER];
	
public :
	virtual int	 Start();
	virtual int	 Stop();	
	virtual	int	 Pause();
	virtual	int  Resume();
	virtual	int	 ReStart();

	        int	 StartClientManager();
	        int	 StartDbManager();
	
	virtual int	 WorkProc();
	virtual int	 UpdateProc();	
	virtual int  ListenProc();

	virtual	int  DatabaseProc();
	virtual	int  LogDatabaseProc()  { return 0; }; // Log Database thread function
	virtual	int  UserDatabaseProc() { return 0; }; // User Database thread function
    
	virtual int  InsertMsg(int nClient, void* pMsg) { return 0; };	

	        void Send	  (DWORD dwClient, LPPER_IO_OPERATION_DATA PerIoData, DWORD dwSize);
		    int	 SendClient(DWORD dwClient, LPVOID pBuffer);
	        void CloseClient(DWORD dwClient);
	        void CloseAllClient(); 	// close all client connections
	        int  FindServer(CString strIP, int nPort);
	        int  FindServer(int nSvrType, int nSvrGrp, int nSvrNum, int nFieldNum);

	        void UserInsert(CString strUsrID, LPCNT_USR_INFO pData);
			void SetSvrInfo(DWORD dwClient, G_SERVER_INFO *gsi);
			void SetChannelInfo(NET_SERVER_CHANNEL_INFO* pMsg);
		
	        ///////////////////////////////////////////////////////////////////////////////
	        // Message process function
	        int  MsgProcess				(MSG_LIST* pMsg);

			int	 ExecuteCommand			(char* strCmd);
};

static DWORD WINAPI CServerListenProc  (CTestServer *pThis)     { return pThis->ListenProc(); }
static DWORD WINAPI CServerWorkProc    (CTestServer *pThis)     { return pThis->WorkProc();   }
static DWORD WINAPI CServerUpdateProc  (CTestServer *pThis)     { return pThis->UpdateProc(); }
static DWORD WINAPI CServerDatabaseProc(CTestServer *pThis)     { return pThis->DatabaseProc(); }
static DWORD WINAPI CServerUserDatabaseProc(CTestServer *pThis) { return pThis->UserDatabaseProc(); }

#endif // S_SESSIONSERVER_H_

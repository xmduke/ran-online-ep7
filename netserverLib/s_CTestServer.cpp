///////////////////////////////////////////////////////////////////////////////
// s_CTestServer.cpp
//
// class CTestServer
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note
// 
///////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "s_CTestServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTestServer::CTestServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo) 
	: CServer(hWnd, hEditBox, hEditBoxInfo),
	m_pClientManager (NULL)
{
}

CTestServer::~CTestServer()
{
	SAFE_DELETE(m_pClientManager);
}

// 클라이언트 정보저장 클래스 생성
int	CTestServer::StartClientManager()
{	
	SAFE_DELETE(m_pClientManager);
	m_pClientManager = new CClientSession(m_nMaxClient, m_pSendIOCP, m_pRecvIOCP, m_hIOServer);
	if (!m_pClientManager) 
	{
		m_bIsRunning = false;
		return NET_ERROR;
	}
	else
	{
		return NET_OK;
	}
}

// Create DB manager and Open DB
int	CTestServer::StartDbManager()
{
	return NET_OK;
}

int CTestServer::Start()
{
	m_bIsRunning = true;

	if (StartCfg()			== NET_ERROR) return NET_ERROR;	
	if (StartIOCPList()		== NET_ERROR) return NET_ERROR;    	
	if (StartMsgManager()	== NET_ERROR) return NET_ERROR;
	if (StartIOCP()			== NET_ERROR) return NET_ERROR;
	if (StartClientManager()== NET_ERROR) return NET_ERROR;		
	if (StartWorkThread()	== NET_ERROR) return NET_ERROR;
	if (StartUpdateThread() == NET_ERROR) return NET_ERROR;	
	if (StartListenThread() == NET_ERROR) return NET_ERROR;	
	
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "=======================================================");	
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "Server Start OK");	
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "=======================================================");

	m_nStatus = S_SERVER_RUNING;

	return NET_OK;
}

int CTestServer::Stop()
{
	// Stop All Thread and exit
	DWORD dwExitCode = 0;	
	
	EnterCriticalSection(&m_CriticalSection);
	m_bIsRunning = false;
	LeaveCriticalSection(&m_CriticalSection);	
	::Sleep(1000);

	StopListenThread();
	StopIOCPList();
	StopWorkThread();
	StopIOCP();

	CloseAllClient(); 	// close all client connections

	// 클라이언트 저장 클래스 종료
	SAFE_DELETE(m_pClientManager);
	// IOCP 종료
	SAFE_DELETE(m_pRecvIOCP);
	SAFE_DELETE(m_pSendIOCP);
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "Stop IOCP");
	// Message Queue 종료	
	SAFE_DELETE(m_pRecvMsgManager);
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "Stop Message Queue");
	// CFG class 종료
	CCfg::GetInstance()->ReleaseInstance();
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "Stop CFG");	
	// Put message to console
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "=======================================================");	
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "Server Stop OK");	
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "=======================================================");
	
	m_nStatus = S_SERVER_STOP;

	return NET_OK;
}

int	CTestServer::Pause()
{
	::closesocket(m_sServer);	
	m_sServer = INVALID_SOCKET;
	CloseAllClient();
	m_nStatus = S_SERVER_PAUSE;
	// Put message to console
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "=======================================================");	
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "Server Pause OK");	
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "=======================================================");

	return NET_OK;	
}

int CTestServer::Resume()
{	
	if (StartListenThread() == NET_OK)
	{
		m_nStatus = S_SERVER_RUNING;
		// Put message to console
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "=======================================================");	
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "Server Resume OK");	
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "=======================================================");

		return NET_OK;
	}
	else
	{
		// Put message to console
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "=======================================================");	
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "Server Resume Failed");	
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "=======================================================");

		return NET_ERROR;
	}
}

int	CTestServer::ReStart()
{
	Stop();
	Start();
	return NET_OK;
}

void CTestServer::CloseAllClient()
{
	for (DWORD dwNum=0; dwNum<(DWORD) m_nMaxClient; dwNum++)
		CloseClient(dwNum);
}

// 클라이언트의 연결을 해제한다.
void CTestServer::CloseClient(DWORD dwClient)
{	
	CConsoleMessage::GetInstance()->Write("CloseClient");
	// 등록된 IP 리스트를 제거한다.
	// SERVER_UTIL::CAddressChecker::GetInstance()->Delete(m_pClientManager->GetClientIP(dwClient));
	// 연결종료
	m_pClientManager->CloseClient(dwClient);
}

int CTestServer::DatabaseProc()
{
	return NET_OK;
}

int CTestServer::ListenProc()
{
	LPPER_IO_OPERATION_DATA IOData;
	SOCKET	Accept;
	DWORD	dwRecvNumBytes = 0;
	DWORD	Flags = 0;
	HANDLE	hRetCode = NULL;
	INT		nClientNumber = 0;
	int		nRetCode;

	while (m_bIsRunning)
	{
		Accept = ::WSAAccept(m_sServer, NULL, NULL, NULL, 0);
		if (Accept == INVALID_SOCKET) 
		{
			nRetCode = ::WSAGetLastError();
			CConsoleMessage::GetInstance()->Write("ERROR:WSAAccept %d", nRetCode);
			if (nRetCode == WSAENOTSOCK || nRetCode == WSAEINTR)
			{	
				// CConsoleMessage::GetInstance()->Write("Listen Thread Stopped");
				break;
			}
			else
			{
				continue;
			}
		}

		///////////////////////////////////////////////////////////////////////		
		// Create per-handle data information structure to associate with the socket
		nClientNumber = m_pClientManager->GetFreeClientID(); // Get free client slot number
		if (nClientNumber == NET_ERROR) 
		{
			closesocket(Accept);
			CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, "CTestServer::ListenProc Not available client slot");
			continue;
		} 
		else 
		{
			// Set client information
			m_pClientManager->SetAcceptedClient(nClientNumber, Accept);
		}

		// IP 를 리스트에 등록한다.
		// SERVER_UTIL::CAddressChecker::GetInstance()->IsBlock(m_pClientManager->GetClientIP((DWORD) nClientNumber));

		// Associate the accepted socket with the completion port
		hRetCode = ::CreateIoCompletionPort((HANDLE)Accept, 
										m_hIOServer, 
										(DWORD)nClientNumber, 
										0);
		if (hRetCode == NULL) 
		{		
			CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "CTestServer::ListenProc CreateIoCompletionPort Error");
			::closesocket(Accept);
			continue;
		} 

		// Start processing I/O on ther accepted socket
		// First WSARecv, TCP/IP Send 8 bytes (ignored byte)
		// 서버 정보를 받는다.
		dwRecvNumBytes = sizeof(NET_MSG_GENERIC);
		IOData = (LPPER_IO_OPERATION_DATA) GetFreeOverIO(NET_RECV_POSTED);

		if (IOData == NULL) continue;

		IOData->dwRcvBytes = 0;
		IOData->dwTotalBytes = dwRecvNumBytes;
		// Head receive mode
		m_pClientManager->SetNetMode(nClientNumber, NET_PACKET_HEAD);
		
		::WSARecv(Accept,
			&(IOData->DataBuf), 
			1,
			&dwRecvNumBytes,
			&Flags,
			&(IOData->Overlapped),
			NULL);
		::Sleep(0);
	}
	CConsoleMessage::GetInstance()->Write("Listen Thread Stopped");
	return 0;	
}

int CTestServer::UpdateProc()
{
	DWORD nTimeS            = timeGetTime();
	float fFrame			= 0;
	float fCount			= 0;	
	float fUpdateSec		= 60000; // FPS 표시 갱신시간 1 분
	DWORD dwUsrChkTime		= 1800000; // 30 분마다 한번씩 사용자 접속끊기 체크	
	DWORD dwUsrSaveTime		= 60000 * 30; // 사용자 저장시간 30분
	DWORD dwUsrSaveStart	= nTimeS; // 사용자 저장시간 체크를 위한 시간변수
	DWORD dwHeartBeatStart  = nTimeS; // 서버가 정상가동중인지 체크한다.
	DWORD dwUsrChkStart		= nTimeS; // 아무것도 하지 않는 사용자 접속끊기를 위한 시간
	
	m_bUpdateEnd			= false;    
    bool  bHeartBeat		= false; // 정상가동 체크 플래그
    DWORD dwN               = 0;
    DWORD dwMaxClient       = (DWORD) m_nMaxClient;

	MSG_LIST* pMsg = NULL;
	while (m_bIsRunning)
	{
		m_pClientManager->ResetPreSleepCID ();
		// Flip Messge Queue
		m_pRecvMsgManager->MsgQueueFlip();	
		// Get Message
		pMsg = m_pRecvMsgManager->GetMsg();
		
		while (pMsg != NULL)
		{
			MsgProcess(pMsg);
			pMsg = m_pRecvMsgManager->GetMsg();
		}

		// HeartBeat Check				
		if ((timeGetTime() - dwHeartBeatStart) > HEARTBEAT_TIME)
		{
			if (bHeartBeat) // Send Heartbeat
			{	
				CConsoleMessage::GetInstance()->WriteInfo(_T("Send Heartbeat Request"));
				for (dwN = NET_RESERVED_SLOT; dwN < dwMaxClient; dwN++)
				{	
					if (m_pClientManager->IsOnline(dwN))
					{
						m_pClientManager->ResetHeartBeat(dwN);
						NET_MSG_GENERIC nmg;
						nmg.dwSize = sizeof(NET_MSG_GENERIC);
						nmg.nType = NET_MSG_HEARTBEAT;
						SendClient(dwN, &nmg);
					}
				}
			}
			else // Check Heartbeat
			{
				CConsoleMessage::GetInstance()->WriteInfo(_T("Check Heartbeat"));
				for (dwN = NET_RESERVED_SLOT; dwN < dwMaxClient; dwN++)
				{	
					if (m_pClientManager->IsOnline(dwN))
					{
						if (!m_pClientManager->CheckHeartBeat(dwN))
						{
							CloseClient(dwN);
						}
					}
				}
			}
			bHeartBeat = !bHeartBeat;
			dwHeartBeatStart = timeGetTime();
		}

		///////////////////////////////////////////////////////////////////////
		// 프레임 측정
		if ((timeGetTime() - nTimeS) >= fUpdateSec )
		{			
			// DB 연결유지
			// COdbcManager::GetInstance()->CheckConnection();

			fFrame = ( (float)((fCount * fUpdateSec) /(float)(timeGetTime() - nTimeS)));
			//nFrame = Count; 
			nTimeS = timeGetTime();
			fCount = 0;
			
			m_pClientManager->resetPacketCount();
		}
		else
		{
			fCount++;
		}
		::Sleep(0);
	}
	TRACE("CTestServer::UpdateProc() End \n");
	m_bUpdateEnd = true;
	return 0;
}

int CTestServer::WorkProc()
{
	LPPER_IO_OPERATION_DATA PerIoData = NULL;
	DWORD dwSndBytes  = 0;
	DWORD dwRcvBytes  = 0;
	DWORD dwByteTrans = 0;
	DWORD Flags       = 0;
	DWORD dwClient    = -1;
	int   nErrCode    = 0;
	int	  nRetCode    = 0;	
	DWORD dwMaxClient = m_pClientManager->GetMaxClient();
	NET_MSG_GENERIC* pNmg = NULL;

	while (m_bIsRunning)
	{
		nRetCode = GetQueuedCompletionStatus(m_hIOServer,
			                                 &dwByteTrans, 
			                                 (LPDWORD)&dwClient,
			                                 (LPOVERLAPPED *)&PerIoData,			
			                                 INFINITE);
		
		if (dwClient == m_dwCompKey && PerIoData == NULL && dwByteTrans == 0)
		{
			CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE,
				                                  "dwClient == m_dwCompKey && PerIoData == NULL && dwByteTrans == 0", 
				                                  dwClient);
			break;
		}
		
		// 잘못된 클라이언트 ID
		if ((dwClient < 0) || (dwClient >= dwMaxClient))
		{
			CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE,
				                                  "(dwClient < 0) || (dwClient >= dwMaxClient)",
				                                  dwClient);
			continue;
		}
		
		if (nRetCode == 0) // 강제종료
		{
			// 64 번 에러이면 무조건 종료
			nErrCode = GetLastError();			
			CConsoleMessage::GetInstance()->Write("nRetCode=0 && nErrCode=64");
            CloseClient(dwClient);
			ReleaseOperationData(PerIoData);
			continue;
		}

		if (dwByteTrans == 0) // 정상종료
		{
			CConsoleMessage::GetInstance()->Write("dwByteTrans=0");
			CloseClient(dwClient);
			ReleaseOperationData(PerIoData);
			continue;
		}

		switch (PerIoData->OperationType)
		{
		// 읽기 완료 통보일때...
		case NET_RECV_POSTED :
			{			
                m_pClientManager->addRcvMsg(dwClient, PerIoData->Buffer, dwByteTrans);
				
				// Get Message
				while (true) {
					pNmg = (NET_MSG_GENERIC*) m_pClientManager->getRcvMsg(dwClient);
					if (pNmg == NULL) {
						break;
					}
					else {
						m_pRecvMsgManager->MsgQueueInsert(dwClient, pNmg, (int) pNmg->dwSize);
					}
				}

				// OverlappedIO 메모리 반환
				ReleaseOperationData(PerIoData);
				// WSARecv 요청
				Flags = 0;
				dwRcvBytes = sizeof(NET_MSG_GENERIC);
				PerIoData = (LPPER_IO_OPERATION_DATA) GetFreeOverIO(NET_RECV_POSTED);

				if (PerIoData == NULL) continue;
				
				nRetCode = WSARecv(m_pClientManager->GetSocket(dwClient),
					               &(PerIoData->DataBuf), 
					               1, 
					               &dwRcvBytes,
					               &Flags,
					               &(PerIoData->Overlapped),
					               NULL);

				if ((nRetCode == SOCKET_ERROR) && 
					((nErrCode = WSAGetLastError()) != WSA_IO_PENDING))
				{					
					ReleaseOperationData(PerIoData);
					CloseClient(dwClient);
					CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, 
						                                  _T("ERROR:case 8 WorkProc WSARecv Client(%d) ERR(%d)"), 
														  dwClient, 
														  nErrCode);
				}
			}
			break;
		// 쓰기 완료 통보 일때...
		case NET_SEND_POSTED :
			// 총 보낸 바이트수
			dwSndBytes = dwByteTrans + PerIoData->dwSndBytes;
			// 쓰기 완료됨
			if (dwSndBytes >= PerIoData->dwTotalBytes)
			{				
				ReleaseOperationData(PerIoData);
			}
			else // 쓰기 완료되지 않음 남은 바이트를 전송
			{				
				PerIoData->dwSndBytes  = dwSndBytes; // 보낸 바이트수 업데이트				
				PerIoData->DataBuf.buf = PerIoData->Buffer + dwSndBytes; // 보내야할 데이타 포인터 업데이트				
				dwSndBytes             = PerIoData->dwTotalBytes - dwSndBytes; // 보내야할 바이트수 업데이트				
				PerIoData->DataBuf.len = (u_long) dwSndBytes; // 버퍼길이 업데이트				
				Send(dwClient, PerIoData, dwSndBytes); // 전송요청
			}
			break;
		default :			
			CloseClient(dwClient);
			break;
		}
		Sleep(0);
	}
	// ServerWorkerThread Running End
	TRACE("CTestServer::WorkProc() End \n");
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE,
		                                  "WorkProc End");
	return 0;
}

void CTestServer::Send(DWORD dwClient, LPPER_IO_OPERATION_DATA PerIoData, DWORD dwSize)
{	
}

void CTestServer::UserInsert(CString strUsrID, LPCNT_USR_INFO pData)
{
    if (pData == NULL) return;

	m_pClientManager->UserInsert(strUsrID, pData);
}

int CTestServer::SendClient(DWORD dwClient, LPVOID pBuffer)
{
    if (pBuffer == NULL) return NET_ERROR;

	return m_pClientManager->SendClient2(dwClient, pBuffer);
}

int CTestServer::ExecuteCommand(char* strCmd)
{
    if (strCmd == NULL) return NET_ERROR;

	// 토큰으로 분리할 분리자.
	// space
	char seps[]   = " ";
	char *token = NULL;

	// Establish string and get the first token
	token = strtok(strCmd, seps);
	while (token)
	{
		// Request Server Info
		// command : req svrfullinfo
		if (strcmp(token, "print_ip") == 0)
		{
			// SERVER_UTIL::CAddressChecker::GetInstance()->Save();
			return 0;
		}
		if (strcmp(token, "print_user") == 0)
		{
			// 사용자 리스트 출력
			token = strtok(NULL, seps);
			if (token)
			{
				if (strcmp(token, "user") == 0)
					m_pClientManager->PrintUsrList();
				else
					return 0;
			}
			else
			{
				return 0;
			}
		}
		// Get next token
		token = strtok(NULL, seps);
	}	
	return 0;
}

int CTestServer::MsgProcess(MSG_LIST* pMsg)
{	
    if (pMsg == NULL) return NET_ERROR;

	NET_MSG_GENERIC* nmg;
	
	nmg = reinterpret_cast<NET_MSG_GENERIC*> (pMsg->Buffer);
	
	// HeartBeat check
	if (nmg->nType == NET_MSG_HEARTBEAT)	
	{
		m_pClientManager->SetHeartBeat(pMsg->dwClient);
		return 0;
	}

	switch (nmg->nType)
	{	
	case NET_MSG_PING_REQUEST : // 핑요청
		break;
	default:
		break;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// s_CGameServer.h
//
// class CGameServer
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note
//
///////////////////////////////////////////////////////////////////////////////
#ifndef S_CGAMESERVER_H_
#define S_CGAMESERVER_H_

#include "s_CServer.h"
#include "s_CClientGame.h"
#include "DxMsgServer.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGameServer : public CServer, public DxMsgServer
{
public :
	CGameServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo);
	~CGameServer();

protected :	
	// Data member for session server
	SOCKET				m_sSession;	// Socket for session server
	CClientGame*		m_pClientManager;
	
public :
	virtual int	Start();
	virtual int	Stop();	
	virtual	int	Pause();
	virtual	int Resume();
	virtual	int	ReStart();

	int			StartClientManager();
	int			StartDbManager();

	virtual int	WorkProc();
	virtual int	UpdateProc();	
	virtual int ListenProc();
	virtual	int DatabaseProc()     {return 0;};
	virtual int LogDatabaseProc()  {return 0;};
	virtual int UserDatabaseProc() {return 0;};

	void		Send			( DWORD nClient, LPPER_IO_OPERATION_DATA PerIoData, DWORD dwSize );
	virtual int SendClient		( DWORD dwClient, LPVOID pBuffer );
	void		SendAllClient	( LPVOID pBuffer );	
	void		CloseClient(DWORD dwClient);  // Close Client Connection
	void		CloseAllClient();

    virtual int SendField ( DWORD dwClient, LPVOID pBuffer )    { GASSERT(0&&""); return 0; };
	virtual int SendAgent ( DWORD dwClient, LPVOID pBuffer )    { GASSERT(0&&""); return 0; };
	virtual void SendAllField ( LPVOID pBuffer )                { GASSERT(0&&""); };
	
	///////////////////////////////////////////////////////////////////////////////
	// Message process function
	int		InsertMsg           (int nClient, void* pMsg) { return 0; };
	int		MsgProcess			(MSG_LIST* pMsg);
	
	void	MsgLogIn			(MSG_LIST* pMsg);
	void	MsgVersion			(MSG_LIST* pMsg);	
	void	MsgCreateCharacter	(MSG_LIST* pMsg);

	void	MsgSndUserInfo		(MSG_LIST* pMsg);
	void	MsgSndPingAnswer	(MSG_LIST* pMsg);	
	void	MsgSndChatGlobal	(char* szChatMsg);
	void	MsgSndChatNormal	(DWORD dwClient, const char* szName, const char* szMsg);
	void	MsgSndCryptKey		(DWORD dwClient);

	void	MsgSndChaBasicBAInfo(MSG_LIST* pMsg);
	void	MsgSndChaBasicInfo  (MSG_LIST* pMsg);
	void	MsgSndChaBasicInfo  (DWORD dwClient, int nChaNum);
	void	MsgSndChaDelInfo	(MSG_LIST* pMsg); // 게임서버->클라이언트 : 케릭터 삭제 결과 전송

	void	MsgGameJoin			(MSG_LIST* pMsg); // 클라이언트->게임서버 : 캐릭터 선택후 조인

	void	SetUserInfo(DWORD nClient, const char* strUserId, const char* strPassword);
	
	void	ServerMsgProcess	(MSG_LIST* pMsg);

    ///////////////////////////////////////////////////////////////////////////////
	// Session Server Communication Member Function
	// If not use Session Server, this functions are unnecessary
	int		SessionConnect(unsigned long cServerIP, int nPort);
	int		SessionConnect(const char* cServerIP, int nPort);
	int		SessionCloseConnect(void);	
	// CHAR*	SessionRecv(void);
	void	SendSession(LPVOID pBuffer);
	void	SessionProcess(LPARAM lParam);
	void	SessionMsgProcess(MSG_LIST* pMsg);
	
	void	SessionMsgPingAnswer(NET_MSG_GENERIC* nmg); // Ping Answer
	void	SessionMsgLoginFeedBack(NET_MSG_GENERIC* nmg); // 세션서버 -> 게임서버 : 로그인결과 데이타
	void	SessionMsgChatProcess(NET_MSG_GENERIC* nmg); // 세션서버->게임서버 : 채팅메시지

	void	SessionSndPing(void); // Send ping
	void	SessionSndSvrInfo(void);
	void	SessionSndSvrCurState(void);
	int		SessionSndLoginInfo(const char* szUsrID, const char* szUsrPass, int nClient);
	void	SessionSndLogOutInfo(const char* szUsrID, int nUserNum=0, int nGameTime=0);
	void	SessionSndLogOutInfoSimple(const char* szUsrID);

	// Execute Command Text
	int		ExecuteCommand(char* strCmd);
};

#endif // S_CGAMESERVER_H_
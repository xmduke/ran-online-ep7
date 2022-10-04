///////////////////////////////////////////////////////////////////////////////
// s_CClientGame.h
// class CClientGame
//
///////////////////////////////////////////////////////////////////////////////

#ifndef S_CCLIENTGAME_H_
#define S_CCLIENTGAME_H_

#include "s_NetGlobal.h"
#include "s_CClientManager.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CClientGame : public CClientManager
{
public :
	CClientGame(int nMaxClient,
				COverlapped* pSendIOCP,
				COverlapped* pRecvIOCP,
				HANDLE	hIOServer);
	virtual ~CClientGame();

private :
	
public :
	void	SetUserID(DWORD dwClient, const char* szUserID);
	void	SetUserNum(DWORD dwClient, INT nUserNum);
	void	SetLoginTime(DWORD dwClient);
	void	SetGaeaID(DWORD dwClient, DWORD dwGaeaID);
	void	SetLastMsgTime(DWORD dwClient, DWORD dwTime);
	void	SetLastSaveTime(DWORD dwClient, DWORD dwTime);

public :
	CHAR*	GetUserID		(DWORD dwClient);
	INT		GetUserNum		(DWORD dwClient);
	DWORD	GetUserCharObjID(DWORD dwClient);
	INT		GetLoginTime	(DWORD dwClient);
	DWORD	GetGaeaID		(DWORD dwClient);
	DWORD	GetLastMsgTime	(DWORD dwClient);
	DWORD	GetLastSaveTime	(DWORD dwClient);

	bool	IsGaming(DWORD dwClient);
};

#endif // S_CCLIENTGAME_H_
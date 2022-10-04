///////////////////////////////////////////////////////////////////////////////
// s_CClientBoard.h
//
// class CClientBoard
//
// * History
// 2003.08.22 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note
// 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef S_CCLIENTBOARD_H_
#define S_CCLIENTBOARD_H_

#include "s_NetGlobal.h"
#include "s_CClientManager.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CClientBoard : public CClientManager
{
public :
	CClientBoard(int nMaxClient,
				COverlapped* pSendIOCP,
				COverlapped* pRecvIOCP,
				HANDLE	hIOServer);
	virtual ~CClientBoard();

public :
	void	SetLastMsgTime		(DWORD dwClient, DWORD dwTime);
	DWORD	GetLastMsgTime		(DWORD dwClient);
};

#endif // S_CCLIENTBOARD_H_
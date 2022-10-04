///////////////////////////////////////////////////////////////////////////////
// s_CClientBoard.cpp
//
// class CClientBoard
//
// * History
// 2003.08.22 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note :
// 
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "s_CClientBoard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CClientBoard::CClientBoard(
	int nMaxClient,
	COverlapped* pSendIOCP,
	COverlapped* pRecvIOCP,
	HANDLE	hIOServer)
: CClientManager(
	nMaxClient,
	pSendIOCP,
	pRecvIOCP,
	hIOServer)
{
}

CClientBoard::~CClientBoard()
{
}

void CClientBoard::SetLastMsgTime(DWORD dwClient, DWORD dwTime)
{
	m_pClient[dwClient].SetLastMsgTime(dwTime);
}

DWORD CClientBoard::GetLastMsgTime(DWORD dwClient)
{
	return m_pClient[dwClient].GetLastMsgTime();
}
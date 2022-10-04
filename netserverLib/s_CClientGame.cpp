///////////////////////////////////////////////////////////////////////////////
// s_CClientGame.cpp
//
// class CClientGame
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note 
// 
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "s_CClientGame.h"
#include "GLGaeaServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CClientGame::CClientGame(
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

CClientGame::~CClientGame()
{
}

void CClientGame::SetUserID(DWORD dwClient, const char* szUserID)
{	
	m_pClient[dwClient].SetUserID(szUserID);
}

void CClientGame::SetUserNum(DWORD dwClient, INT nUserNum)
{
	m_pClient[dwClient].SetUserNum(nUserNum);
}

void CClientGame::SetLoginTime(DWORD dwClient)
{
	m_pClient[dwClient].SetLoginTime();
}

void CClientGame::SetGaeaID(DWORD dwClient, DWORD dwGaeaID)
{
	m_pClient[dwClient].SetGaeaID(dwGaeaID);
}

void CClientGame::SetLastMsgTime(DWORD dwClient, DWORD dwTime)
{
	m_pClient[dwClient].SetLastMsgTime(dwTime);
}

DWORD CClientGame::GetLastMsgTime(DWORD dwClient)
{
	return m_pClient[dwClient].GetLastMsgTime();
}

void CClientGame::SetLastSaveTime(DWORD dwClient, DWORD dwTime)
{
	m_pClient[dwClient].SetLastSaveTime(dwTime);
}

DWORD CClientGame::GetLastSaveTime(DWORD dwClient)
{
	return m_pClient[dwClient].GetLastSaveTime();
}

INT CClientGame::GetLoginTime(DWORD dwClient)
{
	CTime crtTime;
	crtTime = CTime::GetCurrentTime();
	CTimeSpan ts = crtTime - m_pClient[dwClient].GetLoginTime();
	return (INT) ts.GetTotalMinutes();
}

CHAR* CClientGame::GetUserID(DWORD dwClient) 
{
	return m_pClient[dwClient].GetUserID();
}

DWORD CClientGame::GetGaeaID(DWORD dwClient)
{
	DWORD dwGaeaID;
	dwGaeaID = m_pClient[dwClient].GetGaeaID();
	return dwGaeaID;
}

// 접속자가 플레이 중인지를 알려준다.
bool CClientGame::IsGaming(DWORD dwClient)
{
	// 온라인 상태이고 가이아 ID 가 있으면 게임중이다
	if (m_pClient[dwClient].IsOnline() && m_pClient[dwClient].GetGaeaID() != GAEAID_NULL)
		return true;
	else
		return false;
}

//DWORD CClientGame::GetUserCharObjID(DWORD dwClient) const
//{
//	return m_pClient[dwClient].dwCharObjID;
//}

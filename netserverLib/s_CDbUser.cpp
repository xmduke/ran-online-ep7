///////////////////////////////////////////////////////////////////////////////
// s_CDbUser.cpp
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note :
// 
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "s_CDbmanager.h"
#include "GLogicData.h"
#include "s_CCfg.h"
#include <strstream>
#include "GLCharAG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// nSvrGrp : 서버 그룹
// nSvrNum : 서버 번호
int CDbmanager::UserLogoutSvr(int nSvrGrp, int nSvrNum)
{
	DB_LIST* pDB = GetFreeConnectionUser();	
	PDBPROCESS pDBProc = pDB->dbproc; 

	RETCODE nRetCode = 0;
	DBINT   nRetDB = 0;

	char szTemp[1000];
	
	StringCchPrintf(szTemp, 1000, "UPDATE UserInfo SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d AND SvrNum=%d", 
						nSvrGrp,
						nSvrNum);	
	
	if (dbcmd(pDBProc, (LPCSTR) szTemp) == FAIL)
	{
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}	
	
	if (dbsqlexec(pDBProc) == FAIL)
	{
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}
	else
	{
		ReleaseConnectionUser(pDB);
		return DB_OK;
	}
}

int CDbmanager::UserLogoutSimple(const char* szUsrID)
{
	DB_LIST* pDB = GetFreeConnectionUser();	
	PDBPROCESS pDBProc = pDB->dbproc; 

	RETCODE nRetCode = 0;
	DBINT   nRetDB = 0;

	// 사용자 ID 를 이용해서 사용자를 로그아웃 시킨다
	std::strstream strTemp2;
	strTemp2 << "Update UserInfo Set UserLoginState=0 WHERE UserID='";
	strTemp2 << szUsrID;
	strTemp2 << "'";
	strTemp2 << '\0';

	nRetCode = dbcmd(pDBProc, (LPCSTR) strTemp2.str());
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : UserLogoutSimple dbcmd %s", szUsrID);
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}
	
	nRetCode = dbsqlexec(pDBProc);
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : UserLogoutSimple dbsqlexec %s", szUsrID);
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}

	ReleaseConnectionUser(pDB);
	return DB_OK;
}

// user_logout
// @userId		char(100),	// 사용자 id
// @usernum	int			// 사용자 번호
// @gametime	int			// 사용시간
int CDbmanager::UserLogout(const char* szUsrID, int nUsrNum, int nGameTime)
{
	DB_LIST* pDB = GetFreeConnectionUser();	
	PDBPROCESS pDBProc = pDB->dbproc; 

	RETCODE nRetCode = 0;
	DBINT   nRetDB = 0;

	if (nGameTime == 0)	nGameTime = 1;

	/* Make the rpc. */
	if (dbrpcinit(pDBProc, "user_logout", (DBSMALLINT)0) == FAIL) 
	{		
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:UserLogout dbrpcinit");
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}
	
	// First Parameter, user id	
	if (dbrpcparam(pDBProc, 
					"@userId",
					(BYTE) 0, 
					(INT) SQLCHAR, 
					100, 
					(DBINT) strlen(szUsrID), 
					(LPCBYTE) szUsrID) == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : UserLogout First Parameter %s", szUsrID);
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}
	// 2nd Parameter, user usernum
	if (dbrpcparam(pDBProc, 
					"@usernum", 
					(BYTE)0, 
					(INT)SQLINT4, 
					-1, 
					(DBINT)-1, 
					(LPCBYTE) &nUsrNum) == FAIL)
	{	
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : UserLogout 2nd Parameter %d", nUsrNum);
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}

	// 3rd Parameter, game time
	if (dbrpcparam(pDBProc, 
					"@gametime", 
					(BYTE)0, 
					(INT)SQLINT4, 
					-1, 
					(DBINT)-1, 
					(LPCBYTE) &nGameTime) == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : UserLogout 3rd Parameter %d", nUsrNum);
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}

	if (dbrpcsend(pDBProc) == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : UserLogout dbrpcsend %s %d %d", szUsrID, nUsrNum, nUsrNum);
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}

	if (dbsqlok(pDBProc) == FAIL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : UserLogout dbsqlok %s %d %d", szUsrID, nUsrNum, nUsrNum);
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}
	
	ReleaseConnectionUser(pDB);
	return DB_OK;
}
/*
	user_verify

	@userId		char(50),
	@userPass 	char(50),
	@userIp		char(50),
	@SvrGrpNum	int,
	@SvrNum		int,
	@nReturn 	int	OUTPUT
*/
int CDbmanager::UserCheck(const char* szUsrID, const char* szPasswd, 
						  const char* szUsrIP, int nSvrGrp, int nSvrNum)
{
	DB_LIST* pDB = GetFreeConnectionUser();	
	PDBPROCESS pDBProc = pDB->dbproc; 

	RETCODE nRetCode = 0;
	DBINT   nRetDB = 0;
	LONG    nRetParam;

	/* Make the rpc. */
	if (dbrpcinit(pDBProc, "user_verify", (DBSMALLINT)0) == FAIL) 
	{
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}
	
	// First Parameter, user id	
	if (dbrpcparam(pDBProc, "@userId",
		(BYTE)0, (INT)SQLCHAR, 100, (DBINT)strlen(szUsrID), (LPCBYTE)szUsrID) == FAIL)
	{
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}
	// 2nd Parameter, user password
	if (dbrpcparam(pDBProc, 
					"@userPass", 
					(BYTE) 0, 
					(INT) SQLCHAR, 
					100, 
					(DBINT) strlen(szPasswd), 
					(LPCBYTE) szPasswd) == FAIL)
	{
		ReleaseConnectionUser(pDB);		
		return DB_ERROR;
	}
	// 3nd Parameter, user ip address
	if (dbrpcparam(pDBProc, 
					"@userIP", 
					(BYTE) 0, 
					(INT) SQLCHAR, 
					100, 
					(DBINT) strlen(szUsrIP), 
					(LPCBYTE) szUsrIP) == FAIL)
	{
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}
	// 4nd Parameter, Server group
	if (dbrpcparam(pDBProc, 
					"@SvrGrpNum", 
					(BYTE) 0, 
					(INT) SQLINT4, 
					-1, 
					-1, 
					(LPCBYTE) &nSvrGrp) == FAIL)
	{
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}
	// 5nd Parameter, user Server number
	if (dbrpcparam(pDBProc, 
					"@SvrNum", 
					(BYTE)0, 
					(INT)SQLINT4, 
					-1, 
					-1, 
					(LPCBYTE) &nSvrNum) == FAIL)
	{
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}
	// 6nd Parameter, Return Parameter
	if (dbrpcparam(pDBProc, 
					"@nReturn", 
					(BYTE) DBRPCRETURN, 
					(INT) SQLINT4, 
					-1, 
					-1, 
					(LPCBYTE) &nRetCode) == FAIL)
	{
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}

	if (dbrpcsend(pDBProc) == FAIL)
	{
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}

	if (dbsqlok(pDBProc) == FAIL) 
	{
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}

	while ((nRetCode = dbresults(pDBProc)) != NO_MORE_RESULTS)
	{
		if (nRetCode == FAIL)
		{
			ReleaseConnectionUser(pDB);
			return DB_ERROR;
		}
	}
	
	if (dbhasretstat(pDBProc))
	{
		nRetParam = dbretstatus(pDBProc);
	}
	else
	{
		nRetParam = DB_ERROR;
	}	
	
	ReleaseConnectionUser(pDB);
	return nRetParam;
}

// 사용자가 캐릭터를 가지고 게임에 조인했을때 정보를 업데이트 한다.
int CDbmanager::UserUpdateCha(int nUsrNum, const char* szChaName)
{
	DB_LIST* pDB = GetFreeConnectionUser();	
	PDBPROCESS pDBProc = pDB->dbproc; 

	RETCODE nRetCode = 0;

	std::strstream strTemp2;
	strTemp2 << "UPDATE UserInfo SET ChaName='";
	strTemp2 << szChaName;
	strTemp2 << "' WHERE UserNum=";
	strTemp2 << nUsrNum;
	strTemp2 << '\0';

	dbcmd(pDBProc, (LPCSTR) strTemp2.str());

	nRetCode = dbsqlexec(pDBProc);
	if (nRetCode == FAIL)
	{
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}
	else
	{
		ReleaseConnectionUser(pDB);
		return DB_OK;
	}	
}

USER_INFO_BASIC CDbmanager::GetUserInfo(const char* szUserId, const char* szPassword)
{
	DB_LIST* pDB = GetFreeConnectionUser();
	PDBPROCESS pDBProc = pDB->dbproc; 

	RETCODE nReturnCode = 0;
	int nUserNum = 0;

	USER_INFO_BASIC uib;	

	std::strstream strTemp;
	strTemp << "SELECT UserNum, UserID, LastLoginDate ";
	strTemp << "FROM UserInfo WHERE UserID = '";
	strTemp << szUserId;
	strTemp << "' AND UserPass ='";
	strTemp << szPassword << "'";
	strTemp << '\0';
	
	uib.nUserNum = 0;
	StringCchCopyN(uib.szUserID, USR_ID_LENGTH, szUserId, USR_ID_LENGTH);	
	
	nReturnCode = dbcmd(pDBProc, (LPCSTR) strTemp.str());
	if (nReturnCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : GetUserInfo dbcmd %s", uib.szUserID);
		ReleaseConnectionUser(pDB);
		return uib;
	}

	// send command buffer to SQL server
	nReturnCode = dbsqlexec (pDBProc); 
	if (nReturnCode == FAIL)
	{
        CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : GetUserInfo dbsqlexec %s", szUserId);
		ReleaseConnectionUser(pDB);
		return uib;
	}

	nReturnCode = dbresults(pDBProc);
	if (nReturnCode != NO_MORE_RESULTS && nReturnCode != FAIL)
	{
		// Bind data
		dbbind(pDBProc, 1, INTBIND, (DBINT)0, (BYTE *) &nUserNum );
	//  dbbind(pDB,		2, INTBIND, 4, (LPSTR)nUserNum);
	//  dbbind(pDB,		3, DATETIMEBIND, 4, (LPSTR)nUserNum);

		dbnextrow(pDBProc); 
		uib.nUserNum = nUserNum;		
	}
	ReleaseConnectionUser(pDB);
	return uib;	
}



// 해당사용자 ID 의 사용자 번호를 가져온다.
int CDbmanager::GetUserNum(const char* szUserId)
{	
	DB_LIST* pDB = GetFreeConnectionUser();
	PDBPROCESS pDBProc = pDB->dbproc; 

	RETCODE nReturnCode = 0;
	int nUserNum = 0;
	
	// 캐릭터 정보를 가져온다.
	std::strstream strTemp;
	strTemp << "SELECT UserNum FROM UserInfo WHERE UserID='" << szUserId << "'";
	strTemp << '\0';
	
	nReturnCode = dbcmd(pDBProc, (LPCSTR) strTemp.str());
	if (nReturnCode == FAIL)
	{
        CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:GetUserNum dbcmd %s", szUserId);
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}
	// send command buffer to SQL server
	nReturnCode = dbsqlexec (pDBProc); 
	if (nReturnCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:GetUserNum dbresults %s", szUserId);
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}

	nReturnCode = dbresults(pDBProc);
	if (nReturnCode != NO_MORE_RESULTS && nReturnCode != FAIL)
	{
		// Bind data
		dbbind(pDBProc, 1, INTBIND, (DBINT)0, (BYTE *) &nUserNum );
		dbnextrow(pDBProc);
		ReleaseConnectionUser(pDB);
		return nUserNum;	
	}
	else
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:GetUserNum dbresults %s", szUserId);
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}
}

int CDbmanager::AddBlockIPList(char* szIP, char* szReason)
{
	DB_LIST* pDB = GetFreeConnectionUser();
	PDBPROCESS pDBProc = pDB->dbproc; 

	RETCODE nReturnCode = 0;
	int nUserNum = 0;
	
	// 캐릭터 정보를 가져온다.
	std::strstream strTemp;
	strTemp << "INSERT INTO BlockAddress (BlockAddress, BlockReason) VALUES('" << szIP << "', '" << szReason << "')";
	strTemp << '\0';
	
	nReturnCode = dbcmd(pDBProc, (LPCSTR) strTemp.str());
	if (nReturnCode == FAIL)
	{
        CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:AddBlockIPList");
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}
	// send command buffer to SQL server
	nReturnCode = dbsqlexec (pDBProc); 
	if (nReturnCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write("ERROR:AddBlockIPList");
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}	
	else
	{
		ReleaseConnectionUser(pDB);
		return DB_OK;
	}
}
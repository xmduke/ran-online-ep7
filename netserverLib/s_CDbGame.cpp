///////////////////////////////////////////////////////////////////////////////
// s_CDbGame.cpp
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
#include "s_CDbAction.h"
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

// 새로운 캐릭터를 생성한다.
// 사용자번호
// 캐릭터직업
// 서버 그룹
// 캐릭터명
int CDbmanager::CreateNewCharacter(SCHARDATA2* pCharData2)
{
	DB_LIST* pDB = GetFreeConnectionGame();	
	PDBPROCESS pDBProc = pDB->dbproc; 

	RETCODE nRetCode = 0;
	
	int nChaNum = 0;
	int nChaMaxNum = 0;
	DWORD dwUserNum = pCharData2->m_dwUserID;
	DWORD dwSvrNum = pCharData2->m_dwServerID;

	// SCHARDATA2 sCha;
	// sCha.operator =n;
	
	// 현재 생성한 캐릭터 갯수를 가져온다.
	std::strstream strTemp2;
	strTemp2 << "SELECT COUNT(*) FROM ChaInfo WHERE UserNum=";
	strTemp2 << dwUserNum;
	strTemp2 << " AND SGNum=";
	strTemp2 << dwSvrNum;
	strTemp2 << '\0';

	dbcmd(pDBProc, (LPCSTR) strTemp2.str());
	
	nRetCode = dbsqlexec(pDBProc);
	if (nRetCode == FAIL)
	{
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}

	nRetCode = dbresults(pDBProc);
	if (nRetCode != NO_MORE_RESULTS && nRetCode != FAIL)
	{
		// Bind data
		dbbind(pDBProc, 1, INTBIND, (DBINT)0, (BYTE *) &nChaMaxNum );
		dbnextrow(pDBProc); // get rows
		dbcancel(pDBProc);
        // 이미 생성한 캐릭터 숫자보다 많이 생성했을때는 리턴..
		if (nChaMaxNum >= MAX_ONESERVERCHAR_NUM)
		{
			ReleaseConnectionGame(pDB);
			return DB_CHA_MAX;
		}
	} 
	else 
	{
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}

	// 새로운 캐릭터를 생성한다.
	// 민첩 ChaDex, wDex
	// 지력 ChaIntel, wInt
	// 체력 ChaStrong, wStr
	// 힘   ChaPower, wPow
	// 정신 ChaSpirit, wSpi
	// 근력 ChaStrength, wSta
	// 2003-11-27 : add ChaSchool, ChaHair, ChaFace, ChaLiving, ChaActionSlot
	std::strstream strTemp;
	strTemp << "INSERT INTO ChaInfo(UserNum, SGNum, ChaName, ChaTribe, ChaClass, ";
	strTemp << "ChaSchool, ChaHair, ChaFace, ChaLiving, "; // 2003-11-27 : add
	strTemp << "ChaBright, ChaLevel, ChaMoney, ChaDex, ChaIntel,";
	strTemp << "ChaStrong, ChaPower, ChaSpirit, ChaStrength, ChaStRemain, ";
	strTemp << "ChaAttackP, ChaDefenseP, ChaFightA, ChaShootA, ChaExp, ";
	strTemp << "ChaSkillPoint, ChaHP, ChaMP, ChaSP, ChaPK, ";
	strTemp << "ChaStartMap, ChaStartGate, ChaPosX, ChaPosY, ChaPosZ,";
	strTemp << "ChaSkills, ChaSkillSlot, ChaActionSlot, ChaPutOnItems, ChaInven ) Values ( ";
		
	strTemp << dwUserNum << ",";
	strTemp << dwSvrNum << ",";
	strTemp << "'" << pCharData2->m_szName << "',";
	strTemp << pCharData2->m_emTribe << ","; 
	strTemp << pCharData2->m_emClass << ",";

	// 2003-11-27 : add
	strTemp << pCharData2->m_wSchool << ",";
	strTemp << pCharData2->m_wHair << ",";
	strTemp << pCharData2->m_wFace << ",";
	strTemp << pCharData2->m_nLiving << ",";	

    strTemp << pCharData2->m_nBright << ",";
	strTemp << pCharData2->m_wLevel << ",";
	strTemp << pCharData2->m_lnMoney << ","; 
	strTemp << pCharData2->m_sStats.wDex << ","; // 민첩 ChaDex, wDex
	strTemp << pCharData2->m_sStats.wInt << ","; // 지력 ChaIntel, wInt

	strTemp << pCharData2->m_sStats.wStr << ","; // 체력 ChaStrong, wStr
	strTemp << pCharData2->m_sStats.wPow << ","; // 힘   ChaPower, wPow
	strTemp << pCharData2->m_sStats.wSpi << ","; // 정신 ChaSpirit, wSpi
	strTemp << pCharData2->m_sStats.wSta << ","; // 근력 ChaStrength, wSta
	strTemp << pCharData2->m_wStatsPoint << ",";

	strTemp << pCharData2->m_wAP << ",";
	strTemp << pCharData2->m_wDP << ",";
	strTemp << pCharData2->m_wPA << ",";
	strTemp << pCharData2->m_wSA << ",";
	strTemp << pCharData2->m_sExperience.lnNow << ",";

	strTemp << pCharData2->m_dwSkillPoint << ",";		
	strTemp << pCharData2->m_sHP.dwData << ",";
	strTemp << pCharData2->m_sMP.dwData << ",";
	strTemp << pCharData2->m_sSP.dwData << ",";
	strTemp << pCharData2->m_wPK << ",";

	strTemp << pCharData2->m_sStartMapID.dwID << ",";
	strTemp << pCharData2->m_dwStartGate << ",";
	strTemp << pCharData2->m_vStartPos.x << ",";
	strTemp << pCharData2->m_vStartPos.y << ",";
	strTemp << pCharData2->m_vStartPos.z << ",";

	strTemp << " '', '', '', '', '')";
	strTemp << '\0';

	dbcmd(pDBProc, (LPCSTR) strTemp.str());	
    	
	// send command buffer to SQL server
	nRetCode = dbsqlexec(pDBProc);
	
	if (nRetCode == FAIL) 
	{
		ReleaseConnectionGame(pDB);
		return DB_CHA_DUF;
	}
	dbcancel(pDBProc);

	// 생성한 캐릭터의 고유번호를 가져온다.
	dbcmd(pDBProc, (LPCSTR) "SELECT @@IDENTITY" );
	dbsqlexec(pDBProc);
	nRetCode = dbresults(pDBProc);

	if (nRetCode != NO_MORE_RESULTS && nRetCode != FAIL) 
	{
		// 캐릭터 고유번호 세팅
		dbbind(pDBProc, 1, INTBIND, (DBINT)0, (BYTE *) &nChaNum );
		dbnextrow(pDBProc);
		pCharData2->m_dwCharID; 
		ReleaseConnectionGame(pDB);
		
		CByteStream ByteStream;
		LPBYTE pBuffer = NULL;
		DWORD dwSize;
		// Character Skill 
		pCharData2->GETEXPSKILLS_BYBUF(ByteStream);		
		ByteStream.GetBuffer(pBuffer, dwSize);
		WriteImage("ChaInfo.ChaSkills", nChaNum, (char *) pBuffer, dwSize);

		// Character Skill Quick Slot
		pCharData2->GETSKILL_QUICKSLOT(ByteStream);		
		ByteStream.GetBuffer(pBuffer, dwSize);
		WriteImage("ChaInfo.ChaSkillSlot", nChaNum, (char *) pBuffer, dwSize);

		// 2003-11-27 : add
        // Character Action Quick Slot
		pCharData2->GETACTION_QUICKSLOT(ByteStream);		
		ByteStream.GetBuffer(pBuffer, dwSize);
		WriteImage("ChaInfo.ChaActionSlot", nChaNum, (char *) pBuffer, dwSize);

		// Character Put on item
		pCharData2->GETPUTONITEMS_BYBUF(ByteStream);
		ByteStream.GetBuffer(pBuffer, dwSize);
		WriteImage("ChaInfo.ChaPutOnItems", nChaNum, (char *) pBuffer, dwSize);

		// Character Inventory
		ByteStream.ClearBuffer();
		pCharData2->GETINVENTORYE_BYBUF(ByteStream);
		ByteStream.GetBuffer(pBuffer, dwSize);		
		WriteImage("ChaInfo.ChaInven", nChaNum, (char *) pBuffer, dwSize);

		// User Inventory
		// 1. Check User Inven		
		// 2. If exist skip
		// 3. not exist, write iventory image
		BOOL bInven = CheckInven(CCfg::GetInstance()->GetServerGroup(), dwUserNum);
		if (!bInven)
		{
			MakeUserInven(CCfg::GetInstance()->GetServerGroup(), dwUserNum);
		}			
		return nChaNum;
	} 
	else 
	{
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}
}

// 캐릭터를 저장한다
int	CDbmanager::SaveCharacter ( LPVOID _pbuffer )
{
	SCHARDATA2* pCharData2 = reinterpret_cast<SCHARDATA2*> ( _pbuffer );

	if (pCharData2 == NULL)
		return DB_ERROR;

	DWORD dwChaNum = pCharData2->m_dwCharID;
	
	if (dwChaNum < 0)
		return DB_ERROR;
	
	DWORD dwUserNum = pCharData2->m_dwUserID;
	DWORD dwSvrNum = pCharData2->m_dwServerID;
	DWORD dwChaID = pCharData2->m_dwCharID;
	

	// 민첩 ChaDex, wDex
	// 지력 ChaIntel, wInt
	// 체력 ChaStrong, wStr
	// 힘   ChaPower, wPow
	// 정신 ChaSpirit, wSpi
	// 근력 ChaStrength, wSta

	// 캐릭터를 저장 한다.
	std::strstream strTemp;
	strTemp << "UPDATE ChaInfo SET ";
	strTemp << "  ChaBright=" << pCharData2->m_nBright;		
	strTemp << ", ChaLevel="  << pCharData2->m_wLevel;
	strTemp << ", ChaMoney="  << pCharData2->m_lnMoney;
	strTemp << ", ChaDex="    << pCharData2->m_sStats.wDex;		// 민첩
	strTemp << ", ChaIntel="  << pCharData2->m_sStats.wInt;		// 지력
	
	// 2003-11-27 : add
	strTemp << ", ChaSchool=" << pCharData2->m_wSchool;
	strTemp << ", ChaHair="   << pCharData2->m_wHair;
	strTemp << ", ChaFace="   << pCharData2->m_wFace;
	strTemp << ", ChaLiving=" << pCharData2->m_nLiving;

	strTemp << ", ChaStrong="   << pCharData2->m_sStats.wStr;		// 체력
	strTemp << ", ChaPower="    << pCharData2->m_sStats.wPow;		// 힘
	strTemp << ", ChaSpirit="   << pCharData2->m_sStats.wSpi;		// 정신
	strTemp << ", ChaStrength=" << pCharData2->m_sStats.wSta;	// 근력
	strTemp << ", ChaStRemain=" << pCharData2->m_wStatsPoint;

	strTemp << ", ChaAttackP="  << pCharData2->m_wAP;	
	strTemp << ", ChaDefenseP=" << pCharData2->m_wDP;
	strTemp << ", ChaFightA="   << pCharData2->m_wPA;
	strTemp << ", ChaShootA="   << pCharData2->m_wSA;
	strTemp << ", ChaExp="      << pCharData2->m_sExperience.lnNow;

	strTemp << ", ChaSkillPoint=" << pCharData2->m_dwSkillPoint;	
	strTemp << ", ChaHP=" << pCharData2->m_sHP.dwData;
	strTemp << ", ChaMP=" << pCharData2->m_sMP.dwData;
	strTemp << ", ChaSP=" << pCharData2->m_sSP.dwData;
	strTemp << ", ChaPK=" << pCharData2->m_wPK;

	strTemp << ", ChaStartMap="  << pCharData2->m_sStartMapID.dwID;	
	strTemp << ", ChaStartGate=" << pCharData2->m_dwStartGate;
	strTemp << ", ChaPosX="      << pCharData2->m_vStartPos.x;
	strTemp << ", ChaPosY="      << pCharData2->m_vStartPos.y;
	strTemp << ", ChaPosZ="      << pCharData2->m_vStartPos.z;

    // 2003-09-24 : add
    strTemp << ", ChaSaveMap="  << pCharData2->m_sSaveMapID.dwID;
    strTemp << ", ChaSavePosX=" << pCharData2->m_vSavePos.x;
    strTemp << ", ChaSavePosY=" << pCharData2->m_vSavePos.y;
    strTemp << ", ChaSavePosZ=" << pCharData2->m_vSavePos.z;
    ////////////////////////////////////////////////////////////

	strTemp << "  WHERE ChaNum=" << dwChaNum;
	strTemp << '\0';
	
	DB_LIST* pDB = GetFreeConnectionGame();	
	PDBPROCESS pDBProc = pDB->dbproc;
	RETCODE nRetCode = 0;	

	dbcmd(pDBProc, (LPCSTR) strTemp.str());
    	
	// send command buffer to SQL server
	nRetCode = dbsqlexec(pDBProc);
	
	if (nRetCode == FAIL) 
	{
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}		
	ReleaseConnectionGame(pDB);
	
	CByteStream ByteStream;
	LPBYTE pBuffer = NULL;
	DWORD dwSize;
	
	// Skill 
	pCharData2->GETEXPSKILLS_BYBUF(ByteStream);		
	ByteStream.GetBuffer(pBuffer, dwSize);	
	if (pBuffer != NULL)
		WriteImage("ChaInfo.ChaSkills", dwChaNum, (char *) pBuffer, dwSize);
	pBuffer = NULL;

	// Skill Quick Slot	
	pCharData2->GETSKILL_QUICKSLOT(ByteStream);		
	ByteStream.GetBuffer(pBuffer, dwSize);	
	if (pBuffer != NULL)
		WriteImage("ChaInfo.ChaSkillSlot", dwChaNum, (char *) pBuffer, dwSize);
	pBuffer = NULL;

	// 2003-11-27 : Add
	// Action Quick Slot
	pCharData2->GETACTION_QUICKSLOT(ByteStream);
	ByteStream.GetBuffer(pBuffer, dwSize);
	if (pBuffer != NULL)
		WriteImage("ChaInfo.ChaActionSlot", dwChaNum, (char *) pBuffer, dwSize);
	pBuffer = NULL;

	// Put on item
	pCharData2->GETPUTONITEMS_BYBUF(ByteStream);
	ByteStream.GetBuffer(pBuffer, dwSize);
	if (pBuffer != NULL)
		WriteImage("ChaInfo.ChaPutOnItems", dwChaNum, (char *) pBuffer, dwSize);
	pBuffer = NULL;

	// Inventory
	ByteStream.ClearBuffer();
	pCharData2->GETINVENTORYE_BYBUF(ByteStream);
	ByteStream.GetBuffer(pBuffer, dwSize);
	if (pBuffer != NULL)
		WriteImage("ChaInfo.ChaInven", dwChaNum, (char *) pBuffer, dwSize);
	pBuffer = NULL;

    // 사용자 인벤토리 저장
	if ( pCharData2->m_bServerStorage )
	{
		pCharData2->GETSTORAGE_BYBUF(ByteStream);
		ByteStream.GetBuffer(pBuffer, dwSize);		
		if (pBuffer != NULL)
		{
			WriteUserInven(CCfg::GetInstance()->GetServerGroup(), 
							pCharData2->m_lnStorageMoney, 
							pCharData2->m_dwUserID , 
							(char *) pBuffer, 
							dwSize);
		}
	}	
	return DB_OK;	
}

// 캐릭터 삭제
int CDbmanager::DelCharacter(int nUsrNum, int nChaNum, const char* szPass2)
{		
	int		nRetCode = 0;	
	DB_LIST* pDB = GetFreeConnectionUser();
	assert(pDB);
	PDBPROCESS pDBProc = pDB->dbproc;
	
	// 2차 비밀번호 체크
	std::strstream strTemp;
	strTemp << "SELECT UserNum FROM UserInfo WHERE UserNum=";
	strTemp << nUsrNum;
	strTemp << " AND UserPass2='";
	strTemp << szPass2;
	strTemp << "'";
	strTemp << '\0';

	dbcmd(pDBProc, (LPCSTR) strTemp.str());

	nRetCode = dbsqlexec(pDBProc);
	if (nRetCode == FAIL)
	{			
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}
	
	nRetCode = dbresults(pDBProc);
	if (nRetCode == NO_MORE_RESULTS || nRetCode == FAIL)
	{		
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}

	if (dbnextrow(pDBProc) == NO_MORE_ROWS )
	{
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}

	ReleaseConnectionUser(pDB);
	
    // 캐릭터 삭제
	pDB	= GetFreeConnectionGame();
	assert(pDB);
	pDBProc = pDB->dbproc;

	std::strstream strTemp2;
	strTemp2 << "DELETE FROM ChaInfo WHERE ChaNum=";
	strTemp2 << nChaNum;
	strTemp2 << '\0';
	
	dbcmd (pDBProc, (LPCSTR) strTemp2.str());
	
	// send command buffer to SQL server
	nRetCode = dbsqlexec (pDBProc);
	
	if (nRetCode == FAIL)
	{			
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}
	else
	{
		ReleaseConnectionGame(pDB);
		return DB_OK;
	}
}

// 해당 사용자의 캐릭터 기본 정보를 가져온다.
// 캐릭터의 고유번호만을 가져온다.
int CDbmanager::GetChaBAInfo(int nUsrNum, int nSvrGrp, NET_CHA_BBA_INFO* ncbi)
{
	DB_LIST* pDB = GetFreeConnectionGame();
	PDBPROCESS pDBProc = pDB->dbproc; 
	
	int	nPos = 0;
	RETCODE nRetCode = 0;
	int nChaNum = 0;	
	
	std::strstream strTemp;
	strTemp << "SELECT ChaNum FROM ChaInfo WHERE UserNum=";
	strTemp << nUsrNum << " AND SGNum=" << nSvrGrp << " ORDER BY ChaNum";
	strTemp << '\0';

	dbcmd (pDBProc, (LPCSTR) strTemp.str());

	// send command buffer to SQL server
	nRetCode = dbsqlexec (pDBProc); 
	if (nRetCode == FAIL)
	{		
		ncbi->nChaSNum = 0;		
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}

	int nLoop = 0;

	nRetCode = dbresults(pDBProc);
	if ((nRetCode != NO_MORE_RESULTS) && (nRetCode != FAIL) )
	{
		// Bind data
		dbbind(pDBProc, 1, INTBIND, (DBINT) 0, (BYTE *) &nChaNum);
		while ( dbnextrow(pDBProc) != NO_MORE_ROWS ) // get all rows	 
		{ 
			ncbi->nChaNum[nLoop] = nChaNum;			
			nLoop++;
			if (nLoop == MAX_ONESERVERCHAR_NUM)
				break;			
		}
		ncbi->nChaSNum = nLoop;
		ReleaseConnectionGame(pDB);
		return DB_OK;
	}
	else
	{
		ncbi->nChaSNum = 0;
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}
}

// 해당 사용자의 캐릭터 정보를 가져온다.
// 캐릭터의 고유번호만을 가져온다.
int CDbmanager::GetChaAllInfo(int nUserNum)
{		
	// 2003-11-27 : Delete All Function

	//DB_LIST* pDB = GetFreeConnectionGame();
	//PDBPROCESS pDBProc = pDB->dbproc; 
	//
	//int	nPos = 0;
	//RETCODE nRetCode = 0;
	//int nChaNum = 0;

	//std::strstream strTemp;
	//strTemp << "SELECT ChaNum FROM ChaInfo WHERE UserNum =" << nUserNum;
	//strTemp << '\0';
	//
	//dbcmd (pDBProc, (LPCSTR) strTemp.str());

	//// send command buffer to SQL server
	//nRetCode = dbsqlexec (pDBProc); 
	//if (nRetCode == FAIL)
	//{
	//	ReleaseConnectionGame(pDB);
	//	return DB_ERROR;
	//}

	//nRetCode = dbresults(pDBProc);
	//if ((nRetCode != NO_MORE_RESULTS) && (nRetCode != FAIL) )
	//{
	//	// Bind data
	//	dbbind(pDBProc, 1,    INTBIND, (DBINT) 0, (BYTE *) &nChaNum);
	//	while ( dbnextrow(pDBProc) != NO_MORE_ROWS ) // get all rows	 
	//	{
 //           GetCharacter(nChaNum);
	//	}	
	//	ReleaseConnectionGame(pDB);
	//	return DB_OK;
	//}
	//else
	//{
	//	ReleaseConnectionGame(pDB);
	//	return DB_ERROR;
	//}	
	return DB_OK;
}

// 캐릭터 정보를 가져온다 (바이너리 제거 데이타)
SCHARDATA2* CDbmanager::GetCharacter(int nChaNum)
{
	DB_LIST* pDB = GetFreeConnectionGame();
	PDBPROCESS pDBProc = pDB->dbproc;
	RETCODE nRetCode = 0;	
	SCHARDATA2* pChaData = new SCHARDATA2;
	
	// 캐릭터 정보를 가져온다.
	std::strstream strTemp;
	strTemp << "SELECT UserNum, SGNum, ChaName, ChaTribe, ChaClass, ";
	strTemp << "ChaBright, ChaLevel, ChaDex, ChaIntel, ChaPower, ";
	strTemp << "ChaStrong, ChaSpirit, ChaStrength, ChaStRemain, ChaAttackP, ";
	strTemp << "ChaDefenseP, ChaFightA, ChaShootA,  ChaSkillPoint, ChaHP, ";
	strTemp << "ChaMP, ChaSP, ChaPK, ChaStartMap, ChaStartGate, ";
	strTemp << "ChaPosX, ChaPosY, ChaPosZ, CAST(ChaMoney AS CHAR(15)), CAST(ChaExp AS CHAR(15)), ";
    // 2003-09-24 : Add
    strTemp << "ChaSaveMap, ChaSavePosX, ChaSavePosY, ChaSavePosZ, ";
	// 2003-11-27 : Add
	strTemp << "ChaSchool, ChaHair, ChaFace, ChaLiving ";
    ////////////////////////////////////////////////////////////////

	strTemp << "FROM ChaInfo WHERE ChaNum=" << nChaNum;
	strTemp << '\0';

	dbcmd(pDBProc, (LPCSTR) strTemp.str());

	// send command buffer to SQL server
	nRetCode = dbsqlexec (pDBProc); 
	if (nRetCode == FAIL)
	{
		delete pChaData;
		ReleaseConnectionGame(pDB);
		return NULL;
	}

	nRetCode = dbresults(pDBProc);
	if ((nRetCode != NO_MORE_RESULTS) && (nRetCode != FAIL) )
	{		
		// dwUserNum 사용자 번호
		// dwSvrNum 서버그룹
		// Bind data
		dbbind(pDBProc,  1,	INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_dwUserID);
		dbbind(pDBProc,  2, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_dwServerID);
		dbbind(pDBProc,  3,	NTBSTRINGBIND, (DBINT) CHR_ID_LENGTH, (LPBYTE) &pChaData->m_szName);
		dbbind(pDBProc,  4,	INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_emTribe);
		dbbind(pDBProc,  5, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_emClass);

		dbbind(pDBProc,  6, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_nBright);
		dbbind(pDBProc,  7, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_wLevel);		
		dbbind(pDBProc,  8, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sStats.wDex); // 민첩
		dbbind(pDBProc,  9, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sStats.wInt); // 지력
		dbbind(pDBProc, 10, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sStats.wPow); // 힘

		dbbind(pDBProc, 11, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sStats.wStr); // 체력	
		dbbind(pDBProc, 12, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sStats.wSpi); // 정신
		dbbind(pDBProc, 13, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sStats.wSta); // 근력
		dbbind(pDBProc, 14, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_wStatsPoint);
		dbbind(pDBProc, 15, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_wAP);

		dbbind(pDBProc, 16, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_wDP);
		dbbind(pDBProc, 17, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_wPA);
		dbbind(pDBProc, 18, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_wSA);
		dbbind(pDBProc, 19, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_dwSkillPoint);		
		dbbind(pDBProc, 20, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sHP.dwData);

		dbbind(pDBProc, 21, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sMP.dwData);
		dbbind(pDBProc, 22, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sSP.dwData);
		dbbind(pDBProc, 23, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_wPK);
		dbbind(pDBProc, 24, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sStartMapID.dwID);
		dbbind(pDBProc, 25, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_dwStartGate);

		dbbind(pDBProc, 26, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vStartPos.x);

		dbbind(pDBProc, 27, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vStartPos.y);
		dbbind(pDBProc, 28, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vStartPos.z);		
		char szMoney[15+1];
		dbbind(pDBProc, 29,	NTBSTRINGBIND, (DBINT) 0, (LPBYTE) szMoney);		
		char szExp[15+1];
		dbbind(pDBProc, 30,	NTBSTRINGBIND, (DBINT) 0, (LPBYTE) szExp);	

        // 2003-09-24 : Add
        dbbind(pDBProc, 31, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sSaveMapID.dwID);
        dbbind(pDBProc, 32, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vSavePos.x);
        dbbind(pDBProc, 33, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vSavePos.y);
        dbbind(pDBProc, 34, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vSavePos.z);
		
		// 2003-11-27 : Add
		dbbind(pDBProc, 35, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_wSchool);
		dbbind(pDBProc, 36, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_wHair);
		dbbind(pDBProc, 37, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_wFace);
		dbbind(pDBProc, 38, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_nLiving);
        //////////////////////////////////////////////////////////////////////////////
		
		dbnextrow(pDBProc); 

		// Money data type convert
		// Character money		
		pChaData->m_lnMoney = _atoi64(szMoney);
		// Character experience		
		pChaData->m_sExperience.lnNow = _atoi64(szExp);
	}
	else
	{
		delete pChaData;
		ReleaseConnectionGame(pDB);
		return NULL;
	}
	ReleaseConnectionGame(pDB);
	return pChaData;	
}

// 캐릭터 정보를 가져온다 (바이너리 포함데이타)
int CDbmanager::GetCharacterInfo(int nChaNum, SCHARDATA2* pChaData2)
{
	DB_LIST* pDB = GetFreeConnectionGame();
	PDBPROCESS pDBProc = pDB->dbproc;
	RETCODE nRetCode = 0;
		
	// 캐릭터 정보를 가져온다.
	std::strstream strTemp;
	strTemp << "SELECT UserNum, SGNum, ChaName, ChaTribe, ChaClass, ";
	strTemp << "ChaBright, ChaLevel, ChaDex, ChaIntel, ChaPower, ";
	strTemp << "ChaStrong, ChaSpirit, ChaStrength, ChaStRemain, ChaAttackP, ";
	strTemp << "ChaDefenseP, ChaFightA, ChaShootA,  ChaSkillPoint, ChaHP, ";
	strTemp << "ChaMP, ChaSP, ChaPK, ChaStartMap, ChaStartGate, ";
	strTemp << "ChaPosX, ChaPosY, ChaPosZ, CAST(ChaMoney AS CHAR(15)), CAST(ChaExp AS CHAR(15)), ";
    // 2003-09-24 : Add
    strTemp << "ChaSaveMap, ChaSavePosX, ChaSavePosY, ChaSavePosZ, ";
	// 2003-11-27 : Add
	strTemp << "ChaSchool, ChaHair, ChaFace, ChaLiving ";
    ////////////////////////////////////////////////////////////////
    
    strTemp << "FROM ChaInfo WHERE ChaNum=" << nChaNum;
	strTemp << '\0';
	
	nRetCode = dbcmd(pDBProc, (LPCSTR) strTemp.str());
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : GetCharacterInfo dbcmd %d", nChaNum);
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}

	// send command buffer to SQL server
	nRetCode = dbsqlexec (pDBProc); 
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : GetCharacterInfo dbsqlexec %d", nChaNum);		
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}	

	nRetCode = dbresults(pDBProc);
	if ((nRetCode != NO_MORE_RESULTS) && (nRetCode != FAIL) )
	{		
		// dwUserNum 사용자 번호
		// dwSvrNum 서버그룹
		// Bind data
		dbbind(pDBProc,  1,	INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_dwUserID);
		dbbind(pDBProc,  2, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_dwServerID);
		dbbind(pDBProc,  3,	NTBSTRINGBIND, (DBINT) CHR_ID_LENGTH, (LPBYTE) &pChaData2->m_szName);
		dbbind(pDBProc,  4,	INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_emTribe);
		dbbind(pDBProc,  5, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_emClass);

		dbbind(pDBProc,  6, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_nBright);
		dbbind(pDBProc,  7, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_wLevel);		
		dbbind(pDBProc,  8, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sStats.wDex); // 민첩
		dbbind(pDBProc,  9, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sStats.wInt); // 지력
		dbbind(pDBProc, 10, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sStats.wPow); // 힘

		dbbind(pDBProc, 11, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sStats.wStr); // 체력	
		dbbind(pDBProc, 12, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sStats.wSpi); // 정신
		dbbind(pDBProc, 13, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sStats.wSta); // 근력
		dbbind(pDBProc, 14, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_wStatsPoint);
		dbbind(pDBProc, 15, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_wAP);

		dbbind(pDBProc, 16, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_wDP);
		dbbind(pDBProc, 17, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_wPA);
		dbbind(pDBProc, 18, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_wSA);
		dbbind(pDBProc, 19, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_dwSkillPoint);		
		dbbind(pDBProc, 20, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sHP.dwData);

		dbbind(pDBProc, 21, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sMP.dwData);
		dbbind(pDBProc, 22, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sSP.dwData);
		dbbind(pDBProc, 23, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_wPK);
		dbbind(pDBProc, 24, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sStartMapID.dwID);
		dbbind(pDBProc, 25, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_dwStartGate);

		dbbind(pDBProc, 26, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_vStartPos.x);

		dbbind(pDBProc, 27, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_vStartPos.y);
		dbbind(pDBProc, 28, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_vStartPos.z);		
				
		char szMoney[15+1];
		dbbind(pDBProc, 29,	NTBSTRINGBIND, (DBINT) 0, (LPBYTE) szMoney);		
		char szExp[15+1];
		dbbind(pDBProc, 30,	NTBSTRINGBIND, (DBINT) 0, (LPBYTE) szExp);

        // 2003-09-24 : Add
        dbbind(pDBProc, 31, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sSaveMapID.dwID);
        dbbind(pDBProc, 32, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_vSavePos.x);
        dbbind(pDBProc, 33, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_vSavePos.y);
        dbbind(pDBProc, 34, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_vSavePos.z);
		// 2003-11-27 : Add
		dbbind(pDBProc, 35, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_wSchool);
		dbbind(pDBProc, 36, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_wHair);
		dbbind(pDBProc, 37, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_wFace);
		dbbind(pDBProc, 38, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_nLiving);
        //////////////////////////////////////////////////////////////////////////////
		
		dbnextrow(pDBProc); 
		ReleaseConnectionGame(pDB);	

		// Money data type convert
		// Character money		
		pChaData2->m_lnMoney = _atoi64(szMoney);
		// Character experience		
		pChaData2->m_sExperience.lnNow = _atoi64(szExp);
		
		CByteStream ByteStream;

		// Skill
		nRetCode = ReadImage("ChaInfo.ChaSkills", nChaNum, ByteStream);
		if (nRetCode == DB_ERROR)
			return DB_ERROR;
		pChaData2->SETEXPSKILLS_BYBUF(ByteStream);

		// Skill Quick Slot
		nRetCode = ReadImage("ChaInfo.ChaSkillSlot", nChaNum, ByteStream);
		if (nRetCode == DB_ERROR)
			return DB_ERROR;
		pChaData2->SETSKILL_QUICKSLOT(ByteStream);
		
		// 2003-11-27 : Add
		// Action Quick Slot
		nRetCode = ReadImage("ChaInfo.ChaActionSlot", nChaNum, ByteStream);
		if (nRetCode == DB_ERROR)
			return DB_ERROR;
		pChaData2->SETACTION_QUICKSLOT(ByteStream);

		// 착용아이템
		nRetCode = ReadImage("ChaInfo.ChaPutOnItems", nChaNum, ByteStream);
		if (nRetCode == DB_ERROR)
			return DB_ERROR;
		SETPUTONITEMS_BYBUF(pChaData2->m_PutOnItems,ByteStream);

		// 캐릭터 인벤토리
		nRetCode = ReadImage("ChaInfo.ChaInven", nChaNum, ByteStream);
		if (nRetCode == DB_ERROR)
			return DB_ERROR;
		pChaData2->SETINVENTORY_BYBUF(ByteStream);
		
		return DB_OK;
	}
	else
	{	
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : GetCharacterInfo dbresults %d", nChaNum);				
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}
}

int CDbmanager::GetCharacterFull(int nChaNum, SCHARDATA2* pChaData2)
{
	int nNET;
	nNET = GetCharacterInfo ( nChaNum, pChaData2 );
	if ( nNET!=DB_OK )	return nNET;

	//	Note : 유져 인벤을 읽어오고, 읽어온 것으로 처리.
	//
	nNET = ReadUserInven ( pChaData2  );
	if ( nNET!=DB_OK )	return nNET;

	pChaData2->m_bServerStorage = TRUE;

	return DB_OK;
}

// 캐릭터 이름, 클래스, 시작맵, 시작게이트, 포지션을 가져온다
int CDbmanager::GetCharacterInfo(int nChaNum, GLCHARAG_DATA* pChaData)
{
	assert(pChaData&&"(GLCHARAG_DATA*)의 값이 유효하지 않습니다.");

	DB_LIST* pDB = GetFreeConnectionGame();
	PDBPROCESS pDBProc = pDB->dbproc;
	RETCODE nRetCode = 0;
		
	// 캐릭터 정보를 가져온다.
	std::strstream strTemp;
	strTemp << "SELECT UserNum, SGNum, ChaName, ChaClass, ChaStartMap, ChaStartGate, ";
	strTemp << "ChaPosX, ChaPosY, ChaPosZ, ";
    // Add : 2003-09-24
    strTemp << "ChaSaveMap, ChaSavePosX, ChaSavePosY, ChaSavePosZ ";    
    //////////////////////////////////////////////////////////////////////////////
	strTemp << "FROM ChaInfo WHERE ChaNum=" << nChaNum;
	strTemp << std::ends;
	
	nRetCode = dbcmd(pDBProc, (LPCSTR) strTemp.str());
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : GetCharacterInfo dbcmd %d", nChaNum);
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}

	// send command buffer to SQL server
	nRetCode = dbsqlexec (pDBProc); 
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : GetCharacterInfo dbsqlexec %d", nChaNum);		
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}	

	nRetCode = dbresults(pDBProc);
	if ((nRetCode != NO_MORE_RESULTS) && (nRetCode != FAIL) )
	{
		// dwUserNum 사용자 번호
		// dwSvrNum 서버그룹
		// Bind data
		dbbind(pDBProc,  1,	INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_dwUserID);
		dbbind(pDBProc,  2, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_dwServerID);
		dbbind(pDBProc,  3,	NTBSTRINGBIND, (DBINT) CHR_ID_LENGTH, (LPBYTE) &pChaData->m_szName);
		dbbind(pDBProc,  4, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_emClass);

		dbbind(pDBProc,  5, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sStartMapID.dwID);
		dbbind(pDBProc,  6, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_dwStartGate);

		dbbind(pDBProc,  7, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vStartPos.x);
		dbbind(pDBProc,  8, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vStartPos.y);
		dbbind(pDBProc,  9, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vStartPos.z);

        // Add : 2003-09-24
        dbbind(pDBProc, 10, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sSaveMapID.dwID);
        dbbind(pDBProc, 11, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vSavePos.x);
        dbbind(pDBProc, 12, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vSavePos.y);
        dbbind(pDBProc, 13, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vSavePos.z);
        //////////////////////////////////////////////////////////////////////////////
		
		dbnextrow(pDBProc); 
		ReleaseConnectionGame(pDB);	

		return DB_OK;
	}
	else
	{	
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : GetCharacterInfo dbresults %d", nChaNum);				
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}
}

// 캐릭터 선택 로비에서 보여질 캐릭터 정보를 가져온다.
int CDbmanager::GetChaBInfo(int nChaNum, SCHARINFO_LOBBY* sci)
{
	DB_LIST* pDB = GetFreeConnectionGame();
	PDBPROCESS pDBProc = pDB->dbproc; 
	
	std::strstream strTemp;
	strTemp << "SELECT ChaName, ChaClass, ChaDex, ChaIntel, ChaPower,";
	strTemp << "ChaStrong, ChaSpirit, ChaStrength, ChaLevel, ";
	// 2003-11-27 : Add
	strTemp << "ChaHair, ChaFace FROM ChaInfo ";
	strTemp << "WHERE ChaNum=" << nChaNum;
	strTemp << '\0';
	
	dbcmd (pDBProc, (LPCSTR) strTemp.str());
	// send command buffer to SQL server
	nRetCode = dbsqlexec (pDBProc);
	if (nRetCode == FAIL)
	{
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}
	nRetCode = dbresults(pDBProc);
	if (nRetCode != NO_MORE_RESULTS && nRetCode != FAIL)
	{
		// Bind data
		sci->m_dwCharID = nChaNum;
		dbbind(pDBProc,  1,	NTBSTRINGBIND, (DBINT) CHR_ID_LENGTH, (LPBYTE) &sci->m_szName);
		dbbind(pDBProc,  2,	INTBIND,	(DBINT) 0, (LPBYTE) &sci->m_emClass);
		dbbind(pDBProc,  3, SMALLBIND,	(DBINT) 0, (LPBYTE) &sci->m_sStats.wDex);
		dbbind(pDBProc,  4, SMALLBIND,	(DBINT) 0, (LPBYTE) &sci->m_sStats.wInt);
		dbbind(pDBProc,  5, SMALLBIND,	(DBINT) 0, (LPBYTE) &sci->m_sStats.wPow);

		dbbind(pDBProc,  6, SMALLBIND,	(DBINT) 0, (LPBYTE) &sci->m_sStats.wStr);
		dbbind(pDBProc,  7, SMALLBIND,	(DBINT) 0, (LPBYTE) &sci->m_sStats.wSpi);
		dbbind(pDBProc,  8, SMALLBIND,	(DBINT) 0, (LPBYTE) &sci->m_sStats.wSta);
		dbbind(pDBProc,  9, INTBIND,	(DBINT) 0, (LPBYTE) &sci->m_wLevel);
		
		// 2003-11-27 : Add
		dbbind(pDBProc, 10, SMALLBIND,	(DBINT) 0, (LPBYTE) &sci->m_wHair);
		dbbind(pDBProc, 11, SMALLBIND,	(DBINT) 0, (LPBYTE) &sci->m_wFace);

		dbnextrow(pDBProc);
		ReleaseConnectionGame(pDB);

		// 착용아이템 정보
		CByteStream ByteStream;
		nRetCode = ReadImage ( "ChaInfo.ChaPutOnItems", nChaNum, ByteStream );
		if (nRetCode == DB_ERROR)
			return DB_ERROR;
		SETPUTONITEMS_BYBUF ( sci->m_PutOnItems,ByteStream );
	
		return DB_OK;
	}
	else
	{
		return DB_ERROR;
	}	
}

// table 의 컬럼이 image, text, ntext 형일때 
// 레코드넣는 시점에 반드시 어떠한 데이터라도 넣어주어야 한다.
// null 일 경우에 timestamp, txtprt 를 얻지 못해서 에러가 나게된다.
// objName 으로 지정된 컬럼에 pData 를 nSize 만큼 넣게된다.
// objName : tablename.columnname
int	CDbmanager::WriteImage(const char* objName, int nChaNum, char* pData, int nSize)
{
	DB_LIST* pDBSelect = GetFreeConnectionGame();
	ASSERT(pDBSelect);
	PDBPROCESS pDBSelectProc = pDBSelect->dbproc; 
	
	RETCODE nRetCode = 0;
	int nTemp = 0;
	
	std::strstream strTemp;
	strTemp << "SELECT " << objName << " FROM ChaInfo where (ChaNum=" <<  nChaNum << ")";
	strTemp << '\0';

	nRetCode = dbcmd(pDBSelectProc, (LPCSTR) strTemp.str());
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : WriteImage dbcmd %d", nChaNum);				
		ReleaseConnectionGame(pDBSelect);
		return DB_ERROR;
	}
		
	nRetCode = dbsqlexec(pDBSelectProc);
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : WriteImage dbsqlexec %d", nChaNum);				
		ReleaseConnectionGame(pDBSelect);
		return DB_ERROR;
	}

	nRetCode = dbresults(pDBSelectProc);
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : WriteImage dbresults %d", nChaNum);						
		ReleaseConnectionGame(pDBSelect);
		return DB_ERROR;
	}
	
	// dbbind(pDBSelectProc, 1, INTBIND, (DBINT) 0, (BYTE *) &nTemp);
	
	DB_LIST* pDBUpdate = GetFreeConnectionGame();
	PDBPROCESS pDBUpdateProc = pDBUpdate->dbproc;

	while (dbnextrow(pDBSelectProc) != NO_MORE_ROWS)
	{
		nRetCode = dbupdatetext(pDBUpdateProc, // pdbproc
								objName, // dest_object
								dbtxptr(pDBSelectProc, 1), // dest_textptr
								dbtxtimestamp(pDBSelectProc, 1), // dest_timestamp
								UT_TEXT, // update_type
								0, // 0, -1 insert_offset
								-1, // 0, -1 delete_length
								NULL, // src_object
								nSize, // src_size
								(LPCBYTE) pData); // src_text

		if (nRetCode == FAIL) 
		{
			CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : WriteImage dbupdatetext %d", nChaNum);								
			ReleaseConnectionGame(pDBUpdate);
			ReleaseConnectionGame(pDBSelect);
			return DB_ERROR;
		}
	}
	ReleaseConnectionGame(pDBUpdate);
	ReleaseConnectionGame(pDBSelect);

	return DB_OK;
}

// objName 해당 컬럼의 이미지데이터를 읽어낸다.
// objName : tablename.columnname
int	CDbmanager::ReadImage ( const char* objName, int nChaNum, CByteStream &ByteStream )
{	
	ByteStream.ClearBuffer ();
	DB_LIST* pDB = GetFreeConnectionGame();
	PDBPROCESS pDBProc = pDB->dbproc; 

	DBINT nInt = 0;
	int nTotalByte = 0;
	RETCODE nRetCode = 0;	
		
	std::strstream strTemp;
	strTemp << "SELECT " << objName << " FROM ChaInfo where (ChaNum=" <<  nChaNum << ")";
	strTemp << '\0';

	if (dbcmd(pDBProc, (LPCSTR) strTemp.str()) == FAIL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : ReadImage dbcmd %d", nChaNum);		
		ReleaseConnectionGame(pDB);	
		return DB_ERROR;;
	}
	if (dbsqlexec(pDBProc) == FAIL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : ReadImage dbsqlexec %d", nChaNum);
		ReleaseConnectionGame(pDB);	
		return DB_ERROR;
	}
	if (dbresults(pDBProc) == FAIL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : ReadImage dbresults %d", nChaNum);
		ReleaseConnectionGame(pDB);	
		return DB_ERROR;
	}	
	char pBuffer[DB_IMAGE_BUF_SIZE];
	while (1)
	{
		nInt = dbreadtext(pDBProc, pBuffer, DB_IMAGE_BUF_SIZE);
		if (nInt == NO_MORE_ROWS || nInt == 0 )	
			break;
		nTotalByte += nInt;
		ByteStream.WriteBuffer ( (LPBYTE)pBuffer, nInt );		
	}
	ReleaseConnectionGame(pDB);

	//	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] 보다 이미지 사이즈가 작다면 버퍼를 리셋.
	if ( nTotalByte < 12 )		ByteStream.ClearBuffer ();

	return nTotalByte;
}

// 사용자 인벤토리에 데이타를 저장한다.
int	CDbmanager::WriteUserInven(int SGNum, // 서버그룹 번호
							   LONGLONG llMoney, // LONGLONG __int64, 저장할 돈의 양
							   DWORD dwUserNum, // 사용자번호
							   char* pData, // 데이타포인터
							   int nSize) // 사이즈

{
	// User Inventory
	// 1. Check User Inven		
	// 2. If exist skip
	// 3. not exist, write iventory image
	BOOL bInven = CheckInven(SGNum, dwUserNum);
	if (!bInven)
	{
		MakeUserInven(SGNum, dwUserNum);
	}

	// Update Money
	DB_LIST* pDBMoney = GetFreeConnectionGame();
	ASSERT(pDBMoney);
	PDBPROCESS pDBMoneyProc = pDBMoney->dbproc; 

	std::strstream strMoney;
	strMoney << "UPDATE UserInven SET UserMoney=" << llMoney ;
	strMoney << "WHERE (UserNum=" <<  dwUserNum << ") AND ";
	strMoney << "(SGNum=" << SGNum << ")";
	strMoney << '\0';

	nRetCode = dbcmd(pDBMoneyProc, (LPCSTR) strMoney.str());
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:WriteUserInven dbcmd %d", dwUserNum);				
		ReleaseConnectionGame(pDBMoney);
		return DB_ERROR;
	}
		
	nRetCode = dbsqlexec(pDBMoneyProc);
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:WriteUserInven dbsqlexec %d", dwUserNum);				
		ReleaseConnectionGame(pDBMoney);
		return DB_ERROR;
	}

	ReleaseConnectionGame(pDBMoney);
	

	// Update Image
	DB_LIST* pDBSelect = GetFreeConnectionGame();
	ASSERT(pDBSelect);
	PDBPROCESS pDBSelectProc = pDBSelect->dbproc; 
	
	RETCODE nRetCode = 0;
	int nTemp = 0;
	
	std::strstream strTemp;
	strTemp << "SELECT UserInven.UserInven FROM UserInven WHERE (UserNum=" <<  dwUserNum << ") AND ";
	strTemp << "(SGNum=" << SGNum << ")";
	strTemp << '\0';

	nRetCode = dbcmd(pDBSelectProc, (LPCSTR) strTemp.str());
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:WriteUserInven dbcmd %d", dwUserNum);				
		ReleaseConnectionGame(pDBSelect);
		return DB_ERROR;
	}
		
	nRetCode = dbsqlexec(pDBSelectProc);
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:WriteUserInven dbsqlexec %d", dwUserNum);				
		ReleaseConnectionGame(pDBSelect);
		return DB_ERROR;
	}

	nRetCode = dbresults(pDBSelectProc);
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:WriteUserInven dbresults %d", dwUserNum);						
		ReleaseConnectionGame(pDBSelect);
		return DB_ERROR;
	}
	
	// dbbind(pDBSelectProc, 1, INTBIND, (DBINT) 0, (BYTE *) &nTemp);
	
	DB_LIST* pDBUpdate = GetFreeConnectionGame();
	PDBPROCESS pDBUpdateProc = pDBUpdate->dbproc;

	CConsoleMessage::GetInstance()->Write("WriteUserInven Size %d", nSize);

	while (dbnextrow(pDBSelectProc) != NO_MORE_ROWS)
	{
		nRetCode = dbupdatetext(pDBUpdateProc, // pdbproc
								"UserInven.UserInven", // dest_object
								dbtxptr(pDBSelectProc, 1), // dest_textptr
								dbtxtimestamp(pDBSelectProc, 1), // dest_timestamp
								UT_TEXT, // update_type
								0, // 0, -1 insert_offset
								-1, // 0, -1 delete_length
								NULL, // src_object
								nSize, // src_size
								(LPCBYTE) pData); // src_text
			
		if (nRetCode == FAIL) 
		{
			CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:WriteUserInven dbupdatetext %d", dwUserNum);
			ReleaseConnectionGame(pDBUpdate);
			ReleaseConnectionGame(pDBSelect);
			return DB_ERROR;
		}
	}
	ReleaseConnectionGame(pDBUpdate);
	ReleaseConnectionGame(pDBSelect);

	return DB_OK;
}

int	CDbmanager::ReadUserInven(SCHARDATA2* pChaData2)
{
	BOOL bInven = CheckInven(CCfg::GetInstance()->GetServerGroup(), pChaData2->m_dwUserID);
	if (!bInven)
	{
		MakeUserInven(CCfg::GetInstance()->GetServerGroup(), pChaData2->m_dwUserID);
	}

	// 유저인벤토리, money
	DB_LIST* pDB = GetFreeConnectionGame();
	PDBPROCESS pDBProc = pDB->dbproc; 

	RETCODE nReturnCode = 0;
	int nUserNum = 0;
	
	// 캐릭터 정보를 가져온다.
	std::strstream strTemp;
	strTemp << "SELECT CAST(UserMoney AS CHAR(15)) FROM UserInven WHERE UserNum=" << pChaData2->m_dwUserID << " AND ";
	strTemp << "SGNum=" << CCfg::GetInstance()->GetServerGroup() ;
	strTemp << '\0';
	
	nReturnCode = dbcmd(pDBProc, (LPCSTR) strTemp.str());
	if (nReturnCode == FAIL)
	{
        CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:ReadUserInven dbcmd");
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}
	// send command buffer to SQL server
	nReturnCode = dbsqlexec (pDBProc); 
	if (nReturnCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:ReadUserInven dbresults");
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}

	nReturnCode = dbresults(pDBProc);
	if (nReturnCode != NO_MORE_RESULTS && nReturnCode != FAIL)
	{
		// Bind data
		char szMoney[15+1];
		dbbind(pDBProc, 1,	NTBSTRINGBIND, (DBINT) 0, (LPBYTE) szMoney);
		dbnextrow(pDBProc); 

		// Money data type convert
		// Character money		
		pChaData2->m_lnStorageMoney = _atoi64(szMoney);
		ReleaseConnectionGame(pDB);
	}
	else
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:ReadUserInven dbresults");
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}	

	// image
	CByteStream ByteStream;
	nRetCode = ReadUserInven(CCfg::GetInstance()->GetServerGroup(), pChaData2->m_dwUserID, ByteStream);
	if (nRetCode == DB_ERROR)
		return DB_ERROR;

	pChaData2->SETSTORAGE_BYBUF(ByteStream);
	return DB_OK;
}

// 사용자 인벤토리에서 데이타를 읽어온다.
int	CDbmanager::ReadUserInven(int SGNum, DWORD dwUserNum, CByteStream &ByteStream)
{	
	ByteStream.ClearBuffer ();
	DB_LIST* pDB = GetFreeConnectionGame();
	PDBPROCESS pDBProc = pDB->dbproc; 

	DBINT nInt = 0;	
	RETCODE nRetCode = 0;	
		
	std::strstream strTemp;
	strTemp << "SELECT UserInven.UserInven FROM UserInven where (UserNum=" <<  dwUserNum << ")";	
	strTemp << '\0';

	if (dbcmd(pDBProc, (LPCSTR) strTemp.str()) == FAIL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:ReadUserInven dbcmd %d", dwUserNum);
		ReleaseConnectionGame(pDB);	
		return DB_ERROR;;
	}
	if (dbsqlexec(pDBProc) == FAIL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:ReadUserInven dbsqlexec %d", dwUserNum);
		ReleaseConnectionGame(pDB);	
		return DB_ERROR;
	}
	
	if (dbresults(pDBProc) == FAIL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:ReadUserInven dbresults %d", dwUserNum);
		ReleaseConnectionGame(pDB);	
		return DB_ERROR;
	}

	char pBuffer[DB_IMAGE_BUF_SIZE];
	
	// LPBYTE pData;
	int nBytes=0;
	int nTotalByte = 0;

	/*
	// Process the results
	while ((nRetCode = dbresults(pDBProc)) != NO_MORE_RESULTS)
	{
		if (nRetCode == FAIL)
		{
			// dbresults() failed
			CConsoleMessage::GetInstance()->Write("dbresults() failed.\n");         
		}
		else
		{              
			while ((nBytes = dbreadtext(pDBProc, (void *) pBuffer, DB_IMAGE_BUF_SIZE)) != NO_MORE_ROWS)
			{
				if (nBytes == -1)
				{                    
					// dbreadtext() failed
					CConsoleMessage::GetInstance()->Write("dbreadtext() failed");
				}
				else if (nBytes == 0)
				{                      
					// We've reached the end of a row.
					CConsoleMessage::GetInstance()->Write("End of Row!");
				}
				else
				{
					// 'bytes' bytes have been placed into our buffer.					
					nTotalByte += nBytes;
					ByteStream.WriteBuffer((LPBYTE) pBuffer, nBytes);
				}
			}
		}
	}
	CConsoleMessage::GetInstance()->Write("UserInven Size %d Bytes", nTotalByte);
	ReleaseConnectionGame(pDB);	
	if ( nTotalByte < 12 )		
		ByteStream.ClearBuffer ();
	return nTotalByte;
	*/
	/*
	while (dbnextrow(pDBProc) != NO_MORE_ROWS)
	{		
		nBytes = dbdatlen(pDBProc, 1);
		CConsoleMessage::GetInstance()->Write("ReadUserInven nBytes %d", nBytes);
		
		if (nBytes == 0)
			break;
		nTotalByte += nBytes;
		CConsoleMessage::GetInstance()->Write("ReadUserInven nTotlaByte %d", nTotalByte);

		pData = ((BYTE*) dbdata(pDBProc, 1));
		ByteStream.WriteBuffer((LPBYTE) pData, nBytes);
	}
	ReleaseConnectionGame(pDB);
	if ( nTotalByte < 12 )		
		ByteStream.ClearBuffer ();

    return nTotalByte;
	*/
	
	while (TRUE)
	{
		// read chunk of text or image data from SQL Server into aBuf
		nBytes = dbreadtext (pDBProc, pBuffer, DB_IMAGE_BUF_SIZE);
		switch (nBytes)
		{
		case 0:
			// end of text or image row
			CConsoleMessage::GetInstance()->Write("End of row");
			break;
		case -1:
			// dbreadtext failed
			CConsoleMessage::GetInstance()->Write("Text or image data retrieval failed");
			break;
		case NO_MORE_ROWS:
			CConsoleMessage::GetInstance()->Write("No more rows");
			break;
		default:
			// dbreadtext has placed lBytes of text or image data
			// into aBuf, now write that chunk to the file
			nTotalByte += nBytes;
			ByteStream.WriteBuffer((LPBYTE) pBuffer, nBytes);			
			break;
		}
		if ((nBytes == -1) || (nBytes == NO_MORE_ROWS))
			break; // while loop
	}
	CConsoleMessage::GetInstance()->Write("UserInven Size %d Bytes", nTotalByte);
	ReleaseConnectionGame(pDB);	
	if ( nTotalByte < 12 )		
		ByteStream.ClearBuffer ();
	return nTotalByte;

	/*
	while (1)
	{
		nInt = dbreadtext(pDBProc, pBuffer, DB_IMAGE_BUF_SIZE);
		if (nInt > 0)
		{
			nTotalByte += nInt;
			ByteStream.WriteBuffer ( (LPBYTE)pBuffer, nInt );
		}
		else if (nInt == 0)
		{
			CConsoleMessage::GetInstance()->Write("UserInven The end of a row");				
		}
		else
		{
			
			
			if (nInt == NO_MORE_ROWS) // NO_MORE_ROWS -2 
				CConsoleMessage::GetInstance()->Write("UserInven All rows read");
			if (nInt == -1)
				CConsoleMessage::GetInstance()->Write("UserInven Read Error");
			break;
		}		
	}
	CConsoleMessage::GetInstance()->Write("UserInven Size %d Bytes", nTotalByte);
	ReleaseConnectionGame(pDB);	
	if ( nTotalByte < 12 )		
		ByteStream.ClearBuffer ();
	return nTotalByte;	
	*/
}

void CDbmanager::AddJob ( CDbAction *pDbActJob )
{
	pDbActJob->SetDbManager ( this );
	CDbExecuter::GetInstance()->AddJob ( pDbActJob );
}

// 유저인벤토리가 있는지 체크한다.
// 없다면 새롭게 생성한다.
bool CDbmanager::CheckInven(int SGNum, DWORD dwUserNum)
{
	DB_LIST* pDB = GetFreeConnectionGame();
	PDBPROCESS pDBProc = pDB->dbproc;

	RETCODE nRetCode = 0;
	int		nInvenNum = 0;

	std::strstream strTemp;
	strTemp << "SELECT count(*) FROM UserInven where (UserNum=" <<  dwUserNum << ") AND ";
	strTemp << "(SGNum=" << SGNum << ")";
	strTemp << '\0';

	if (dbcmd(pDBProc, (LPCSTR) strTemp.str()) == FAIL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:CheckInven dbcmd %d", dwUserNum);
		ReleaseConnectionGame(pDB);	
		return false;
	}
	if (dbsqlexec(pDBProc) == FAIL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:CheckInven dbsqlexec %d", dwUserNum);
		ReleaseConnectionGame(pDB);	
		return false;
	}

	nRetCode = dbresults(pDBProc);
	if (nRetCode != NO_MORE_RESULTS && nRetCode != FAIL)
	{
		// Bind data
		dbbind(pDBProc, 1, INTBIND, (DBINT)0, (BYTE *) &nInvenNum);	
		dbnextrow(pDBProc);
	}

	ReleaseConnectionGame(pDB);

	if (nInvenNum > 0)
	{
		return true;
	}
	else // make new inven
	{
		return false;
	}
}

int	CDbmanager::MakeUserInven(int SGNum, DWORD dwUserNum)
{
	DB_LIST* pDB = GetFreeConnectionGame();
	PDBPROCESS pDBProc = pDB->dbproc;

	RETCODE nRetCode = 0;
	int		nInvenNum = 0;

	std::strstream strTemp;
	strTemp << "INSERT INTO UserInven (SGNum, UserNum, UserMoney, UserInven) VALUES( ";
	strTemp << SGNum << ",";
	strTemp << dwUserNum << ",";
	strTemp << "0,";
	strTemp << "'')";
	strTemp << '\0';

	if (dbcmd(pDBProc, (LPCSTR) strTemp.str()) == FAIL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:MakeUserInven dbcmd %d", dwUserNum);
		ReleaseConnectionGame(pDB);	
		return DB_ERROR;
	}
	if (dbsqlexec(pDBProc) == FAIL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:MakeUserInven dbsqlexec %d", dwUserNum);
		ReleaseConnectionGame(pDB);	
		return DB_ERROR;
	}

	ReleaseConnectionGame(pDB);
	return DB_OK;
}

#include "stdafx.h"

#include "SerialFile.h"

#include "DxBoneCollector.h"
#include "DxEffAni.h"

#include "DxSkinAniMan.h"
#include "StringUtils.h"
#include "StlFunctions.h"

#include "dsutil.h"
#include "DxViewPort.h"
#include "DxSoundMan.h"

BOOL SANIMCONINFO::LoadFile ( char *szFileName, LPDIRECT3DDEVICE8 pd3dDevice )
{
	char szFilePath[MAX_PATH];
	strcpy ( szFilePath, DxSkinAniMan::Instance.GetPath() );
	strcat ( szFilePath, szFileName );

	CSerialFile SFile;
	if ( !SFile.OpenFile ( FOT_READ, szFilePath ) )
	{
		return FALSE;
	}

	char szFileType[FILETYPESIZE];
	DWORD dwVersion;
	SFile.GetFileType ( szFileType, dwVersion );

	if ( 0x0101 == dwVersion )
	{
		SANIMCONINFO_101 OldAnimInfo;
		SFile.ReadBuffer ( &OldAnimInfo, sizeof(OldAnimInfo) );
		
		strcpy ( m_szName, OldAnimInfo.m_szName );
		strcpy ( m_szSkeletion, OldAnimInfo.m_szSkeletion );

        m_dwFlag = OldAnimInfo.m_dwFlag;
		m_dwSTime = OldAnimInfo.m_dwSTime;
		m_dwETime = OldAnimInfo.m_dwETime;
		m_dwETimeOrig = OldAnimInfo.m_dwETimeOrig;
		m_UNITTIME = OldAnimInfo.m_UNITTIME;

		m_MainType = OldAnimInfo.m_MainType;
		m_SubType = OldAnimInfo.m_SubType;

		m_wDivCount = OldAnimInfo.m_wDivCount;
		memcpy ( m_wDivFrame, OldAnimInfo.m_wDivFrame, sizeof(WORD)*ACF_DIV );			
	}
	else if ( 0x0102 == dwVersion )
	{
		SANIMCONINFO_102 OldAnimInfo;
		SFile.ReadBuffer ( &OldAnimInfo, sizeof(OldAnimInfo) );
		
		strcpy ( m_szName, OldAnimInfo.m_szName );
		strcpy ( m_szSkeletion, OldAnimInfo.m_szSkeletion );

        m_dwFlag = OldAnimInfo.m_dwFlag;
		m_dwSTime = OldAnimInfo.m_dwSTime;
		m_dwETime = OldAnimInfo.m_dwETime;
		m_dwETimeOrig = OldAnimInfo.m_dwETimeOrig;
		m_UNITTIME = OldAnimInfo.m_UNITTIME;

		m_MainType = OldAnimInfo.m_MainType;
		m_SubType = OldAnimInfo.m_SubType;

		m_wDivCount = OldAnimInfo.m_wDivCount;
		memcpy ( m_wDivFrame, OldAnimInfo.m_wDivFrame, sizeof(WORD)*ACF_DIV );

		SChaSoundData_102& rChaSoundData_102 = OldAnimInfo.m_ChaSoundData;
		m_ChaSoundData.m_PlayFrameCount = rChaSoundData_102.m_PlayFrameCount;               		
		
		for ( int i = 0; i < rChaSoundData_102.m_PlayFrameCount; i++ )
		{
			strcpy ( m_ChaSoundData.m_szFileName[i], rChaSoundData_102.m_szFileName[i] );
            m_ChaSoundData.m_BufferCount[i] = rChaSoundData_102.m_BufferCount[i];
			m_ChaSoundData.m_bLoop[i] = rChaSoundData_102.m_bLoop[i];			
			m_ChaSoundData.m_PlayType[i] = rChaSoundData_102.m_PlayType[i];
			m_ChaSoundData.m_PlayFrame[i] = rChaSoundData_102.m_PlayFrame[i];
		}
	}
	else if ( 0x0103 == dwVersion )
	{
		SANIMCONINFO_103 OldAnimInfo;
		SFile.ReadBuffer ( &OldAnimInfo, sizeof(OldAnimInfo) );

		strcpy ( m_szName, OldAnimInfo.m_szName );
		strcpy ( m_szSkeletion, OldAnimInfo.m_szSkeletion );

		m_dwFlag = OldAnimInfo.m_dwFlag;
		m_dwSTime = OldAnimInfo.m_dwSTime;
		m_dwETime = OldAnimInfo.m_dwETime;
		m_dwETimeOrig = OldAnimInfo.m_dwETimeOrig;
		m_UNITTIME = OldAnimInfo.m_UNITTIME;

		m_MainType = OldAnimInfo.m_MainType;
		m_SubType = OldAnimInfo.m_SubType;

		m_wDivCount = OldAnimInfo.m_wDivCount;
		memcpy ( m_wDivFrame, OldAnimInfo.m_wDivFrame, sizeof(WORD)*ACF_DIV );

		m_wStrikeCount = 0;

		m_ChaSoundData = OldAnimInfo.m_ChaSoundData;
	}
	else if ( 0x0104 == dwVersion )
	{
		SANIMCONINFO_104 OldAnimInfo;
		SFile.ReadBuffer ( &OldAnimInfo, sizeof(SANIMCONINFO_104) );

		strcpy ( m_szName, OldAnimInfo.m_szName );
		strcpy ( m_szSkeletion, OldAnimInfo.m_szSkeletion );

		m_dwFlag = OldAnimInfo.m_dwFlag;
		m_dwSTime = OldAnimInfo.m_dwSTime;
		m_dwETime = OldAnimInfo.m_dwETime;
		m_dwETimeOrig = OldAnimInfo.m_dwETimeOrig;
		m_UNITTIME = OldAnimInfo.m_UNITTIME;

		m_MainType = OldAnimInfo.m_MainType;
		m_SubType = OldAnimInfo.m_SubType;

		m_wDivCount = OldAnimInfo.m_wDivCount;
		memcpy ( m_wDivFrame, OldAnimInfo.m_wDivFrame, sizeof(WORD)*ACF_DIV );

		m_wStrikeCount = OldAnimInfo.m_wStrikeCount;
		memcpy ( m_sStrikeEff, OldAnimInfo.m_sStrikeEff, sizeof(SANIMSTRIKE)*ACF_STRIKE );

		m_ChaSoundData = OldAnimInfo.m_ChaSoundData;
	}
	else if ( dwVersion==SANIMCONINFO::VERSION )
	{
		//SFile.ReadBuffer ( this, sizeof(SANIMCONINFO) );
		SFile.ReadBuffer ( m_szName, sizeof(char)*ACF_SZNAME );
		SFile.ReadBuffer ( m_szSkeletion, sizeof(char)*ACF_SZNAME );

		SFile >> m_dwFlag;
		SFile >> m_dwSTime;
		SFile >> m_dwETime;
		SFile >> m_dwETimeOrig;
		SFile >> m_UNITTIME;

		DWORD	dwType;
		SFile >> dwType;
		m_MainType = (EMANI_MAINTYPE)dwType;
		SFile >> dwType;
		m_SubType = (EMANI_SUBTYPE)dwType;

		SFile >> m_wDivCount;
		SFile.ReadBuffer ( m_wDivFrame, sizeof(WORD)*ACF_DIV );

		SFile >> m_wStrikeCount;
		SFile.ReadBuffer ( m_sStrikeEff, sizeof(SANIMSTRIKE)*ACF_STRIKE );

		SFile.ReadBuffer ( &m_ChaSoundData, sizeof(SChaSoundData) );

		DWORD	dwSize;
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			DWORD	dwType;
			SFile >> dwType;
			DxEffAniData*	pEff = DxEffAniMan::Instance.CreateEffInstance ( dwType );
			if ( pEff )
			{
				pEff->LoadFile ( SFile, pd3dDevice );
				m_listEffAni.push_back ( pEff );
			}
		}
	}
	else
	{
		MessageBox ( NULL, "SANIMCONINFO::LoadFile() 지원되지않는 버전의 파일입니다.", szFileName, MB_OK );
	}

	return TRUE;
}

BOOL SANIMCONINFO::SaveFile ( char *szFileName )
{
	char szFilePath[1024];
	strcpy ( szFilePath, DxSkinAniMan::Instance.GetPath() );
	strcat ( szFilePath, szFileName );

	CSerialFile SFile;
	SFile.SetFileType ( "SANIMCONINFO", SANIMCONINFO::VERSION );
	if ( !SFile.OpenFile ( FOT_WRITE, szFilePath ) )	return FALSE;

	//SFile.WriteBuffer ( this, sizeof(SANIMCONINFO) );

	SFile.WriteBuffer ( m_szName, sizeof(char)*ACF_SZNAME );
	SFile.WriteBuffer ( m_szSkeletion, sizeof(char)*ACF_SZNAME );

	SFile << m_dwFlag;
	SFile << m_dwSTime;
	SFile << m_dwETime;
	SFile << m_dwETimeOrig;
	SFile << m_UNITTIME;

	SFile << m_MainType;
	SFile << m_SubType;

	SFile << m_wDivCount;
	SFile.WriteBuffer ( m_wDivFrame, sizeof(WORD)*ACF_DIV );

	SFile << m_wStrikeCount;
	SFile.WriteBuffer ( m_sStrikeEff, sizeof(SANIMSTRIKE)*ACF_STRIKE );

	SFile.WriteBuffer ( &m_ChaSoundData, sizeof(SChaSoundData) );

	SFile << (DWORD) m_listEffAni.size();
	EFFANILIST_ITER iter = m_listEffAni.begin();
	EFFANILIST_ITER iter_end = m_listEffAni.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SFile << (DWORD) (*iter)->GetTypeID();
		(*iter)->SaveFile ( SFile );
	}

	return TRUE;
}




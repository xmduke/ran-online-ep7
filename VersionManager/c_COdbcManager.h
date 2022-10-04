#pragma once

#ifndef _C_COdbcManager_H_
#define _C_COdbcManager_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Auto Link Library Files
// To use the installer DLL functions, you must: 
// Use #include < ODBCINST.H>.
// Link with Odbcinst.lib for 16-bit applications.
// Link with Odbccp32.lib for 32-bit applications. 
// #pragma comment (lib,"Odbccp32.lib")
#pragma comment (lib,"odbc32.lib")
#include <odbcinst.h>
#include <sqlext.h> // ODBC library

#include <Windows.h>
#include <stdio.h>
#include <vector>

#include "DatabaseTable.h"

// Seconds of database response time
#define DB_RESPONSE_TIME	30
#define DB_CONNECTION_COUNT 10

#define DB_ERROR			-1
#define DB_OK				0

#define DB_USE				1
#define DB_NOT_USE			0

#define DB_CHA_MAX			-2

// string data size
#define USR_ID_LENGTH				20
#define CHR_ID_LENGTH				33
#define USR_PASS_LENGTH				20

#define CHAT_MSG_SIZE				100
#define GLOBAL_CHAT_MSG_SIZE		981
#define DB_SVR_NAME_LENGTH			50
#define DB_NAME_LENGTH				50
#define SERVER_NAME_LENGTH			50
#define MAP_NAME_LENGTH				50

// #ifndef MAX_SERVERCHAR_NUM
//	#define MAX_SERVERCHAR_NUM		2 // 전체 서버에서 만들수 있는 캐릭터 갯수
// #endif

#define MAX_ONESERVERCHAR_NUM		16 // 한 서버에서 만들수 있는 캐릭터 갯수

class COdbcManager
{
public:
	static COdbcManager* GetInstance();	
	static void	ReleaseInstance();

private:
	COdbcManager(void);
	~COdbcManager(void);

	static COdbcManager* SelfInstance;

protected:
   	SQLHENV hEnvVer;  // Handle of sql (void*)	
	SQLHDBC hOdbcVer; // Handle of ODBC (void*)	

	char m_szVerOdbcName[DB_SVR_NAME_LENGTH+1];
	char m_szVerUsrID[USR_ID_LENGTH+1];	
	char m_szVerUsrPass[USR_PASS_LENGTH+1];
	char m_szVerDBName[DB_NAME_LENGTH+1];	
	int	m_nVerDBTimeOut;

public:	
	void SetVerDB(
			const char* szOdbcName,
			const char* szUsrID,
			const char* szUsrPass,
			const char* szDBName );

	void SetVerOdbcName(
			const char* szName );

	void SetVerUserID( const char* szUsrID );

	void SetVerUserPass( const char* szUsrPasswd );

	void SetVerDBName( const char* szDBName );

	void SetVerDBServer(
			const char* szName,
			const char* szUsrID,
			const char* szUsrPass,
			const char* szDBName);

	int OpenVerDB();
	
	int	OpenVerDB(
			char* szOdbcName, 
			char* szUsrID, 
			char* szUsrPasswd,
			char* szDbName );

	void CloseVerDB();

	// Get Folder List
	int	GetFolderList(
			std::vector<FolderList> &v );

	// Get File List
	int	GetFileList(
		int cd_idx,
		std::vector<FileList> &v );

    int GetAllFileList(
		std::vector<FullFileList> &v );

    int GetNotUploadedFileList(
		std::vector<FullFileList> &v );

    int UpdateFileStateTrue(
		int nIdx );

    int UpdateFileStateFalse(
		const CString& strFileName );

	int CreateFolder(
		const CString& strDir );

	int	RemoveFolder(
		int nDirNum );
	
    CString	GetFolderName(
				int nDirNum );

    int GetFolderNum( const CString& strName );

    int GetFileFolderNum( const CString& strFileName );

	int InsertFile(
		int nDirNum,
		const CString& strFileName,
		const CString& strMD5 );

	// 파일이 변경되었을때 변경된 파일의
	// 버전정보와 md5 값을 갱신한다.
	int UpdateFile(
		const CString& strFileName,
		const CString& strMD5 );

    int DeleteFile( int nFileNum );
	
    BOOL IsFileExist( const CString& strFileName );

    BOOL IsFolderExist( const CString& strFolderName );

	int ExecuteSQL( const CString& strSQL );

	int Insert( const CString& strSQL );
	
	int Update( const CString& strSQL );

	int	Delete( const CString& strSQL );

	void msgBox( const CString& strMsg );
};

#endif _C_COdbcManager_H_
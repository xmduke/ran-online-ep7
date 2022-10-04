#include "StdAfx.h"
#include "./AutoPatchThread.h"
#include "./Resource.h"

#include "./LogControl.h"
#include "../netclientlib/s_CHttpPatch.h"
#include "../mfcexlib/RANPARAM.h"
#include "../MfcExLib/ExLibs/CompactFdi.h"
#include "../EngineLib/Common/StringUtils.h"
#include "LauncherText.h"
#include "../Common/SUBPATH.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CAutoPatchThread, CHttpThread)

CAutoPatchThread::CAutoPatchThread( LPVOID pParam, DWORD nThreadID )
{
	VERIFY( pParam != NULL);
	m_pPatchThreadParam = (S_PATCH_THREAD_PARAM *)pParam;

	m_nDlgThreadID = nThreadID;
	m_sFailGameVer = 0;
}

void CAutoPatchThread::ThreadMain()
{
	CHttpPatch* pHttpPatch	= m_pPatchThreadParam->pHttpPatch;
	const bool	bUseHttp	= m_pPatchThreadParam->bUseHttp;
	const int	cPatchVer	= m_pPatchThreadParam->cPatchVer;
	const int	sPatchVer	= m_pPatchThreadParam->sPatchVer;
	const int	cGameVer	= m_pPatchThreadParam->cGameVer;
	const int	sGameVer	= m_pPatchThreadParam->sGameVer;	

	CString cFileList, sFileList, str, strTemp;
	BOOL bInstalling(FALSE);

	NS_LOG_CONTROL::SetProcessAllPosition ( 0, 100 );

	if ( IsForceTerminate() ) return ;


	// 리스트 전송중
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 49 ), 0 );
	// 파일 리스트 다운로드
	if ( !GETFILE_USEHTTP ( pHttpPatch, "\\", NS_GLOBAL_VAR::strServerCabFileList.GetString(), "" ) )
	{	
		if ( !IsForceTerminate() )
		{
			// 리스트 전송중 오류
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 50 ), 0 );
		}

		goto LFail;
	}

	

	if ( IsForceTerminate() ) goto LFail;

	//	압축 풀기위한 전체 경로 설정 및 압축 풀기&설치
	str = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strServerCabFileList;

	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 51 ), 0 );
	if ( !Extract ( NS_GLOBAL_VAR::strAppPath.GetString (), str.GetString() ) )
	{
		// 리스트 압축풀기 실패
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 52 ), 0 );
		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 4, 100 );

	//Initialize(); // MEMO : 함수 구현 안됨.

	if ( IsForceTerminate() ) goto LFail;

	// 파일 리스트 비교 및 새 목록 작성
	// 패치 파일 리스트 로드
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 53 ), 0 );
	if ( !LoadList() )
	{
		// 리스트 로드에 실패
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 54 ), 0 );
		goto LFail;
	}

	if ( IsForceTerminate() ) goto LFail;

	// 패치 파일 리스트 비교 진행
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 57 ), 0 );
	if ( !MakeNewList ( cPatchVer, sPatchVer, cGameVer, sGameVer ) )
	{
		// 리스트 비교에 실패
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 58 ), 0 );
		goto LFail;
	}    

	NS_LOG_CONTROL::SetProcessAllPosition ( 10, 100 );

	if ( IsForceTerminate() ) goto LFail;

	//	새 목록에 나온 파일 다운 로드
	str = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strDownloadTemp;
	CreateDirectory ( str.GetString(), NULL );
	if ( !DownloadFilesByHttp ( pHttpPatch ) )
	{
		if ( !IsForceTerminate() )
		{
			// 패치 파일 전송중 오류
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 55 ), 0 );
		}

		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 70, 100 );

	if ( IsForceTerminate() ) goto LFail;

	//	파일 복사
	if ( !Installing() )
	{	
		//	강제 종료가 아닌 진짜 오류
		//
		if ( IsExtractError() )
		{
			// 패치 파일중 일부 손상
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 59 ), 0 );

			//	Note : DS list 삭제
			//
			CString strDownList = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strDownList;
			DeleteFile ( strDownList.GetString() );

			// MEMO : 카피 리스트로 비교후에 설치된 파일은 다운로드 받지 않아도 된다.
			//
			CString strCopyList = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT  + NS_GLOBAL_VAR::strCopyList;
			CopyFile( strCopyList.GetString(), strDownList.GetString(), FALSE );
		}

		bInstalling = TRUE; // 설치중이다.

		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 80, 100 );

//	Package
//	if(!CompressFile()){
//		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 71 ), 0 );
//		goto LFail;
//	}

	NS_LOG_CONTROL::SetProcessAllPosition ( 90, 100 );

	//	Note : DS list 삭제
	//	
	strTemp = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT  + NS_GLOBAL_VAR::strDownList;
	DeleteFile( strTemp.GetString() );

	// Note : 카피 리스트 삭제
	//
	strTemp = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT  + NS_GLOBAL_VAR::strCopyList;
	DeleteFile ( strTemp.GetString() );

	//	<--	클라이언트 목록를 서버목록으로 교체	-->	//
	cFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szClientFileList;
	sFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szServerFileList;

	DeleteFile( cFileList.GetString () );
	MoveFile( sFileList.GetString(), cFileList.GetString() );

	sFileList = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strServerCabFileList;
	DeleteFile ( sFileList.GetString() );

	DeleteDownFiles();
	DeleteNotFoundFile();
//	DeleteCompressFile();
//	if(!OptimizeCompressFile()){
//		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 72 ), 0 );
//		goto LFail;
//	}

    
	Destroy ();	

	// 클라이언트 버전 갱신
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 60 ), 0 );
	if ( !VersionUp ( sGameVer ) )
	{
		// 클라이언트 버전 갱신 실패
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 61 ), 0 );
		goto LFail;
	}

	// 모든 패치 성공
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 62 ), 0 );
	NS_LOG_CONTROL::SetProcessAllPosition ( 100, 100 );

	return ;

LFail:
	//	다운로드할 파일들이 손상되어 실패한 경우에는
	//	리스트를 처음부터 다시 작성해야하기때문에 리스트를 저장하지 않는다.
	if ( IsExtractError () )
	{
		if ( IsForceTerminate () )
		{
			SaveDownList ( sGameVer );
		}

		SaveCopyList ( sGameVer ); // MEMO :설치한 리스트는 설치중에 취소되었을때는 무조건 저장한다.
	}
	else
	{
		SaveDownList ( sGameVer );

		if( bInstalling ) SaveCopyList ( sGameVer ); // MEMO :설치중에 중단되면 카피 리스트 작성.
	}

	sFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szServerFileList;
	DeleteFile ( sFileList.GetString() );
	sFileList = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strServerCabFileList;
	DeleteFile ( sFileList.GetString() );	

	Destroy ();

	SetFail();

	return ;
}

BOOL CAutoPatchThread::SaveDownList( int sGameVer )
{
	CString strTemp;
	strTemp = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strDownList;

	FILE* fp = fopen ( strTemp, "wb" );
	if ( !fp )
	{
		return FALSE;
	}

	SFILENODE FileInfo;

	FileInfo.Ver = sGameVer;
	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		CDebugSet::ToLogFile( "[ERROR]SaveDownList File Game Version Write Failed!" );
		return FALSE;
	}

	FileInfo.Ver = (int) m_vectorDownFile.size ();
	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		CDebugSet::ToLogFile( "[ERROR]SaveDownList File Size Write Failed!" );
		return FALSE;
	}

	for ( int i = 0; i < (int)m_vectorDownFile.size (); i++ )
	{
		SFILENODE* pNewFile = m_vectorDownFile[i];

		if ( 1 != fwrite ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
		{
			fclose ( fp );

			CDebugSet::ToLogFile( "[ERROR]SaveDownList File Write Failed!" );
			return FALSE;
		}
	}

	fclose ( fp );
	
	return TRUE;
}

BOOL CAutoPatchThread::LoadDownList()
{
	CString strTemp;
	strTemp = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strDownList;

	FILE* fp = fopen ( strTemp, "rb" );
	if ( !fp )
	{
		return FALSE;
	}

	SFILENODE FileInfo;

	if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		CDebugSet::ToLogFile( "[ERROR]LoadDownList File Game Version Read Failed!" );
		return FALSE;
	}
	m_sFailGameVer = FileInfo.Ver;

	if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		CDebugSet::ToLogFile( "[ERROR]LoadDownList File Size Read Failed!" );
		return FALSE;
	}	

	for ( int i = 0; i < FileInfo.Ver; i++ )
	{
		SFILENODE* pNewFile = new SFILENODE;
		if ( 1 != fread ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
		{
			fclose ( fp );

			CDebugSet::ToLogFile( "[ERROR]LoadDownList File Read Failed!" );
			return FALSE;
		}

		m_mapDownedFile.insert ( std::make_pair(std::string(pNewFile->FileName), pNewFile) );
	}

	fclose ( fp );

	return TRUE;
}

BOOL CAutoPatchThread::SaveCopyList( int sGameVer )
{
	CString strTemp;
	strTemp = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strCopyList;

	FILE* fp = fopen ( strTemp, "wb" );
	if ( !fp )
	{
		return FALSE;
	}

	SFILENODE FileInfo;

	FileInfo.Ver = sGameVer;
	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		CDebugSet::ToLogFile( "[ERROR]SaveCopyList File Game Version Write Failed!" );
		return FALSE;
	}

	FileInfo.Ver = (int) m_vectorCopyFile.size ();
	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		CDebugSet::ToLogFile( "[ERROR]SaveCopyList File Size Write Failed!" );
		return FALSE;
	}

	for ( int i = 0; i < (int)m_vectorCopyFile.size (); i++ )
	{
		SFILENODE* pNewFile = m_vectorCopyFile[i];

		if ( 1 != fwrite ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
		{
			fclose ( fp );

			CDebugSet::ToLogFile( "[ERROR]SaveCopyList File Write Failed!" );
			return FALSE;
		}
	}

	fclose ( fp );

	return TRUE;
}

BOOL CAutoPatchThread::LoadCopyList()
{
	CString strTemp;
	strTemp = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strCopyList;

	FILE* fp = fopen ( strTemp, "rb" );
	if ( !fp )
	{
		return FALSE;
	}

	SFILENODE FileInfo;

	if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		CDebugSet::ToLogFile( "[ERROR]LoadCopyList File Game Version Read Failed!" );
		return FALSE;
	}
	m_sFailGameVer = FileInfo.Ver;

	if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		CDebugSet::ToLogFile( "[ERROR]LoadCopyList File Size Read Failed!" );
		return FALSE;
	}	

	for ( int i = 0; i < FileInfo.Ver; i++ )
	{
		SFILENODE* pNewFile = new SFILENODE;
		if ( 1 != fread ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
		{
			fclose ( fp );

			CDebugSet::ToLogFile( "[ERROR]LoadCopyList File Read Failed!" );
			return FALSE;
		}

		m_mapCopiedFile.insert ( std::make_pair(std::string(pNewFile->FileName), pNewFile) );
/*
		if(strstr(pNewFile->SubPath,NS_GLOBAL_VAR::strCompDir)){ // 압축할 파일인지 패스를 검색해서 알아본다.
			m_vecCompFile.push_back(pNewFile);
		}
*/
	}

	fclose ( fp );

	return TRUE;
}

BOOL CAutoPatchThread::MakeNewList( const int cPatchVer, const int sPatchVer, const int cGameVer, const int sGameVer )
{
	NS_LOG_CONTROL::SetProcessCurPosition ( 0, 1 );
	{
		FILEVECTOR::size_type nServerFileSize = m_vectorServerFile.size ();
		for ( FILEVECTOR::size_type i = 0; i < nServerFileSize; i++ )
		{	
			SFILENODE* pServerGetFile = m_vectorServerFile[i];

			CPatchSetNode* pNode = m_ClientFileTree.Find ( pServerGetFile );
			if ( pNode )	//	기존 파일
			{
				SFILENODE* pClientFile = pNode->GetFile ();
				pNode->SetUseFlag ();

				if ( pClientFile->Ver < pServerGetFile->Ver )
				{
					SFILENODE* pNewFile = new SFILENODE;
					*pNewFile = *pServerGetFile;
					m_vectorNewFile.push_back ( pNewFile );
				}
			}
			else	//	추가되는 파일
			{			
				SFILENODE* pNewFile = new SFILENODE;
				*pNewFile = *pServerGetFile;
				m_vectorNewFile.push_back ( pNewFile );
			}

			NS_LOG_CONTROL::SetProcessCurPosition ( i, nServerFileSize );

			if ( IsForceTerminate() )	return FALSE;
		}
	}
	NS_LOG_CONTROL::SetProcessCurPosition ( 1, 1 );

	//	<--	서버에 없는 파일이 클라이언트에 존재할 경우 삭제 -->	//
	m_ClientFileTree.GetNotUseItem ( &m_vectorDeleteFile );

	LoadDownList ();	

	return TRUE;
}

BOOL CAutoPatchThread::DownloadFilesByHttp( CHttpPatch* pHttpPatch )
{
	int DownCount = 0;

	// Temp 폴더패스를 설정한다.
	//
	CString	DownloadDir( NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strDownloadTemp );

	int nVectorNewFileSize = (int)m_vectorNewFile.size();
	for ( int i = 0; i < nVectorNewFileSize; ++i )
	{
		SFILENODE* pNewFile = m_vectorNewFile[i];		

		FILEMAP_ITER found = m_mapDownedFile.find( std::string ( pNewFile->FileName ) );
		FILEMAP_ITER iter_end = m_mapDownedFile.end();

		BOOL bAlreadyDown( FALSE );
		if ( found != iter_end ) bAlreadyDown = TRUE; // Note : 이미 정상적으로 다운로드 완료된 것.

		if ( IsForceTerminate() ) return FALSE;


		//	<--	카운트는 현재 진행하고 있는것을 포함한다.	-->	//
		//	현재 첫번째것을 받고 있으면, 카운트는 1이 되는 것이다.
		//	완료 기준이 아니라, 진행 기준이다.			
		if ( !bAlreadyDown ) // MEMO : 이미 다운받은 파일은 리스트에 표시하지 않는다.
		{
			CString	strMsg;
			strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 47 ); 
			// 동기화 문제 때문에 메세지를 보낼때 항상 메모리를 새로 생성한다.
			CHAR * szListMessage = new CHAR[256]; 
			wsprintf( szListMessage, "%s %s", strMsg.GetString(), pNewFile->FileName );
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );
		}

		ULONGLONG TotalPos = 10 + (++DownCount * 60) / nVectorNewFileSize;			
		NS_LOG_CONTROL::SetProcessAllPosition( TotalPos, 100 );

		CString FullSubPath = pNewFile->SubPath;

		if ( !bAlreadyDown ) //	Note : 아직 다운로드 안된 것만 GetFile한다.
		{
			if ( !GETFILE_USEHTTP ( pHttpPatch, FullSubPath.GetString(), pNewFile->FileName ) )
			{
				return FALSE;
			}
		}

		//	<--	정상적으로 다운로드 했으면 리스트 작성한다.	--> //
		m_vectorDownFile.push_back( pNewFile );

		if( IsForceTerminate() ) return FALSE;

		Sleep( 0 ); // 파일받다가 멈추는것 방지용
	}

	if ( m_vectorNewFile.size() != m_vectorDownFile.size () )
	{
		return FALSE; // 다운받은 파일과 목록에 있는 파일의 개수가 다르다면 실패
	}

	LoadCopyList (); // 설치했던 리스트를 로딩한다.

	return TRUE;
}

BOOL CAutoPatchThread::Installing()
{
	CString strTemp;
	int InstallCount = 0;
	int nMaxPos = 0;

	int nVectorDownFileSize = (int)m_vectorDownFile.size();
	for ( int i = 0; i < nVectorDownFileSize; ++i )
	{
		SFILENODE* pNewFile = m_vectorDownFile[i];	

		//	Note : 이미 정상적으로 설치 완료된 것.
		FILEMAP_ITER found = m_mapCopiedFile.find ( std::string ( pNewFile->FileName ) );
		FILEMAP_ITER iter_end = m_mapCopiedFile.end ();

		BOOL bAlreadyCopy(FALSE);
		if ( found != iter_end ) bAlreadyCopy = TRUE;

		if ( IsForceTerminate() ) return FALSE;

		CString	Seperator = "\\";
		CString FullSubPath = pNewFile->SubPath;
		CStringArray SubPathArray;

		STRUTIL::ClearSeparator ();
		STRUTIL::RegisterSeparator ( Seperator );
		STRUTIL::StringSeparate ( FullSubPath, SubPathArray );		

		//	<--	없는 디렉토리 만들기	-->	//
		CString FullPath;
		FullPath.Format( "%s", NS_GLOBAL_VAR::strAppPath.GetString() );
		for ( int i = 0; i < SubPathArray.GetCount (); i++ )
		{
			FullPath += SubPathArray[i];			
			CreateDirectory ( FullPath.GetString(), NULL );
			FullPath += "\\";
		}

		//	<--	압축 풀기위한 전체 경로 설정 및 압축 풀기&설치	-->	//
		NS_LOG_CONTROL::SetProcessCurPosition ( 0, 1 );
		{
			if ( !bAlreadyCopy )
			{
				CString	strMsg;
				strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 48 );
				CHAR * szListMessage = new CHAR[256];
				wsprintf( szListMessage, "%s %s", strMsg.GetString(), pNewFile->FileName );
				::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );
			}

			ULONGLONG TotalPos = 70 + (++InstallCount * 20) / nVectorDownFileSize;
			NS_LOG_CONTROL::SetProcessAllPosition ( TotalPos, 100 );

			if ( !bAlreadyCopy )
			{
				CString	DownloadDir;
				DownloadDir = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strDownloadTemp + pNewFile->FileName;		

				CString	DestFile;
				DestFile.Format( "%s%s", FullPath.GetString(), pNewFile->FileName );
				DestFile = DestFile.Left ( DestFile.ReverseFind ( '.' ) );

				SetFileAttributes( DestFile.GetString(), FILE_ATTRIBUTE_NORMAL );
				DeleteFile( DestFile.GetString() );

				if( !Extract ( FullPath.GetString(), DownloadDir.GetString() ) )
				{
					DownloadDir.MakeLower();
					DownloadDir = DownloadDir.Left( DownloadDir.ReverseFind( '.' ) );

					if( ( DownloadDir.Find( _T(".dll") ) == -1 ) && ( !IsForceTerminate() ) )
					{
						CHAR * szListMessage = new CHAR[MAX_PATH];
						StringCchCopy ( szListMessage, MAX_PATH, GetErrorMsg() );
						::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );

						SetExtractError (); // 오류메시지 : 오류일때 계속 시도한다. 무한루프 가능 ㅡ.ㅡ;;

						return FALSE;
					}
				}

				

				SetFileAttributes ( DestFile.GetString(), FILE_ATTRIBUTE_NORMAL );
			}
		}
		NS_LOG_CONTROL::SetProcessCurPosition ( 1, 1 );

//		if(strstr(pNewFile->SubPath,NS_GLOBAL_VAR::strCompDir)){ // 압축할 파일인지 패스를 검색해서 알아본다.
//			m_vecCompFile.push_back(pNewFile);
//		}

		//	<--	정상적으로 설치했으면 리스트 작성한다. --> //
		m_vectorCopyFile.push_back ( pNewFile );

		if( IsForceTerminate() ) return FALSE;

		Sleep( 0 );
	}

	if ( m_vectorNewFile.size() != m_vectorCopyFile.size () )
	{
		return FALSE; // MEMO : 업데이트할 파일 개수와 설치한 파일 개수가 일치하지 않으면...
	}

	return TRUE;
}

BOOL CAutoPatchThread::DeleteNotFoundFile()
{
	CString	str, strCompFile;
	int nCnt = static_cast<int>( m_vectorDeleteFile.size() );

	strCompFile = NS_GLOBAL_VAR::strAppPath + RANPARAM::RPFDataPath;

	m_FileSystem.OpenFileSystem(strCompFile);	


	for ( int i = 0; i < nCnt; i++ )
	{
		SFILENODE* pNewFile = m_vectorDeleteFile[i];

/*
		if(strstr(pNewFile->SubPath,NS_GLOBAL_VAR::strCompDir)){
			CString strSubPath, strSubDir;
			
			strSubPath = pNewFile->SubPath;
			strSubDir = strSubPath.Right( strSubPath.GetLength() - (int)strlen("\\package") ); // \\package 파일 뺌
			
			m_FileSystem.ChangeDir(strSubDir);
			
			str.Format( "%s",pNewFile->FileName );
			str.MakeLower();
			str = str.Left( str.ReverseFind ( '.' ) );
			
			m_FileSystem.Remove(pNewFile->FileName);			
			
			continue;
		}
*/
		str.Format( "%s%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), pNewFile->SubPath, pNewFile->FileName );
		str.MakeLower();
		str = str.Left( str.ReverseFind ( '.' ) );

		DeleteFile( str.GetString() );
	}

	m_FileSystem.CloseFileSystem();

	return TRUE;
}

BOOL CAutoPatchThread::DeleteDownFiles()
{
	CString	strTemp;
	int nCnt = static_cast<int>( m_vectorDownFile.size() );

	for ( int i = 0; i < nCnt; ++i )
	{
		SFILENODE* pNewFile = m_vectorDownFile[i];
		strTemp.Format( "%s%s%s%s", NS_GLOBAL_VAR::strProFile.GetString(), SUBPATH::SAVE_ROOT, 
						            NS_GLOBAL_VAR::strDownloadTemp.GetString() , pNewFile->FileName );

		DeleteFile( strTemp.GetString() );
	}

	strTemp = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strDownloadTemp;
	RemoveDirectory ( strTemp.GetString() );

	return TRUE;
}


/*
BOOL CAutoPatchThread::DeleteCompressFile()
{
	CString	strTemp;
	int nCnt = static_cast<int>( m_vecCompFile.size() );

	for ( int i = 0; i < nCnt; ++i )
	{
		SFILENODE* pNewFile = m_vecCompFile[i];
		strTemp.Format( "%s%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), pNewFile->SubPath, pNewFile->FileName );
		strTemp.MakeLower();
		strTemp = strTemp.Left( strTemp.ReverseFind ( '.' ) );

		DeleteFile( strTemp.GetString() );
	}

	// directory 삭제 하는 부분
	CString strDir[5];
	strDir[0].Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(),"\\package\\data\\glogic\\level");
	strDir[1].Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(),"\\package\\data\\glogic\\npctalk");
	strDir[2].Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(),"\\package\\data\\glogic\\quest");
	strDir[3].Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(),"\\package\\data\\glogic");
	strDir[4].Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(),"\\package\\data");

	for(int i = 0; i < 5 ; ++i){
		RemoveDirectory(strDir[i].GetString());			
	}

	return TRUE;

}
*/


BOOL CAutoPatchThread::Destroy()
{
	for ( int i = 0; i < (int)m_vectorServerFile.size (); i++ )
	{
		delete m_vectorServerFile[i];
	}
	m_vectorServerFile.clear();

	for ( int i = 0; i < (int)m_vectorNewFile.size (); i++ )
	{
		delete m_vectorNewFile[i];
	}
	m_vectorNewFile.clear();
	m_vectorDownFile.clear(); // MEMO : 중복된 포인터라서 delete 할 필요없음.

	// m_mapDownedFile 삭제 ///////////////////////////
	FILEMAP_ITER iter = m_mapDownedFile.begin ();
	FILEMAP_ITER iter_end = m_mapDownedFile.end ();
	for ( ; iter != iter_end; iter++ )
	{
		delete (*iter).second;
	}
	m_mapDownedFile.clear();
	m_vectorCopyFile.clear();

	// m_mapDownedFile 삭제 ///////////////////////////
	iter = m_mapCopiedFile.begin ();
	iter_end = m_mapCopiedFile.end ();
	for ( ; iter != iter_end; iter++ )
	{
		delete (*iter).second;
	}
	m_mapCopiedFile.clear();
	m_vectorDeleteFile.clear();
//	m_vecCompFile.clear();

	return TRUE;
}

BOOL CAutoPatchThread::VersionUp( int sGameVer )
{
	CString strTemp;
	strTemp.Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), g_szClientVerFile );
	SetFileAttributes( strTemp.GetString(), FILE_ATTRIBUTE_NORMAL );

	FILE* cfp = fopen( strTemp.GetString(), "rb+" );	
	if ( cfp )
	{		
		fseek( cfp, sizeof ( int ) * 1, SEEK_SET );
		if ( 1 != fwrite( &sGameVer, sizeof( int ), 1, cfp ) )
		{
			fclose( cfp );
			return FALSE;
		}
		fclose( cfp );
	}

	return TRUE;
}



/*
BOOL CAutoPatchThread::CompressFile()
{
	CString strCompPath;
	strCompPath = NS_GLOBAL_VAR::strAppPath + RANPARAM::RPFDataPath;

	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 70 ), 0 );

	if(!m_FileSystem.OpenFileSystem(strCompPath)){ // 파일 자체가 없을경우
		m_FileSystem.NewFileSystem(strCompPath);
		m_FileSystem.OpenFileSystem(strCompPath);
	}

    TCHAR strFileName[256]; // 파일 이름
	CString strFilePath; // 파일 전체경로
	CString strSubDir; // 파일시스템 subpath
	CString strSubPath; // 원래 subpath
	CString strTemp;
	for ( int i = 0; i < (int)m_vecCompFile.size (); i++ )
	{
		strSubPath = m_vecCompFile[i]->SubPath;
		strSubDir = strSubPath.Right( strSubPath.GetLength() - (int)strlen("\\package") ); // \\package 이부분 삭제
		if(!m_FileSystem.ChangeDir(strSubDir)){ // 폴더가 생성되지 않았을 경우 폴더 생성
			m_FileSystem.ChangeDir(m_FileSystem.GetCurDir());

			int count = 0;

			if(strcmp(m_FileSystem.GetCurDir(),"/") == 0){ // 여기두 이상 
				strTemp = strSubDir;	
			}else{
				count =(int) ( strSubDir.GetLength() - strlen(m_FileSystem.GetCurDir()));
				strTemp = strSubDir.Right(count); 
			}
			
			CString token;
			
			int curPos = 0;
			token = strTemp.Tokenize("\\",curPos);
			while(token != ""){
				if(!m_FileSystem.AddDir(token))	return FALSE;
				if(!m_FileSystem.ChangeDir(token)) return FALSE;
				token= strTemp.Tokenize("\\",curPos);
			}			
			
		}

		strFilePath.Format("%s%s%s",NS_GLOBAL_VAR::strAppPath.GetString(),m_vecCompFile[i]->SubPath,m_vecCompFile[i]->FileName);
		strFilePath = strFilePath.Left(strFilePath.ReverseFind('.'));
		m_FileSystem.GetNameOnly(strFileName,strFilePath);
		if(m_FileSystem.CheckNameExist((LPCTSTR)strFileName)) // 같은 파일이 있는지 확인
		{
			m_FileSystem.Remove(strFileName);
		}

		if(!m_FileSystem.AddFile(LPCTSTR(strFilePath))) // 파일 추가
		{
			return FALSE;
		}
		if(!m_FileSystem.ChangeDir("/"))  return FALSE;
	}

	m_FileSystem.CloseFileSystem();
	return TRUE;
}
*/
/*
BOOL CAutoPatchThread::OptimizeCompressFile()
{

	CString strCompPath, strPath;
	strCompPath = NS_GLOBAL_VAR::strAppPath + RANPARAM::RPFDataPath;
	strPath = NS_GLOBAL_VAR::strAppPath + "package\\temp.rpf";

	m_FileSystem.OpenFileSystem(strCompPath);

	if(!m_FileSystem.OptimizeFileSystem(strPath)){
		m_FileSystem.CloseFileSystem();
		return FALSE;
	}

	m_FileSystem.CloseFileSystem();
	remove(strCompPath);
	rename(strPath,strCompPath);

	return TRUE;
}
*/

BEGIN_MESSAGE_MAP(CAutoPatchThread, CHttpThread)
END_MESSAGE_MAP()
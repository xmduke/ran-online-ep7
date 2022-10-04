#include "StdAfx.h"
#include ".\httppatchthread.h"
#include "Resource.h"

#include "LogControl.h"
#include "StringUtils.h"
#include "CompactFdi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CHttpPatchThread::LoadList()
{
	//	<--	리스트 읽기	-->	//
	CString str;
	FILE* fp;

	str.Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), NS_GLOBAL_VAR::g_szServerFileList );
	fp = fopen( str.GetString(), "rb" );
	if ( !fp )
	{
		return FALSE;
	}

	SFILENODE FileInfo;
	//	NOTE
	//		첫번째 버전 정보 [아직은 아무런 일도 하지 않음]
	if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		return FALSE;
	}

	//	NOTE
	//		두번째 파일 개수
	if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		return FALSE;
	}	

	CString strTemp;

	for ( int i = 0; i < FileInfo.Ver; i++ )
	{
		SFILENODE* pNewFile = new SFILENODE;
		if ( 1 != fread ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
		{
			return FALSE;
		}

		strTemp = pNewFile->FileName;
		strTemp = strTemp.Left( strTemp.ReverseFind( '.' ) );
		strTemp.MakeUpper();

		if( strTemp.Find( ".DLL" ) != -1 )
		{
			//CDebugSet::ToLogFile( "서버 파일 리스트에 [%s]이 있습니다.", pNewFile->FileName );
			m_vectorServerFile.push_back( pNewFile );
		}
		else
		{
			m_vectorServerFile.push_back( pNewFile );
		}
	}
	fclose ( fp );

	if( IsForceTerminate() ) return FALSE;

	str.Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), NS_GLOBAL_VAR::g_szClientFileList );
	fp = fopen( str.GetString(), "rb" );
	if ( !fp )
	{
		return FALSE;
	}

	//	NOTE
	//		첫번째 버전 정보 [아직은 아무런 일도 하지 않음]
	if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		return FALSE;
	}

	//	NOTE
	//		두번째 파일 개수
	if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		return FALSE;
	}	

	for ( int i = 0; i < FileInfo.Ver; i++ )
	{
		SFILENODE NewFile;
		if ( 1 != fread( &NewFile, sizeof( SFILENODE ), 1, fp ) )
		{
			return FALSE;
		}
		m_ClientFileTree.Add( &NewFile ); 
	}
	fclose ( fp );

	return TRUE;
}

BOOL CHttpPatchThread::MakeNewList( const int cPatchVer, const int sPatchVer, const int cGameVer, const int sGameVer )
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

BOOL CHttpPatchThread::DownloadFilesByHttp( CHttpPatch* pHttpPatch )
{
	int DownCount = 0;

	// Temp 폴더패스를 설정한다.
	//
	CString	DownloadDir( NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp );

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
		DownCount++;

		if ( !bAlreadyDown ) // MEMO : 이미 다운받은 파일은 리스트에 표시하지 않는다.
		{
			CString	strMsg;
			strMsg.LoadString( IDS_MESSAGE_047 );
			// 동기화 문제 때문에 메세지를 보낼때 항상 메모리를 새로 생성한다.
			CHAR * szListMessage = new CHAR[256]; 
			wsprintf( szListMessage, "%s %s", strMsg.GetString(), pNewFile->FileName );
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );
		}

		ULONGLONG TotalPos = 10 + (DownCount * 60) / (int)m_vectorNewFile.size();			
		NS_LOG_CONTROL::SetProcessAllPosition( TotalPos, 100 );
		
		CString FullSubPath = pNewFile->SubPath;

		if ( !bAlreadyDown ) //	Note : 아직 다운로드 안된 것만 GetFile한다.
		{
			if ( !GETFILE_USEHTTP ( pHttpPatch, FullSubPath.GetString(), pNewFile->FileName ) )
			{
				return FALSE;
			}
		}

		if( IsForceTerminate() ) return FALSE;

		//	<--	정상적으로 다운로드 했으면 리스트 작성한다.	--> //
		m_vectorDownFile.push_back( pNewFile );

		Sleep( 0 ); // 파일받다가 멈추는것 방지용
	}

	if ( m_vectorNewFile.size() != m_vectorDownFile.size () )
	{
		return FALSE; // 다운받은 파일과 목록에 있는 파일의 개수가 다르다면 실패
	}

	LoadCopyList (); // 설치했던 리스트를 로딩한다.

	return TRUE;
}

BOOL CHttpPatchThread::Installing()
{
	CString strTemp;
	int InstallCount = 0;

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
			InstallCount++;

			if ( !bAlreadyCopy )
			{
				CString	strMsg;
				strMsg.LoadString( IDS_MESSAGE_048 );
				CHAR * szListMessage = new CHAR[256];
				wsprintf( szListMessage, "%s %s", strMsg.GetString(), pNewFile->FileName );
				::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );
			}

			ULONGLONG TotalPos = 70 + (InstallCount * 20) / m_vectorDownFile.size();
			NS_LOG_CONTROL::SetProcessAllPosition ( TotalPos, 100 );

			if ( !bAlreadyCopy )
			{
				CString	DownloadDir;
				DownloadDir = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp + pNewFile->FileName;		

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

BOOL CHttpPatchThread::DeleteNotFoundFile()
{
	CString	str;
	int nCnt = static_cast<int>( m_vectorDeleteFile.size() );

	for ( int i = 0; i < nCnt; i++ )
	{
		SFILENODE* pNewFile = m_vectorDeleteFile[i];

		str.Format( "%s%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), pNewFile->SubPath, pNewFile->FileName );
		str.MakeLower();
		str = str.Left( str.ReverseFind ( '.' ) );

		if( str.Find( ".dll" ) == -1 )
		{
			if( !DeleteFile( str.GetString() ) ) return FALSE;
		}
	}

	return TRUE;
}

BOOL CHttpPatchThread::DeleteDownFiles()
{
	CString	strTemp;
	int nCnt = static_cast<int>( m_vectorDownFile.size() );

	for ( int i = 0; i < nCnt; ++i )
	{
		SFILENODE* pNewFile = m_vectorDownFile[i];
		strTemp.Format( "%s%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), NS_GLOBAL_VAR::strDownloadTemp.GetString(), pNewFile->FileName );

		if( !DeleteFile( strTemp.GetString() ) ) return FALSE;
	}

	return TRUE;
}

BOOL CHttpPatchThread::Destroy()
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

	return TRUE;
}

BOOL CHttpPatchThread::VersionUp( int sGameVer )
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
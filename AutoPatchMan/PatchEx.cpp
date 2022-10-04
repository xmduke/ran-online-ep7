#include "stdafx.h"
#include "Resource.h"
#include "Patch.h"

#include "PatchThread.h"

//#include "PatchBinTree.h"
#include "patchset.h"

#include "GlobalVariable.h"
#include <string>
#include "LogControl.h"
#include "RANPARAM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern	FILEVECTOR		g_vectorServerFile;
extern	CPatchSet		g_ClientFileTree;
extern	FILEVECTOR		g_vectorNewFile;
extern	FILEVECTOR		g_vectorDeleteFile;
extern	FILEVECTOR		g_vectorDownFile;
extern	FILEMAP			g_mapDownedFile;

extern	FILEVECTOR		g_vectorCopyFile;
extern	FILEMAP			g_mapCopiedFile;

extern	int	g_sFailGameVer;

BOOL	Initialize ()
{	
	return TRUE;
}

BOOL	Destroy ()
{
	// g_vectorServerFile 삭제 //////////////////////////////////////
	for ( int i = 0; i < (int)g_vectorServerFile.size (); i++ )
	{
		//SFILENODE* pFileNode = g_vectorServerFile[i];
		delete g_vectorServerFile[i];
	}
	g_vectorServerFile.clear ();
	/////////////////////////////////////////////////////////////////

	g_ClientFileTree.Clear ();

	/////////////////////////////////////////////////////////////////
	for ( int i = 0; i < (int)g_vectorNewFile.size (); i++ )
	{
		//SFILENODE* pFileNode = g_vectorNewFile[i];
		delete g_vectorNewFile[i];
	}
	g_vectorNewFile.clear ();
	/////////////////////////////////////////////////////////////////

	g_vectorDownFile.clear (); // MEMO : 중복된 포인터라서 delete 할 필요없음.
	
//	g_DownFileList.RemoveAll();
//	g_DeleteFileList.RemoveAll();

	// g_mapDownedFile 삭제 ///////////////////////////
	FILEMAP_ITER iter = g_mapDownedFile.begin ();
	FILEMAP_ITER iter_end = g_mapDownedFile.end ();
	for ( ; iter != iter_end; iter++ )
	{
		//SFILENODE* pFileNode = (*iter).second;
		delete (*iter).second;
	}
	g_mapDownedFile.clear ();
	///////////////////////////////////////////////////

	g_vectorCopyFile.clear();

	// g_mapDownedFile 삭제 ///////////////////////////
	iter = g_mapCopiedFile.begin ();
	iter_end = g_mapCopiedFile.end ();
	for ( ; iter != iter_end; iter++ )
	{
		//SFILENODE* pFileNode = (*iter).second;
		delete (*iter).second;
	}
	g_mapCopiedFile.clear ();
	///////////////////////////////////////////////////

	g_vectorDeleteFile.clear ();

	return TRUE;
}

BOOL	DeleteDownFiles ()
{
	CString	strTemp;
	int nCnt = static_cast<int>( g_vectorDownFile.size() );

	for ( int i = 0; i < nCnt; ++i )
	{
		SFILENODE* pNewFile = g_vectorDownFile[i];
		strTemp.Format ( "%s%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::strDownloadTemp.GetString(), pNewFile->FileName );

		DeleteFile ( strTemp.GetString() );
	}

	return TRUE;
}

BOOL	DeleteNotFoundFile()
{
	CString	str;
	int nCnt = static_cast<int>( g_vectorDeleteFile.size() );

	for ( int i = 0; i < nCnt; i++ )
	{
		SFILENODE* pNewFile = g_vectorDeleteFile[i];

		str.Format ( "%s%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), pNewFile->SubPath, pNewFile->FileName );
		str.MakeLower();
		str = str.Left ( str.ReverseFind ( '.' ) );

		DeleteFile ( str.GetString() );
	}

	return TRUE;
}

BOOL LoadList()
{
	//	<--	리스트 읽기	-->	//
	CString str;
	FILE* fp;

	str.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::g_szServerFileList );
	fp = fopen ( str.GetString(), "rb" );
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
		strTemp = strTemp.Left ( strTemp.ReverseFind ( '.' ) );
		strTemp.MakeUpper();

		if( strTemp.Find( ".DLL" ) != -1 )
		{
			CDebugSet::ToLogFile( "서버 파일 리스트에 [%s]이 있습니다.", pNewFile->FileName );
		}
		else
		{
			g_vectorServerFile.push_back ( pNewFile );
		}
	}
	fclose ( fp );


	//	<--	'강제종료'	-->	//
	if ( NS_PATCH_THREAD::IsForceTerminate () )
	{
		return FALSE;
	}

	str.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::g_szClientFileList );
	fp = fopen ( str.GetString(), "rb" );
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
		if ( 1 != fread ( &NewFile, sizeof ( SFILENODE ), 1, fp ) )
		{
			return FALSE;
		}
		g_ClientFileTree.Add ( &NewFile ); 
	}
	fclose ( fp );

	return TRUE;
}

BOOL	VersionUp ( int sGameVer )
{	
	CString strTemp;
	strTemp.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), g_szClientVerFile );
	SetFileAttributes ( strTemp.GetString(), FILE_ATTRIBUTE_NORMAL );

	FILE* cfp = fopen ( strTemp.GetString(), "rb+" );	
	if ( cfp )
	{		
		fseek ( cfp, sizeof ( int ) * 1, SEEK_SET );
		if ( 1 != fwrite ( &sGameVer, sizeof ( int ), 1, cfp ) )
		{
			fclose ( cfp );
			return FALSE;
		}
		fclose ( cfp );
	}

	return TRUE;
}

BOOL	SaveDownList ( int sGameVer )
{
	CString strTemp;
	strTemp = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownList;

	//EnterCriticalSection ( &NS_PATCH_THREAD::CRITICAL_SECTION_INSTANCE );

		FILE* fp = fopen ( strTemp, "wb" );
		if( !fp )
		{
			CDebugSet::ToLogFile( "다운 완료 파일 쓰기에 실패했습니다." );
			return FALSE;
		}

		SFILENODE FileInfo;

		FileInfo.Ver = sGameVer;
		if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
		{
			CDebugSet::ToLogFile( "다운 완료 파일 쓰기에 실패했습니다." );
			return FALSE;
		}

		FileInfo.Ver = (int) g_vectorDownFile.size ();
		if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
		{
			CDebugSet::ToLogFile( "다운 완료 파일 쓰기에 실패했습니다." );
			return FALSE;
		}

		for ( int i = 0; i < (int)g_vectorDownFile.size (); i++ )
		{
			SFILENODE* pNewFile = g_vectorDownFile[i];

			if ( 1 != fwrite ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
			{
				CDebugSet::ToLogFile( "다운 완료 파일 쓰기에 실패했습니다." );
				return FALSE;
			}
		}

	//LeaveCriticalSection ( &NS_PATCH_THREAD::CRITICAL_SECTION_INSTANCE );

	return TRUE;
}

BOOL	LoadDownList ()
{
	CString strTemp;
	strTemp = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownList;

    FILE* fp = fopen ( strTemp, "rb" );
	if ( fp )
	{
		SFILENODE FileInfo;

		if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
		{
//			GASSERT ( 0 && "다운 완료 파일 읽기에 실패했습니다." );
			return FALSE;
		}
		g_sFailGameVer = FileInfo.Ver;

		if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
		{
//			GASSERT ( 0 && "다운 완료 파일 읽기에 실패했습니다." );
			return FALSE;
		}	

		for ( int i = 0; i < FileInfo.Ver; i++ )
		{
			SFILENODE* pNewFile = new SFILENODE;
			if ( 1 != fread ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
			{
//				GASSERT ( 0 && "다운 완료 파일 읽기에 실패했습니다." );
				return FALSE;
			}

			g_mapDownedFile.insert ( std::make_pair(std::string(pNewFile->FileName), pNewFile) );
		}
		fclose ( fp );
	}

	return TRUE;
}

BOOL	DeleteDownList ()
{
	GASSERT ( 0 && "제작해야합니다." );
	return TRUE;
}

BOOL	SaveCopyList ( int sGameVer )
{
	CString strTemp;
	strTemp = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strCopyList;

	//EnterCriticalSection ( &NS_PATCH_THREAD::CRITICAL_SECTION_INSTANCE );

		FILE* fp = fopen ( strTemp, "wb" );
		if ( fp )
		{
			SFILENODE FileInfo;

			FileInfo.Ver = sGameVer;
			if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
			{
				GASSERT ( 0 && "다운 완료 파일 쓰기에 실패했습니다." );
				return FALSE;
			}

			FileInfo.Ver = (int) g_vectorCopyFile.size ();
			if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
			{
				GASSERT ( 0 && "다운 완료 파일 쓰기에 실패했습니다." );
				return FALSE;
			}

			for ( int i = 0; i < (int)g_vectorCopyFile.size (); i++ )
			{
				SFILENODE* pNewFile = g_vectorCopyFile[i];

				if ( 1 != fwrite ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
				{
					GASSERT ( 0 && "다운 완료 파일 쓰기에 실패했습니다." );
					return FALSE;
				}
			}
		}

	//LeaveCriticalSection ( &NS_PATCH_THREAD::CRITICAL_SECTION_INSTANCE );

	return TRUE;
}

BOOL	LoadCopyList ()
{
	CString strTemp;
	strTemp = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strCopyList;

    FILE* fp = fopen ( strTemp, "rb" );
	if ( fp )
	{
		SFILENODE FileInfo;

		if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
		{
//			GASSERT ( 0 && "다운 완료 파일 읽기에 실패했습니다." );
			return FALSE;
		}
		g_sFailGameVer = FileInfo.Ver;

		if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
		{
//			GASSERT ( 0 && "다운 완료 파일 읽기에 실패했습니다." );
			return FALSE;
		}	

		for ( int i = 0; i < FileInfo.Ver; i++ )
		{
			SFILENODE* pNewFile = new SFILENODE;
			if ( 1 != fread ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
			{
//				GASSERT ( 0 && "다운 완료 파일 읽기에 실패했습니다." );
				return FALSE;
			}

			g_mapCopiedFile.insert ( std::make_pair(std::string(pNewFile->FileName), pNewFile) );
		}
		fclose ( fp );
	}

	return TRUE;
}

BOOL	DeleteCopyList ()
{
	GASSERT ( 0 && "제작해야합니다." );
	return TRUE;
}

BOOL	GETFILE_USEHTTP ( CHttpPatch* pHttpPatch, std::string strRemoteSubPath, std::string strFileName, CString strTempDir )
{
	if ( !pHttpPatch )
	{
		GASSERT ( 0 && "잘못된 연산을 수행할 수 있습니다." );
		return FALSE;
	}

    if ( !strFileName.size () )
	{
		GASSERT ( 0 && "파일이 지정되지 않았습니다." );
		return FALSE;
	}

	//	'\\'문자를 '/'로 변경한다.
	replace ( strRemoteSubPath.begin(), strRemoteSubPath.end(), '\\','/' );	
	strRemoteSubPath += strFileName;
	
	std::string strLocalFullPath;
	strLocalFullPath += NS_GLOBAL_VAR::strAppPath.GetString();	
	strLocalFullPath += strTempDir.GetString();
	strLocalFullPath += strFileName;

	static int nTRY = 0;
	int nTRY_FILESIZE_CHECK = 0;
	int nERROR_RETRY = 0;
	int nADDRESS_NULL_COUNT = 0;
	while ( nTRY_FILESIZE_CHECK < 3 )
	{
		Sleep( 0 );

		//	강제 종료시
		if ( NS_PATCH_THREAD::IsForceTerminate () ) return FALSE;		
        
		//	NOTE
		//		최대 시도 회수 초과시
		if ( nADDRESS_NULL_COUNT == RANPARAM::MAX_HTTP ) return FALSE;

		if ( RANPARAM::MAX_HTTP == nTRY ) nTRY = 0;
		if ( nERROR_RETRY == 5 ) return FALSE;		
		
		static const CString strHTTP = "http://";
		
		CString strRealAddress = RANPARAM::HttpAddressTable[nTRY];
		if ( !strRealAddress.GetLength () )
		{
			nADDRESS_NULL_COUNT++;		//	MAX_HTTP가 모두 널인가?
			nTRY++;
			continue;
		}

		//	널 체크를 통과했다는 것은 nADDRESS_NULL_COUNT을 초기화해야함을 의미한다.
		nADDRESS_NULL_COUNT = 0;

		CString strHttpAddress = strHTTP + strRealAddress;
		if ( NET_ERROR == pHttpPatch->SetBaseURL ( strHttpAddress ) )
		{			
			NS_LOG_CONTROL::Write ( pHttpPatch->GetErrMsg() );
			nTRY++;
			nERROR_RETRY++;
			continue;
		}
		
		if ( NET_ERROR == pHttpPatch->GetFile ( strRemoteSubPath.c_str(), strLocalFullPath.c_str() ) )
		{
			NS_LOG_CONTROL::Write ( pHttpPatch->GetErrMsg() );
			
			CString	strTemp, strMsg;
			strMsg.LoadString( IDS_MESSAGE_047 );
			strTemp.Format ( "%s %s", strMsg.GetString(), strFileName.c_str() );

			NS_LOG_CONTROL::Write ( strTemp );

			nTRY ++;
			nERROR_RETRY++;

			continue;
		}

		//	강제 종료시
		if ( NS_PATCH_THREAD::IsForceTerminate () ) return FALSE;	

		ULONGLONG ulRECEIVED, ulTOTALSIZE;
		NS_LOG_CONTROL::GetProcessCurPosition ( &ulRECEIVED, &ulTOTALSIZE );
		
		if ( ulRECEIVED != ulTOTALSIZE )
		{
			nTRY_FILESIZE_CHECK++;
			NS_LOG_CONTROL::Write ( IDS_MESSAGE_046 ); // 전송 받는 파일 크기가 맞지 않다.	
			continue;
		}
		else
		{
			//NOTE	
			//	NET_OK
			//	성공한 것
			return TRUE;
		}        
	}

	return FALSE;
}

//BOOL	CheckIntegrity ( CString strPath )
//{	
//	static	CString strTempDir = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp;
//
//	//	모든 파일에 대해서 검사를 진행함.
//	if ( !strPath.GetLength () ) return FALSE;
//	if ( strPath[strPath.GetLength()-1] != '\\') strPath += "\\";
//	strPath += "*.*";
//
//	CFileFind finder;	
//	BOOL bWorking = finder.FindFile ( strPath.GetString () );
//
//	CString strFilePath;
//	while (bWorking)
//	{
//		if ( NS_PATCH_THREAD::IsForceTerminate () )	return FALSE;
//
//		bWorking = finder.FindNextFile();
//
//		// skip . and .. files; otherwise, we'd
//		// recur infinitely!
//		if (finder.IsDots())
//			continue;
//
//		ULONGLONG ulCurCount;
//		ULONGLONG ulServerFile;
//		NS_LOG_CONTROL::GetProcessCurPosition ( &ulCurCount, &ulServerFile );
//		ulCurCount++;
//		NS_LOG_CONTROL::SetProcessCurPosition ( ulCurCount, ulServerFile );
//        
//		strFilePath = finder.GetFilePath ();        
//		if ( finder.IsDirectory () )
//		{
//			if ( strPath == strTempDir )
//				continue;
//
////			NS_LOG_CONTROL::Write ( strFilePath );
//			if ( !CheckIntegrity ( strFilePath ) )
//			{
//				finder.Close ();
//				return FALSE;
//			}
//		}
//		else
//		{
//			//	일반 파일
//			//
////			NS_LOG_CONTROL::Write ( strFilePath );
//		}
//	}
//	finder.Close ();
//
//	return TRUE;
//}
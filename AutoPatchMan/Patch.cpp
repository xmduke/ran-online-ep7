#include "stdafx.h"
#include "Resource.h"
#include "Patch.h"

#include "PatchThread.h"

#include "dxutil.h"
#include <afxinet.h>
#include "s_NetClient.h"
#include "s_CPatch.h"
#include "CompactFdi.h"
#include "StringUtils.h"
#include "patchset.h"

#include "GlobalVariable.h"
#include "RANPARAM.h"
#include "LogControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define		_MAKE_PATCH_STATUS // MEMO : 디버그 정의

FILEVECTOR		g_vectorServerFile;
CPatchSet		g_ClientFileTree;
FILEVECTOR		g_vectorNewFile;
FILEVECTOR		g_vectorDeleteFile;
FILEVECTOR		g_vectorDownFile;
FILEMAP			g_mapDownedFile;

FILEVECTOR		g_vectorCopyFile;
FILEMAP			g_mapCopiedFile;

int		g_sFailGameVer = 0;


BOOL	MakeNewList ( const int cPatchVer, const int sPatchVer,
					 const int cGameVer, const int sGameVer )
{	
	NS_LOG_CONTROL::SetProcessCurPosition ( 0, 1 );

#ifdef	_MAKE_PATCH_STATUS
	CDebugSet::ClearFile ("_STATUS.txt");
	CDebugSet::ClearFile ("_ALL_STATUS.txt");
	CDebugSet::ClearFile ("_TEST_filelist.lst");
	static	CString strDIRBACK;
	strDIRBACK.Empty ();
	int	nVER_DOWN_COUNT = 0;
	int nVER_NORMAL = 0;

	CDebugSet::ToFileWithTime ( "_ALL_STATUS.txt", "BEGIN-----------------------------------" );
	CDebugSet::ToFileWithTime ( "_ALL_STATUS.txt", "PatchVer - c:%d/s:%d", cPatchVer, sPatchVer );
	CDebugSet::ToFileWithTime ( "_ALL_STATUS.txt", "GameVer - c:%d/s:%d", cGameVer, sGameVer );

	CDebugSet::ToFileWithTime ( "_STATUS.txt", "PatchVer - c:%d/s:%d", cPatchVer, sPatchVer );
	CDebugSet::ToFileWithTime ( "_STATUS.txt", "GameVer - c:%d/s:%d", cGameVer, sGameVer );

	CDebugSet::ToFileWithTime ( "_TEST_filelist.lst", "PatchVer - c:%d/s:%d", cPatchVer, sPatchVer );
	CDebugSet::ToFileWithTime ( "_TEST_filelist.lst", "GameVer - c:%d/s:%d", cGameVer, sGameVer );
#endif	//	_MAKE_PATCH_STATUS

	FILEVECTOR::size_type nServerFileSize = g_vectorServerFile.size ();
	for ( FILEVECTOR::size_type i = 0; i < nServerFileSize; i++ )
	{	
		SFILENODE* pServerGetFile = g_vectorServerFile[i];
#ifdef	_MAKE_PATCH_STATUS
		if ( strDIRBACK != pServerGetFile->SubPath )
		{
			CDebugSet::ToFile ( "_TEST_filelist.lst", "\n%s", pServerGetFile->SubPath );
			strDIRBACK = pServerGetFile->SubPath;
		}
#endif	//	_MAKE_PATCH_STATUS

        CPatchSetNode* pNode = g_ClientFileTree.Find ( pServerGetFile );
		if ( pNode )	//	기존 파일
		{
			SFILENODE* pClientFile = pNode->GetFile ();
			pNode->SetUseFlag ();

			if ( pClientFile->Ver < pServerGetFile->Ver )
			{
				SFILENODE* pNewFile = new SFILENODE;
				*pNewFile = *pServerGetFile;
				g_vectorNewFile.push_back ( pNewFile );

#ifdef	_MAKE_PATCH_STATUS
				CDebugSet::ToFile ( "_ALL_STATUS.txt", "[VersinoUp]%s {c:%d<s:%d}",
					pServerGetFile->FileName, pClientFile->Ver, pServerGetFile->Ver );

				CString strFileName = pServerGetFile->FileName;
				strFileName = strFileName.Left ( strFileName.ReverseFind ( '.' ) );
				CDebugSet::ToFile ( "_TEST_filelist.lst", "\t%s", strFileName.GetString() );
#endif	//	_MAKE_PATCH_STATUS
			}
#ifdef	_MAKE_PATCH_STATUS
			else
			{
				if ( pClientFile->Ver == pServerGetFile->Ver )
				{
					nVER_NORMAL++;
					CDebugSet::ToFile ( "_ALL_STATUS.txt", "[변화없음]%s {c:%d==s:%d}",
					pClientFile->FileName, pClientFile->Ver, pServerGetFile->Ver );
				}
				else
				{
					nVER_DOWN_COUNT++;
					CDebugSet::ToFile ( "_ALL_STATUS.txt", "[버전낮음]%s {c:%d>s:%d}",
					pClientFile->FileName, pClientFile->Ver, pServerGetFile->Ver );
				}
			}
#endif	//	_MAKE_PATCH_STATUS
		}
		else	//	추가되는 파일
		{			
			SFILENODE* pNewFile = new SFILENODE;
			*pNewFile = *pServerGetFile;
			g_vectorNewFile.push_back ( pNewFile );
#ifdef	_MAKE_PATCH_STATUS
			CDebugSet::ToFile ( "_ALL_STATUS.txt", "[Addition]%s {%d}",
				pServerGetFile->FileName, pServerGetFile->Ver );

			CString strFileName = pServerGetFile->FileName;
			strFileName = strFileName.Left ( strFileName.ReverseFind ( '.' ) );
			CDebugSet::ToFile ( "_TEST_filelist.lst", "\t%s", strFileName );
#endif	//	_MAKE_PATCH_STATUS
		}

		NS_LOG_CONTROL::SetProcessCurPosition ( i, nServerFileSize );

		//	강제종료
		if ( NS_PATCH_THREAD::IsForceTerminate () )	return FALSE;
	}
	NS_LOG_CONTROL::SetProcessCurPosition ( 1, 1 );

	//	<--	서버에 없는 파일이 클라이언트에 존재할 경우 삭제	-->	//
	g_ClientFileTree.GetNotUseItem ( &g_vectorDeleteFile );

#ifdef	_MAKE_PATCH_STATUS
	for ( int i = 0; i < (int)g_vectorDeleteFile.size (); i++ )
	{
		SFILENODE* pFileNode = g_vectorDeleteFile[i];
		CDebugSet::ToFile ( "_ALL_STATUS.txt", "[삭제]%s", pFileNode->FileName );
	}
	
	//CDebugSet::ToFile ( "_STATUS.txt", "등록된 파일 개수\t: c:%d-s:%d", g_ClientFileTree.GetCount (), g_vectorServerFile.size() );
	CDebugSet::ToFile ( "_STATUS.txt", "---------------" );
	CDebugSet::ToFile ( "_STATUS.txt", "업데이트[버전업/추가]\t: %d", g_vectorNewFile.size() );
	CDebugSet::ToFile ( "_STATUS.txt", "삭제\t\t\t: %d", g_vectorDeleteFile.size () );
	CDebugSet::ToFile ( "_STATUS.txt", "버전같음\t\t: %d", nVER_NORMAL );
	CDebugSet::ToFile ( "_STATUS.txt", "버전다운\t\t: %d", nVER_DOWN_COUNT );
	CDebugSet::ToFile ( "_STATUS.txt", "---------------" );
	CDebugSet::ToFile ( "_STATUS.txt", "전체[%d]\n = 업데이트[%d] + 삭제[%d] + 버전같음[%d] + 버전다운[%d]",
		g_vectorServerFile.size (), g_vectorNewFile.size(), g_vectorDeleteFile.size (), nVER_NORMAL, nVER_DOWN_COUNT );	
	CDebugSet::ToFile ( "_STATUS.txt", "-----------------------------------" );

	int nSUM = (int)g_vectorNewFile.size() + (int)g_vectorDeleteFile.size () + nVER_NORMAL + nVER_DOWN_COUNT;
	if ( (int)g_vectorServerFile.size () == nSUM )
		CDebugSet::ToFile ( "_STATUS.txt", "정상");
	else
		CDebugSet::ToFile ( "_STATUS.txt", "오류 - 파일 개수 다름");

#endif	//	_MAKE_PATCH_STATUS

	LoadDownList ();	

	return TRUE;
}

BOOL DownloadFilesByHttp ( CHttpPatch* pHttpPatch )
{
	int DownCount = 0;

	BOOL	bFirst = TRUE;
	SFILENODE* pOldFile = NULL;

	// Temp 폴더패스를 설정한다.
	//
	CString	DownloadDir;
	DownloadDir = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp;

	int nErrorCount = 0;
	const int nMaxError = 5;

	for ( int i = 0; i < (int) g_vectorNewFile.size (); ++i )
	{
		nErrorCount = 0;
		SFILENODE* pNewFile = g_vectorNewFile[i];		
		
		//	Note : 이미 정상적으로 다운로드 완료된 것.
        BOOL bAlreadyDown = FALSE;
		FILEMAP_ITER found = g_mapDownedFile.find ( std::string ( pNewFile->FileName ) );
		FILEMAP_ITER iter_end = g_mapDownedFile.end ();
		if ( found != iter_end ) bAlreadyDown = TRUE;

		//	<--	'강제종료'	-->	//
		if ( NS_PATCH_THREAD::IsForceTerminate () )
		{
			return FALSE;
		}
		
		{
			//	<--	카운트는 현재 진행하고 있는것을 포함한다.	-->	//
			//	<**	현재 첫번째것을 받고 있으면, 카운트는 1이 되는 것이다.
			//		완료 기준이 아니라, 진행 기준이다.			
			DownCount++;
			//	**>

			if ( !bAlreadyDown ) // MEMO : 이미 다운받은 파일은 리스트에 표시하지 않는다.
			{
				CString	strTemp, strMsg;
				strMsg.LoadString( IDS_MESSAGE_047 );
				strTemp.Format ( "%s %s", strMsg.GetString(), pNewFile->FileName );

				NS_LOG_CONTROL::Write ( strTemp );	
			}

			ULONGLONG TotalPos = 10 + (DownCount * 60)/(int)g_vectorNewFile.size();			
			NS_LOG_CONTROL::SetProcessAllPosition ( TotalPos, 100 );
		}

		CString FullSubPath = pNewFile->SubPath;

		//	Note : 아직 다운로드 안된 것만 GetFile한다.
		//
		if ( !bAlreadyDown )
		{
#ifdef	_MAKE_PATCH_STATUS
			CDebugSet::ToFile ( "_ALL_STATUS.txt", "[전송시작]%s", pNewFile->FileName );
#endif	//	_MAKE_PATCH_STATUS
			if ( !GETFILE_USEHTTP ( pHttpPatch, FullSubPath.GetString(), pNewFile->FileName ) )
			{
#ifdef	_MAKE_PATCH_STATUS
				if ( !NS_PATCH_THREAD::IsForceTerminate () )
				{
					CDebugSet::ToFile ( "_ALL_STATUS.txt", "[전송오류]" );
				}
#endif	//	_MAKE_PATCH_STATUS
				return FALSE;
			}
#ifdef	_MAKE_PATCH_STATUS
			CDebugSet::ToFile ( "_ALL_STATUS.txt", "[전송완료]" );
#endif	//	_MAKE_PATCH_STATUS
		}
		else
		{
#ifdef	_MAKE_PATCH_STATUS
            CDebugSet::ToFile ( "_ALL_STATUS.txt", "[이미받은파일]%s", pNewFile->FileName );
#endif	//	_MAKE_PATCH_STATUS
		}	

		if ( NS_PATCH_THREAD::IsForceTerminate () )
		{
			return FALSE;
		}

		//	<--	정상적으로 다운로드 했으면
		//		리스트 작성한다.
		//	-->
		g_vectorDownFile.push_back ( pNewFile );
		pOldFile = pNewFile;	

		//	NOTE
		//		파일받다가 멈추는것 방지용
		Sleep( 0 );
	}

    if ( g_vectorNewFile.size() != g_vectorDownFile.size () )
	{
		return FALSE;
	}

	LoadCopyList (); // 설치했던 리스트를 로딩한다.

	return TRUE;
}

//BOOL DownloadFilesByFtp ( CPatch* pFtpPatch )
//{
//	int DownCount = 0;
//
//	BOOL	bFirst = TRUE;
//	SFILENODE* pOldFile = NULL;
//
//	CString	DownloadDir;
//	DownloadDir = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp;
//	pFtpPatch->SetLocalDir ( DownloadDir.GetString () );
//
//	int nErrorCount = 0;
//	const int nMaxError = 5;
//
//	for ( int i = 0; i < (int) g_vectorNewFile.size (); ++i )
//	{
//		nErrorCount = 0;
//		SFILENODE* pNewFile = g_vectorNewFile[i];		
//		
//		//	Note : 이미 정상적으로 다운로드 완료된 것.
//        BOOL bAlreadyDown = FALSE;
//		FILEMAP_ITER found = g_mapDownedFile.find ( std::string ( pNewFile->FileName ) );
//		FILEMAP_ITER iter_end = g_mapDownedFile.end ();
//		if ( found != iter_end ) bAlreadyDown = TRUE;
//
//		//	<--	'강제종료'	-->	//
//		if ( NS_PATCH_THREAD::IsForceTerminate () )
//		{
//			return FALSE;
//		}
//		else
//		{
//			//	<--	카운트는 현재 진행하고 있는것을 포함한다.	-->	//
//			//	<**	현재 첫번째것을 받고 있으면, 카운트는 1이 되는 것이다.
//			//		완료 기준이 아니라, 진행 기준이다.			
//			DownCount++;
//			//	**>
//
//			CString	strTemp, strMsg;
//			strMsg.LoadString( IDS_MESSAGE_047 );
//			strTemp.Format ( "%s %s", strMsg.GetString(), pNewFile->FileName );			
//			ULONGLONG TotalPos = 10 + (DownCount * 60)/(int)g_vectorNewFile.size();			
//			
//			NS_LOG_CONTROL::Write ( strTemp );	
//			NS_LOG_CONTROL::SetProcessAllPosition ( TotalPos, 100 );
//		}
//
//		if ( bFirst || strcmp ( pOldFile->SubPath, pNewFile->SubPath ) )
//		{
//			CString FullSubPath = pNewFile->SubPath;
//
//			//	루트로 이동
//			pFtpPatch->SetCurrentDirectory ( "\\" );
//			pFtpPatch->SetCurrentDirectory ( FullSubPath.GetString() );
//
//			bFirst = FALSE;
//		}
//		
//		//	Note : 아직 다운로드 안된 것만 GetFile한다.
//		//
//		if ( !bAlreadyDown )
//		{
//			//	재시도
//			while ( nErrorCount != nMaxError )
//			{	
//				if ( pFtpPatch->GetFile ( pNewFile->FileName ) == NET_ERROR )
//				{
//					//	<--	'강제종료'	-->	//
//					if ( NS_PATCH_THREAD::IsForceTerminate () )
//					{
//						return FALSE;
//					}						
//
//					if ( nMaxError == nErrorCount )
//					{
//						return FALSE;
//					}
//
//					{	//	재 접속							
//						pFtpPatch->DisConnect ();
//
//						CString strFtpAddress;
//						int nIndex = nErrorCount % RANPARAM::MAX_FTP;
//						strFtpAddress = RANPARAM::FtpAddressTable[nIndex];
//
//						int nRetCode = pFtpPatch->Connect( strFtpAddress.GetString (),
//											21,
//											NS_GLOBAL_VAR::g_szBetaFtpID,
//											NS_GLOBAL_VAR::g_szBetaFtpPW,
//											RANPARAM::bUsePassiveDN );
//
//						if ( nRetCode == NET_ERROR )
//						{
//							return FALSE;
//						}
//
//
//						CString FullSubPath = pNewFile->SubPath;
//
//						//	루트로 이동
//						pFtpPatch->SetCurrentDirectory ( "\\" );
//						pFtpPatch->SetCurrentDirectory ( FullSubPath.GetString() );
//					}
//
//					nErrorCount++;
//
//					continue ;
//				}
//				break;
//			}				
//		}
//
//		//	<--	정상적으로 다운로드 했으면
//		//		리스트 작성한다.
//		//	-->
//		g_vectorDownFile.push_back ( pNewFile );
//		pOldFile = pNewFile;		
//	}
//
//    if ( g_vectorNewFile.size() != g_vectorDownFile.size () )
//	{
//		return FALSE;
//	}
//
//	return TRUE;
//}

BOOL	Installing ()
{
	int InstallCount = 0;

	for ( int i = 0; i < (int) g_vectorDownFile.size (); ++i )
	{
		SFILENODE* pNewFile = g_vectorDownFile[i];	

		//	Note : 이미 정상적으로 다운로드 완료된 것.
		BOOL bAlreadyCopy(FALSE);
		FILEMAP_ITER found = g_mapCopiedFile.find ( std::string ( pNewFile->FileName ) );
		FILEMAP_ITER iter_end = g_mapCopiedFile.end ();
		if ( found != iter_end ) bAlreadyCopy = TRUE;

		//	<--	'강제종료'	-->	//
		if ( NS_PATCH_THREAD::IsForceTerminate () )
		{
			return FALSE;
		}

		CString	Seperator = "\\";
		CString FullSubPath = pNewFile->SubPath;
		CStringArray SubPathArray;

		STRUTIL::ClearSeparator ();
		STRUTIL::RegisterSeparator ( Seperator );
		STRUTIL::StringSeparate ( FullSubPath, SubPathArray );		

		//	<--	없는 디렉토리 만들기	-->	//
		CString FullPath;
		FullPath.Format ( "%s", NS_GLOBAL_VAR::strAppPath.GetString () );
		for ( int i = 0; i < SubPathArray.GetCount (); i++ )
		{
			FullPath += SubPathArray[i];			
			CreateDirectory ( FullPath.GetString (), NULL );
			FullPath += "\\";
		}

		//	<--	압축 풀기위한 전체 경로 설정 및 압축 풀기&설치	-->	//
		//NS_PATCH_THREAD::SleepThread ();
		//NS_PATCH_THREAD::SetProcessCurPosition ( 0, 1 );
		NS_LOG_CONTROL::SetProcessCurPosition ( 0, 1 );
		{
			InstallCount++;

			if ( !bAlreadyCopy )
			{
				CString	strTemp, strMsg;
				strMsg.LoadString( IDS_MESSAGE_048 );
				strTemp.Format ( "%s %s", strMsg.GetString(), pNewFile->FileName );
				NS_LOG_CONTROL::Write ( strTemp );
			}
			
			ULONGLONG TotalPos = 70 + (InstallCount * 20) / g_vectorDownFile.size();
			NS_LOG_CONTROL::SetProcessAllPosition ( TotalPos, 100 );

			if ( !bAlreadyCopy )
			{
				CString	DownloadDir;
				DownloadDir = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp + pNewFile->FileName;		

				CString	DestFile;
				DestFile.Format ( "%s%s", FullPath.GetString(), pNewFile->FileName );
				DestFile = DestFile.Left ( DestFile.ReverseFind ( '.' ) );

				if ( !SetFileAttributes ( DestFile.GetString(), FILE_ATTRIBUTE_NORMAL ) )
				{
				}
				if ( !DeleteFile ( DestFile.GetString() ) )
				{
				}

	#ifdef	_MAKE_PATCH_STATUS
				CDebugSet::ToFileWithTime ( "_ALL_STATUS.txt", "[설치]%s", DestFile.GetString () );
	#endif	//	_MAKE_PATCH_STATUS

				if ( !Extract ( FullPath.GetString (), DownloadDir.GetString() ) )
				{
					if ( !NS_PATCH_THREAD::IsForceTerminate () )
					{

	#ifdef	_MAKE_PATCH_STATUS
						CDebugSet::ToFileWithTime ( "_ALL_STATUS.txt", "[설치오류]%s{%s}", DestFile.GetString (), GetErrorMsg () );
	#endif	//	_MAKE_PATCH_STATUS

						NS_LOG_CONTROL::Write ( GetErrorMsg () );
						//	오류메시지				
						NS_PATCH_THREAD::SetExtractError ();
					}
					return FALSE;
				}

				if ( !SetFileAttributes ( DestFile.GetString(), FILE_ATTRIBUTE_NORMAL ) )
				{
				}
			}
		}
		NS_LOG_CONTROL::SetProcessCurPosition ( 1, 1 );

		//	<--	정상적으로 설치했으면
		//		리스트 작성한다.
		//	-->
		g_vectorCopyFile.push_back ( pNewFile );

		//	<--	'강제종료'	-->	//
		if ( NS_PATCH_THREAD::IsForceTerminate () ) return FALSE;
	}

#ifdef	_MAKE_PATCH_STATUS
	CDebugSet::ToFileWithTime ( "_ALL_STATUS.txt", "END---------------" );
#endif	//	_MAKE_PATCH_STATUS

	if ( g_vectorNewFile.size() != g_vectorCopyFile.size () )
	{
		return FALSE; // MEMO : 업데이트할 파일 개수와 설치한 파일 개수가 일치하지 않으면...
	}

	return TRUE;
}


//DWORD	PatchByFTP ( S_PATCH_THREAD_PARAM* pParam )
//{
//	CPatch*		pFtpPatch = pParam->pFtpPatch;
////	CHttpPatch* pHttpPatch = pParam->pHttpPatch;
//	const bool bUseHttp = pParam->bUseHttp;
//	const int cPatchVer = pParam->cPatchVer;
//	const int sPatchVer = pParam->sPatchVer;
//	const int cGameVer = pParam->cGameVer;
//	const int sGameVer = pParam->sGameVer;
//	
//	NS_LOG_CONTROL::SetProcessAllPosition ( 0, 100 );
//
//	CString cFileList, sFileList, str, strTemp;
//
//	if ( NS_PATCH_THREAD::IsForceTerminate () )
//	{
//		return 0;
//	}
//	
//	//	파일 리스트 다운로드	
//	pFtpPatch->SetLocalDir ( NS_GLOBAL_VAR::strAppPath.GetString () );
//	NS_LOG_CONTROL::Write ( IDS_MESSAGE_049 );	
//	pFtpPatch->SetCurrentDirectory ( "\\" );
//	if ( pFtpPatch->GetFile ( NS_GLOBAL_VAR::strServerCabFileList ) == NET_ERROR )
//	{
//		NS_LOG_CONTROL::Write ( IDS_MESSAGE_050 );
//		NS_LOG_CONTROL::Write ( IDS_MESSAGE_056 );
//		NS_LOG_CONTROL::Write ( IDS_MESSAGE_063 );
//		goto LFail;
//	}
//
//	if ( NS_PATCH_THREAD::IsForceTerminate () )
//	{
//		goto LFail;
//	}
//
//	//	압축 풀기위한 전체 경로 설정 및 압축 풀기&설치
//	str = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strServerCabFileList;
//	NS_LOG_CONTROL::Write ( IDS_MESSAGE_051 );	
//	if ( !Extract ( NS_GLOBAL_VAR::strAppPath.GetString (), str.GetString() ) )
//	{
//		NS_LOG_CONTROL::Write ( IDS_MESSAGE_052 );		
//		goto LFail;
//	}
//	NS_LOG_CONTROL::SetProcessAllPosition ( 4, 100 );
//
//	Initialize ();
//
//	if ( NS_PATCH_THREAD::IsForceTerminate () )
//	{
//		goto LFail;
//	}
//
//	//	파일 리스트 비교 및 새 목록 작성
//	NS_LOG_CONTROL::Write ( IDS_MESSAGE_053 );
//	if ( !LoadList () )
//	{
//		NS_LOG_CONTROL::Write ( IDS_MESSAGE_054 );	
//		goto LFail;
//	}
//
//	if ( NS_PATCH_THREAD::IsForceTerminate () )
//	{
//		goto LFail;
//	}
//	
//	NS_LOG_CONTROL::Write ( IDS_MESSAGE_057 );	
//	if ( !MakeNewList ( cPatchVer, sPatchVer, cGameVer, sGameVer ) )
//	{
//		NS_LOG_CONTROL::Write ( IDS_MESSAGE_058 );		
//		goto LFail;
//	}    
//
//	NS_LOG_CONTROL::SetProcessAllPosition ( 10, 100 );
//
//	if ( NS_PATCH_THREAD::IsForceTerminate () )
//	{
//		goto LFail;
//	}
//
//	//	새 목록에 나온 파일 다운 로드
//	str = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp;
//	CreateDirectory ( str.GetString (), NULL );
//	if ( !DownloadFilesByFtp ( pFtpPatch ) )
//	{			
//		if ( !NS_PATCH_THREAD::IsForceTerminate () )
//		{
//			NS_LOG_CONTROL::Write ( IDS_MESSAGE_055 );
//		}
//		goto LFail;
//	}
//	NS_LOG_CONTROL::SetProcessAllPosition ( 70, 100 );
//
//	if ( NS_PATCH_THREAD::IsForceTerminate () )
//	{
//		goto LFail;
//	}
//
//	//	파일 복사
//	if ( !Installing() )
//	{	
//		//	강제 종료가 아닌 진짜 오류
//		//
//		if ( NS_PATCH_THREAD::IsExtractError() )
//		{
//			NS_LOG_CONTROL::Write ( IDS_MESSAGE_059 );
//
//			//	Note : DS list 삭제
//			//
//			strTemp.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::strDownList );
//			DeleteFile ( strTemp.GetString () );			
//		}
//
//		goto LFail;
//	}
//	NS_LOG_CONTROL::SetProcessAllPosition ( 90, 100 );
//
//    //	<--	클라이언트 목록를 서버목록으로 교체	-->	//
//	cFileList.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::g_szClientFileList );
//	sFileList.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::g_szServerFileList );	
//	
//	DeleteFile ( cFileList.GetString () );
//	MoveFile ( sFileList.GetString (), cFileList.GetString () );
//
//	//	Note : DS list 삭제
//	//	
//	strTemp.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::strDownList );
//	DeleteFile ( strTemp.GetString () );
//	
//	sFileList.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::strServerCabFileList );
//	DeleteFile ( sFileList.GetString () );
//
//	DeleteDownFiles();
//	DeleteNotFoundFile();
//	
//	str = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp;
//	RemoveDirectory ( str.GetString () );
//	
//    Destroy ();	
//	
//	NS_LOG_CONTROL::Write ( IDS_MESSAGE_060 );
//	if ( !VersionUp ( sGameVer ) )
//	{
//		NS_LOG_CONTROL::Write ( IDS_MESSAGE_061 );
//		goto LFail;
//	}
//	
//	NS_LOG_CONTROL::Write ( IDS_MESSAGE_062 );
//	NS_LOG_CONTROL::SetProcessAllPosition ( 100, 100 );
//
//	return 0;
//
//LFail:
//	//	다운로드할 파일들이 손상되어 실패한 경우에는
//	//	리스트를 처음부터 다시 작성해야하기때문에
//	//	리스트를 저장하지 않는다.
//	if ( NS_PATCH_THREAD::IsExtractError () )
//	{
//		if ( NS_PATCH_THREAD::IsForceTerminate () )
//		{
//			SaveDownList ( sGameVer );
//		}
//	}
//	else
//	{
//		SaveDownList ( sGameVer );
//	}
//
//	sFileList.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::g_szServerFileList );
//	DeleteFile ( sFileList.GetString () );
//	sFileList.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::strServerCabFileList );
//	DeleteFile ( sFileList.GetString () );	
//
////	DeleteDownFiles ();
//    Destroy ();
//
////	str.Format ( "%s%s", strAppPath.GetString (), g_szDownloadTemp );
////	RemoveDirectory ( str.GetString () );
//
//	sFileList.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::g_szServerFileList );	
//	DeleteFile ( sFileList.GetString () );
//	
//	NS_PATCH_THREAD::SetFail ();
//
//	return 0;
//}

DWORD	PatchByHTTP ( S_PATCH_THREAD_PARAM* pParam )
{
	CPatch*		pFtpPatch = pParam->pFtpPatch;
	CHttpPatch* pHttpPatch = pParam->pHttpPatch;
	const bool bUseHttp = pParam->bUseHttp;
	const int cPatchVer = pParam->cPatchVer;
	const int sPatchVer = pParam->sPatchVer;
	const int cGameVer = pParam->cGameVer;
	const int sGameVer = pParam->sGameVer;	
	
	CString cFileList, sFileList, str, strTemp;
	BOOL bInstalling(FALSE);

	NS_LOG_CONTROL::SetProcessAllPosition ( 0, 100 );

	//	<--	'강제종료'	-->	//
	if ( NS_PATCH_THREAD::IsForceTerminate () ) return 0;
	
	//	파일 리스트 다운로드
	NS_LOG_CONTROL::Write ( IDS_MESSAGE_049 ); // 리스트 전송중
	if ( !GETFILE_USEHTTP ( pHttpPatch, "\\", NS_GLOBAL_VAR::strServerCabFileList.GetString(), "" ) )
	{	
		if ( !NS_PATCH_THREAD::IsForceTerminate () )
		{
			NS_LOG_CONTROL::Write ( IDS_MESSAGE_050 ); // 리스트 전송중 오류
		}

		goto LFail;
	}

	//	<--	'강제종료'	-->	//
	if ( NS_PATCH_THREAD::IsForceTerminate () ) goto LFail;
	
	//	압축 풀기위한 전체 경로 설정 및 압축 풀기&설치
	str = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strServerCabFileList;
	NS_LOG_CONTROL::Write ( IDS_MESSAGE_051 ); // 리스트 압축 풀고 있음
	if ( !Extract ( NS_GLOBAL_VAR::strAppPath.GetString (), str.GetString() ) )
	{
		NS_LOG_CONTROL::Write ( IDS_MESSAGE_052 ); // 리스트 압축풀기 실패
		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 4, 100 );

	//Initialize (); // MEMO : 함수 구현 안됨.

	//	<--	'강제종료'	-->	//
	if ( NS_PATCH_THREAD::IsForceTerminate () ) goto LFail;

	//	파일 리스트 비교 및 새 목록 작성
	NS_LOG_CONTROL::Write ( IDS_MESSAGE_053 ); // 패치 파일 리스트 로드
	if ( !LoadList () )
	{
		NS_LOG_CONTROL::Write ( IDS_MESSAGE_054 ); // 리스트 로드에 실패
		goto LFail;
	}

	//	<--	'강제종료'	-->	//
	if ( NS_PATCH_THREAD::IsForceTerminate () ) goto LFail;
	
	NS_LOG_CONTROL::Write ( IDS_MESSAGE_057 ); // 패치 파일 리스트 비교 진행
	if ( !MakeNewList ( cPatchVer, sPatchVer, cGameVer, sGameVer ) )
	{
		NS_LOG_CONTROL::Write ( IDS_MESSAGE_058 ); // 리스트 비교에 실패
		goto LFail;
	}    

	NS_LOG_CONTROL::SetProcessAllPosition ( 10, 100 );

	//	<--	'강제종료'	-->	//
	if ( NS_PATCH_THREAD::IsForceTerminate () ) goto LFail;
	
	//	새 목록에 나온 파일 다운 로드
	str = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp;
	CreateDirectory ( str.GetString (), NULL );
	if ( !DownloadFilesByHttp ( pHttpPatch ) )
	{
		if ( !NS_PATCH_THREAD::IsForceTerminate () )
		{
			NS_LOG_CONTROL::Write ( IDS_MESSAGE_055 ); // 패치 파일 전송중 오류
		}

		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 70, 100 );

	//	<--	'강제종료'	-->	//
	if ( NS_PATCH_THREAD::IsForceTerminate () ) goto LFail;

	//	파일 복사
	if ( !Installing() )
	{	
		//	강제 종료가 아닌 진짜 오류
		//
		if ( NS_PATCH_THREAD::IsExtractError() )
		{
			NS_LOG_CONTROL::Write ( IDS_MESSAGE_059 ); // 패치 파일중 일부 손상

			//	Note : DS list 삭제
			//
			CString strDownList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownList;
			DeleteFile ( strDownList.GetString() );

			// MEMO : 카피 리스트로 비교후에 설치된 파일은 다운로드 받지 않아도 된다.
			//
			CString strCopyList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strCopyList;
			CopyFile( strCopyList.GetString(), strDownList.GetString(), FALSE );
		}

		bInstalling = TRUE; // 설치중이다.

		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 90, 100 );

	//CDebugSet::ToLogFile ( "시작" );
	//NS_LOG_CONTROL::Write ( "게임 진행에 필요한 파일 무결성을 검사합니다." );
	//ULONGLONG ulServerFile = (ULONGLONG)g_vectorServerFile.size ();
	//NS_LOG_CONTROL::SetProcessCurPosition ( 0, ulServerFile );
	//if ( !CheckIntegrity ( NS_GLOBAL_VAR::strAppPath ) )
	//{
	//	if ( !NS_PATCH_THREAD::IsForceTerminate () )
	//	{
	//		NS_LOG_CONTROL::Write ( "게임 진행에 필요한 파일중 일부가 손상되었습니다." );
	//	}

	//	goto LFail;
	//}
	//NS_LOG_CONTROL::SetProcessCurPosition ( ulServerFile, ulServerFile );
	//CDebugSet::ToLogFile ( "끝" );

    //	<--	클라이언트 목록를 서버목록으로 교체	-->	//
	cFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szClientFileList;
	sFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szServerFileList;
	
	DeleteFile ( cFileList.GetString () );
	MoveFile ( sFileList.GetString (), cFileList.GetString () );

	//	Note : DS list 삭제
	//	
	strTemp = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownList;
	DeleteFile ( strTemp.GetString () );

	// Note : 카피 리스트 삭제
	//
	strTemp = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strCopyList;
	DeleteFile ( strTemp.GetString () );
	
	sFileList.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::strServerCabFileList );
	DeleteFile ( sFileList.GetString () );

	DeleteDownFiles();
	DeleteNotFoundFile();
	
	str = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp;
	RemoveDirectory ( str.GetString () );
	
    Destroy ();	
	
	NS_LOG_CONTROL::Write ( IDS_MESSAGE_060 ); // 클라이언트 버전 갱신
	if ( !VersionUp ( sGameVer ) )
	{
		NS_LOG_CONTROL::Write ( IDS_MESSAGE_061 ); // 클라이언트 버전 갱신 실패
		goto LFail;
	}
	
	NS_LOG_CONTROL::Write ( IDS_MESSAGE_062 ); // 모든 패치 성공
	NS_LOG_CONTROL::SetProcessAllPosition ( 100, 100 );

	return 0;

LFail:
	//	다운로드할 파일들이 손상되어 실패한 경우에는
	//	리스트를 처음부터 다시 작성해야하기때문에
	//	리스트를 저장하지 않는다.
	if ( NS_PATCH_THREAD::IsExtractError () )
	{
		if ( NS_PATCH_THREAD::IsForceTerminate () )
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
	DeleteFile ( sFileList.GetString () );
	sFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strServerCabFileList;
	DeleteFile ( sFileList.GetString () );	

//	DeleteDownFiles ();
    Destroy ();

//	str.Format ( "%s%s", strAppPath.GetString (), g_szDownloadTemp );
//	RemoveDirectory ( str.GetString () );

	sFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szServerFileList;
	DeleteFile ( sFileList.GetString () );
	
	NS_PATCH_THREAD::SetFail ();

	return 0;
}
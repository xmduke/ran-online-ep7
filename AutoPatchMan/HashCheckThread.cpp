#include "StdAfx.h"
#include "./HashCheckThread.h"
#include "./Resource.h"

#include "./LogControl.h"
#include "../EngineLib/Common/StringUtils.h"
#include "../MfcExLib/ExLibs/CompactFdi.h"
#include "../EngineLib/Hash/CHash.h"
#include "LauncherText.h"
#include "../Common/SUBPATH.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CHashCheckThread, CHttpThread)

CHashCheckThread::CHashCheckThread( LPVOID pParam, DWORD nThreadID )
{
	VERIFY( pParam != NULL);
	m_pPatchThreadParam = (S_PATCH_THREAD_PARAM *)pParam;

	m_nDlgThreadID = nThreadID;
	m_sFailGameVer = 0;
}

CHashCheckThread::~CHashCheckThread()
{
	for ( int i = 0; i < (int)m_vectorClientFile.size (); i++ )
	{
		delete m_vectorClientFile[i];
	}
	m_vectorClientFile.clear();

	m_vecHashFile.clear();
	m_vecHashFailFile.clear();
	m_vecHashDownFile.clear();
}

void CHashCheckThread::ThreadMain()
{
	CHttpPatch* pHttpPatch	= m_pPatchThreadParam->pHttpPatch;

	CString sFileList, str;

	// 파일 검사를 시작합니다.
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 65 ), 0 );

	NS_LOG_CONTROL::SetProcessAllPosition ( 0, 100 );

	if ( IsForceTerminate() ) return ;

	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 53 ), 0 );
	if( !LoadListHash() )
	{
		// 리스트 로드에 실패
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 54 ), 0 );
		goto LFail;
	}

	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 67 ), 0 );
	if( !CreateHashFile() ) // 해쉬 검사에 필요한 타겟이 되는 리스트 생성
	{
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 68 ), 0 );
		goto LFail;
	}    

	NS_LOG_CONTROL::SetProcessAllPosition ( 10, 100 );

	if ( IsForceTerminate() ) goto LFail;

	if( !CheckHashFile() )
	{
		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 40, 100 );

	// 임시 폴더(Temp) 생성

	str = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strDownloadTemp;
	CreateDirectory ( str.GetString(), NULL );

	if( !DownloadHashFailFiles( pHttpPatch ) )
	{
		if( !IsForceTerminate() )
		{
			// 패치 파일 전송중 오류
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 55 ), 0 );
		}

		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 70, 100 );

	if( !InstallingHashFailFiles() ) //	파일 복사
	{	
		if ( IsExtractError() ) // 강제 종료가 아닌 진짜 오류
		{
			// 패치 파일중 일부 손상
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 59 ), 0 );
		}

		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 90, 100 );

	// 파일 검사를 종료합니다.
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 66 ), 0 );

	DeleteDownFiles();

	NS_LOG_CONTROL::SetProcessAllPosition ( 100, 100 );

	return ;

LFail:
	DeleteDownFiles();
	SetFail();
}

BOOL CHashCheckThread::LoadListHash()
{
	CString str;
	str.Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), NS_GLOBAL_VAR::g_szClientFileList );

	FILE* fp = fopen( str.GetString(), "rb" );
	if( !fp ) return FALSE;

	SFILENODE FileInfo;

	// 첫번째 버전 정보
	if( 1 != fread ( &FileInfo, VER_1_OFFSET, 1, fp ) )
	{
		fclose ( fp );
		return FALSE;
	}

	// 버젼 옵셋 설정
	INT nVerOffset(0);

	switch( FileInfo.Ver )
	{
	case 1: nVerOffset = VER_1_OFFSET; break;
	case 2: nVerOffset = VER_2_OFFSET; break;
	default:
		::AfxMessageBox( _T("[ERROR]:Filelist is different!") );
		return FALSE;
	}

	// 현재 버젼의 옵셋 만큼 파일 포지션을 뒤로 이동한다.
	LONG lOffset = nVerOffset - VER_1_OFFSET;

	if( fseek( fp, lOffset, SEEK_CUR) )
	{
		fclose ( fp );
		return FALSE;
	}

	// 두번째 파일 개수
	if( 1 != fread ( &FileInfo, nVerOffset, 1, fp ) )
	{
		fclose ( fp );
		return FALSE;
	}

	for( int i = 0; i < FileInfo.Ver; i++ )
	{
		SFILENODE* pNewFile = new SFILENODE;
		if( 1 != fread( pNewFile, nVerOffset, 1, fp ) )
		{
			fclose ( fp );
			return FALSE;
		}

		m_vectorClientFile.push_back( pNewFile );
	}

	fclose ( fp );

	return TRUE;
}

BOOL CHashCheckThread::CreateHashFile()
{
	NS_LOG_CONTROL::SetProcessCurPosition ( 0, 1 );
	{
		FILEVECTOR::size_type nFileSize = m_vectorClientFile.size ();
		for ( FILEVECTOR::size_type i = 0; i < nFileSize; i++ )
		{	
			SFILENODE* pGetFile = m_vectorClientFile[i];

			// 검사에 필요한 폴더를 파일로 빼서 서버에 가지고 있으면 좋겠다.(광수)
			if( ( !_tcscmp( pGetFile->SubPath, _T("\\") ) && 
				  ( _tcscmp( pGetFile->FileName, _T("param.ini.cab") ) &&
				    _tcscmp( pGetFile->FileName, _T("option.ini.cab") ) ) ) ||
				!_tcscmp( pGetFile->SubPath, _T("\\data\\glogic\\") ) ||
				!_tcscmp( pGetFile->SubPath, _T("\\data\\glogic\\npctalk\\") ) ||
				!_tcscmp( pGetFile->SubPath, _T("\\data\\glogic\\quest\\") ) )
			{
				m_vecHashFile.push_back( pGetFile );
			}

			NS_LOG_CONTROL::SetProcessCurPosition ( i, nFileSize );

			if ( IsForceTerminate() )	return FALSE;
		}
	}
	NS_LOG_CONTROL::SetProcessCurPosition ( 1, 1 );

	return TRUE;
}

BOOL CHashCheckThread::CheckHashFile()
{
	HASH::CHash g_hashObj;
	int nCheckCount = 0;

	g_hashObj.SetHashAlgorithm( HASH::MD5 ); // MD5 로 설정
	g_hashObj.SetHashOperation( HASH::FILE_HASH ); // 파일 해쉬로 설정
	g_hashObj.SetHashFormat( HASH::UPPERCASE_NOSPACES );

	FILEVECTOR::size_type nVecSize = m_vecHashFile.size();
	for( FILEVECTOR::size_type i = 0; i<nVecSize; i++ )
	{
		NS_LOG_CONTROL::SetProcessCurPosition ( 0, 1 );
		{
			SFILENODE* pFile = m_vecHashFile[i];

			CString	DestFile;
			DestFile.Format( "%s%s%s", NS_GLOBAL_VAR::strAppPath, pFile->SubPath, pFile->FileName );
			DestFile = DestFile.Left( DestFile.ReverseFind( '.' ) );

			g_hashObj.SetHashFile( DestFile.GetString() );
			CString strHash = g_hashObj.DoHash();

			CString	strMsg;
			strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 69 ) ;

			CHAR * szListMessage = new CHAR[256]; 
			wsprintf( szListMessage, "%s %s", strMsg.GetString(), pFile->FileName );
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );

			ULONGLONG TotalPos = 10 + (++nCheckCount * 30) / nVecSize;
			NS_LOG_CONTROL::SetProcessAllPosition ( TotalPos, 100 );

			if( strHash != pFile->szMD5 ) // 해쉬비교
			{
				// 새로 다운 받을 목록에 저장
				m_vecHashFailFile.push_back( pFile );
			}
		}
		NS_LOG_CONTROL::SetProcessCurPosition ( 1, 1 );

		if( IsForceTerminate() ) return FALSE;

		Sleep( 0 );
	}

	return TRUE;
}

BOOL CHashCheckThread::DownloadHashFailFiles( CHttpPatch* pHttpPatch )
{
	int DownCount = 0;

	CString	DownloadDir( NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT  + NS_GLOBAL_VAR::strDownloadTemp );

	int nVectorSize = (int)m_vecHashFailFile.size();
	for( int i = 0; i < nVectorSize; ++i )
	{
		SFILENODE* pNewFile = m_vecHashFailFile[i];		

		if ( IsForceTerminate() ) return FALSE;

		CString	strMsg;
		strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 47 );

		CHAR * szListMessage = new CHAR[256]; 
		wsprintf( szListMessage, "%s %s", strMsg.GetString(), pNewFile->FileName );
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );

		ULONGLONG TotalPos = 40 + (++DownCount * 30) / nVectorSize;			
		NS_LOG_CONTROL::SetProcessAllPosition( TotalPos, 100 );

		CString FullSubPath = pNewFile->SubPath;

		if ( !GETFILE_USEHTTP ( pHttpPatch, FullSubPath.GetString(), pNewFile->FileName ) )
			return FALSE;

		m_vecHashDownFile.push_back( pNewFile );

		if( IsForceTerminate() ) return FALSE;

		Sleep( 0 ); // 파일받다가 멈추는것 방지용
	}

	return TRUE;
}

BOOL CHashCheckThread::InstallingHashFailFiles()
{
	CString strTemp;
	int InstallCount = 0;

	int nVectorSize = (int)m_vecHashDownFile.size();
	for ( int i = 0; i < nVectorSize; ++i )
	{
		SFILENODE* pNewFile = m_vecHashDownFile[i];	

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
			CString	strMsg;
			strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 48 );

			CHAR * szListMessage = new CHAR[256];
			wsprintf( szListMessage, "%s %s", strMsg.GetString(), pNewFile->FileName );
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );

			ULONGLONG TotalPos = 70 + (++InstallCount * 20) / nVectorSize;
			NS_LOG_CONTROL::SetProcessAllPosition ( TotalPos, 100 );

			CString	DownloadDir;
			DownloadDir = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + 
				          NS_GLOBAL_VAR::strDownloadTemp + pNewFile->FileName;		

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
		NS_LOG_CONTROL::SetProcessCurPosition ( 1, 1 );

		if( IsForceTerminate() ) return FALSE;

		Sleep( 0 );
	}

	return TRUE;
}

BOOL CHashCheckThread::DeleteDownFiles()
{
	CString	strTemp;
	int nCnt = static_cast<int>( m_vecHashDownFile.size() );

	for ( int i = 0; i < nCnt; ++i )
	{
		SFILENODE* pNewFile = m_vecHashDownFile[i];
		strTemp.Format( "%s%s%s%s", NS_GLOBAL_VAR::strProFile.GetString(),  SUBPATH::SAVE_ROOT  , NS_GLOBAL_VAR::strDownloadTemp.GetString(), pNewFile->FileName );

		DeleteFile( strTemp.GetString() );
	}

	strTemp = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strDownloadTemp;
	RemoveDirectory ( strTemp.GetString() );

	return TRUE;
}

BEGIN_MESSAGE_MAP(CHashCheckThread, CHttpThread)
END_MESSAGE_MAP()
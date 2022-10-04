#include "StdAfx.h"
#include ".\httppatchthread.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CHttpPatchThread::SaveDownList( int sGameVer )
{
	CString strTemp;
	strTemp = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownList;

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

		FileInfo.Ver = (int) m_vectorDownFile.size ();
		if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
		{
			GASSERT ( 0 && "다운 완료 파일 쓰기에 실패했습니다." );
			return FALSE;
		}

		for ( int i = 0; i < (int)m_vectorDownFile.size (); i++ )
		{
			SFILENODE* pNewFile = m_vectorDownFile[i];

			if ( 1 != fwrite ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
			{
				GASSERT ( 0 && "다운 완료 파일 쓰기에 실패했습니다." );
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CHttpPatchThread::LoadDownList()
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
		m_sFailGameVer = FileInfo.Ver;

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

			m_mapDownedFile.insert ( std::make_pair(std::string(pNewFile->FileName), pNewFile) );
		}
		fclose ( fp );
	}

	return TRUE;
}

BOOL CHttpPatchThread::SaveCopyList( int sGameVer )
{
	CString strTemp;
	strTemp = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strCopyList;

	FILE* fp = fopen ( strTemp, "wb" );
	if ( fp )
	{
		SFILENODE FileInfo;

		FileInfo.Ver = sGameVer;
		if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
		{
			GASSERT ( 0 && "복사 완료 파일 쓰기에 실패했습니다." );
			return FALSE;
		}

		FileInfo.Ver = (int) m_vectorCopyFile.size ();
		if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
		{
			GASSERT ( 0 && "복사 완료 파일 쓰기에 실패했습니다." );
			return FALSE;
		}

		for ( int i = 0; i < (int)m_vectorCopyFile.size (); i++ )
		{
			SFILENODE* pNewFile = m_vectorCopyFile[i];

			if ( 1 != fwrite ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
			{
				GASSERT ( 0 && "복사 완료 파일 쓰기에 실패했습니다." );
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CHttpPatchThread::LoadCopyList()
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
		m_sFailGameVer = FileInfo.Ver;

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

			m_mapCopiedFile.insert ( std::make_pair(std::string(pNewFile->FileName), pNewFile) );
		}
		fclose ( fp );
	}

	return TRUE;
}
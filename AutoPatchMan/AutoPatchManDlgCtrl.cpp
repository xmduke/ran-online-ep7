#include "stdafx.h"
#include <algorithm>

#include "AutoPatchMan.h"
#include "AutoPatchManDlg.h"

#include "GlobalVariable.h"
#include "LogControl.h"
#include "RANPARAM.h"
#include "LauncherText.h"
#include "../Common/SUBPATH.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void	CAutoPatchManDlg::UpdateControls()
{
	static const ULONGLONG UL100 = 100UL;
	static const ULONGLONG UL1 = 1UL;

	ULONGLONG Pos = 0;
	ULONGLONG End = 0;
	
	int	CurPercent	= 0;
	int	AllPercent= 0;	
	
	NS_LOG_CONTROL::GetProcessCurPosition ( &Pos, &End );
	End = ( !End ) ? UL1 : End;
	CurPercent = int( (Pos*UL100) / End );	

	NS_LOG_CONTROL::GetProcessAllPosition ( &Pos, &End );	
	End = ( !End )? UL1 : End;
	AllPercent = int( (Pos*UL100) / End);

	m_ctrlProgressCur.SetPos ( CurPercent );
	m_ctrlProgressAll.SetPos ( AllPercent );
}

void CAutoPatchManDlg::ListAddString( const CString & strLog )
{
	CString strLog_COPY( strLog );
	if ( strLog_COPY[strLog_COPY.GetLength()-1] == '\n' )
	{
		strLog_COPY = strLog_COPY.Left ( strLog_COPY.GetLength()-1 );
		if ( strLog_COPY[strLog_COPY.GetLength()-1] == '\r' )
		{
			strLog_COPY = strLog_COPY.Left ( strLog_COPY.GetLength()-1 );
		}
	}

	int nIndex = m_ctrlListBox.AddString( strLog_COPY );
	m_ctrlListBox.SetCurSel( nIndex );
}

void CAutoPatchManDlg::ListAddString( UINT nIDS )
{
	CString strLog_COPY;
	strLog_COPY.LoadString( nIDS );

	int nIndex = m_ctrlListBox.AddString ( strLog_COPY );
	m_ctrlListBox.SetCurSel ( nIndex );
}

void CAutoPatchManDlg::ListAddString( const PCHAR szMessage )
{
	GASSERT( szMessage );

	CString strLog_COPY( szMessage );
	if ( strLog_COPY[strLog_COPY.GetLength()-1] == '\n' )
	{
		strLog_COPY = strLog_COPY.Left ( strLog_COPY.GetLength()-1 );
		if ( strLog_COPY[strLog_COPY.GetLength()-1] == '\r' )
		{
			strLog_COPY = strLog_COPY.Left ( strLog_COPY.GetLength()-1 );
		}
	}

	int nIndex = m_ctrlListBox.AddString( strLog_COPY );
	m_ctrlListBox.SetCurSel( nIndex );
}

void CAutoPatchManDlg::FILEOUT( const CString& strLogFile )
{
	FILE* fp = fopen( strLogFile, "wt" );
	if ( fp )
	{
		CString strTemp;

		int nCnt = m_ctrlListBox.GetCount();
		for(int i = 0; i < nCnt; ++i )
		{
			m_ctrlListBox.GetText( i, strTemp );
			fprintf ( fp, "%s\r\n", strTemp.GetString() );
		}

		fclose ( fp );
	}
}

BOOL CAutoPatchManDlg::GETFILE_USEHTTP( CHttpPatch* pHttpPatch, std::string strRemoteSubPath, std::string strFileName, CString strTempDir )
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

	std::string strSubPath( strRemoteSubPath );

	//	'\\'문자를 '/'로 변경한다.
	std::replace ( strSubPath.begin(), strSubPath.end(), '\\', '/' );

	#if !defined(_DEBUG) && !defined(KR_PARAM) && !defined(GS_PARAM)
	{
		CString strFolder;
		strFolder.Format( "/%04d", m_sGameVer );
		strFolder += strSubPath.c_str();
		strSubPath = strFolder;
	}
	#endif

	strSubPath += strFileName;

	CHAR * szListMessage = new CHAR[MAX_PATH];
	StringCchCopy( szListMessage, MAX_PATH, strSubPath.c_str() );
	PostMessage( WM_LISTADDSTRING, 0, (LPARAM)szListMessage );

	std::string strLocalFullPath;
	strLocalFullPath += NS_GLOBAL_VAR::strProFile.GetString();	
	strLocalFullPath += SUBPATH::SAVE_ROOT;
	strLocalFullPath += strTempDir.GetString();
	strLocalFullPath += strFileName;


	static int nTRY = 0;
	int nTRY_FILESIZE_CHECK = 0;
	int nERROR_RETRY = 0;
	int nADDRESS_NULL_COUNT = 0;

	while ( nTRY_FILESIZE_CHECK < 3 )
	{
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

		CHAR * szListMessage = new CHAR[MAX_PATH];
		StringCchCopy( szListMessage, MAX_PATH, strHttpAddress );
		PostMessage( WM_LISTADDSTRING, 0, (LPARAM)szListMessage );

		if ( NET_ERROR == pHttpPatch->SetBaseURL ( strHttpAddress ) )
		{			
			//NS_LOG_CONTROL::Write ( pHttpPatch->GetErrMsg() );
			++nTRY;
			++nERROR_RETRY;
			continue;
		}

		if ( NET_ERROR == pHttpPatch->GetFile ( strSubPath.c_str(), strLocalFullPath.c_str() ) )
		{
			CString	strTemp, strMsg;
			strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 47 );
			strTemp.Format ( "%s %s", strMsg.GetString(), strFileName.c_str() );
			ListAddString( strTemp );

			++nTRY;
			++nERROR_RETRY;
			continue;
		}

		ULONGLONG ulRECEIVED, ulTOTALSIZE;
		NS_LOG_CONTROL::GetProcessCurPosition ( &ulRECEIVED, &ulTOTALSIZE );

		if ( ulRECEIVED != ulTOTALSIZE )
		{
			++nTRY_FILESIZE_CHECK;
			ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 46 ) ); // 전송 받는 파일 크기가 맞지 않다.
			continue;
		}
		else
		{
			return TRUE; //	NET_OK
		}        

		Sleep( 0 );
	}

	return FALSE;
}
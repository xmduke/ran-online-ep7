#include "stdafx.h"
#include "LogControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace	NS_LOG_CONTROL
{
	//CListBox*	pCtrlLog = NULL;
	////
	//void	SetListBoxCtrl ( CListBox* pListBox )
	//{
	//	GASSERT ( pListBox && "pListBox is NULL." );
	//	pCtrlLog = pListBox;
	//}

	//void	Write ( const CString & strLog )
	//{
	//	if ( !pCtrlLog )
	//	{
	//		GASSERT ( 0 && "pCtrlLog is NULL." );
	//		return ;
	//	}

	//	CString strLog_COPY = strLog;
	//	if ( strLog_COPY[strLog_COPY.GetLength()-1] == '\n' )
	//	{
	//		strLog_COPY = strLog_COPY.Left ( strLog_COPY.GetLength()-1 );
	//		if ( strLog_COPY[strLog_COPY.GetLength()-1] == '\r' )
	//		{
	//			strLog_COPY = strLog_COPY.Left ( strLog_COPY.GetLength()-1 );
	//		}
	//	}

	//	//if ( bModify )
	//	//{
	//	//	int nLastIndex = pCtrlLog->GetCount () - 1;
	//	//	pCtrlLog->DeleteString ( nLastIndex );

	//	//	int nIndex = pCtrlLog->AddString ( strLog );
	//	//	pCtrlLog->SetCurSel ( nIndex );
	//	//}
	//	//else
	//	//{
	//		int nIndex = pCtrlLog->AddString ( strLog_COPY );
	//		pCtrlLog->SetCurSel ( nIndex );
	//	//}
	//	
	//	//vecLog.push_back( strLog_COPY );
	//}

	//void	Write ( UINT nIDS )
	//{
	//	if ( !pCtrlLog )
	//	{
	//		GASSERT ( 0 && "pCtrlLog is NULL." );
	//		return ;
	//	}

	//	CString strLog;
	//	strLog.LoadString( nIDS );

	//	int nIndex = pCtrlLog->AddString ( strLog );
	//	pCtrlLog->SetCurSel ( nIndex );

	//	//vecLog.push_back ( strLog );
	//}

	ULONGLONG	nCurPos = 0;
	ULONGLONG	nCurEnd = 0;
	ULONGLONG	nAllPos = 0;
	ULONGLONG	nAllEnd = 0;

    void	SetProcessCurPosition ( ULONGLONG  CurPos,  ULONGLONG  CurEnd )
	{
		nCurPos = CurPos;
		nCurEnd = CurEnd;
	}

	void	GetProcessCurPosition ( ULONGLONG* pCurPos, ULONGLONG* pCurEnd)
	{
		*pCurPos = nCurPos;
		*pCurEnd = nCurEnd;
	}

	void	GetProcessCurPositionAddr ( ULONGLONG** ppCurPos, ULONGLONG** ppCurEnd)
	{
		*ppCurPos = &nCurPos;
		*ppCurEnd = &nCurEnd;
	}

	void	SetProcessAllPosition ( ULONGLONG  AllPos,  ULONGLONG  AllEnd )
	{
		nAllPos = AllPos;
		nAllEnd = AllEnd;
	}

	void	GetProcessAllPosition ( ULONGLONG* pAllPos, ULONGLONG* pAllEnd)
	{
		*pAllPos = nAllPos;
		*pAllEnd = nAllEnd;
	}

	//void	FILEOUT ( const CString& strLogFile )
	//{
	//	FILE* fp = fopen ( strLogFile, "wt" );
	//	if ( fp )
	//	{
	//		//LOG_VECTOR_ITER iter = vecLog.begin ();
	//		//LOG_VECTOR_ITER iter_end = vecLog.end ();
	//		//for ( ; iter != iter_end; ++iter )
	//		CString strTemp;

	//		int nCnt = pCtrlLog->GetCount();
	//		for(int i = 0; i < nCnt; ++i )
	//		{
	//			pCtrlLog->GetText( i, strTemp );
	//			fprintf ( fp, "%s\r\n", strTemp.GetString() );
	//		}

	//		fclose ( fp );
	//	}
	//}
};

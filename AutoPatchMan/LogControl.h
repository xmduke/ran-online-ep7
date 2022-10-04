#pragma once

class	CListBox;

namespace	NS_LOG_CONTROL
{
	//void	SetListBoxCtrl ( CListBox* pListBox );
	//void	Write ( const CString & strLog );
	//void	Write ( UINT nIDS );

    void	SetProcessCurPosition ( ULONGLONG  CurPos,  ULONGLONG  CurEnd );
	void	GetProcessCurPosition ( ULONGLONG* pCurPos, ULONGLONG* pCurEnd);
	void	GetProcessCurPositionAddr ( ULONGLONG** ppCurPos, ULONGLONG** ppCurEnd);

	void	SetProcessAllPosition ( ULONGLONG  AllPos,  ULONGLONG  AllEnd );
	void	GetProcessAllPosition ( ULONGLONG* pAllPos, ULONGLONG* pAllEnd);

	//void	FILEOUT ( const CString& strLogFile );
};
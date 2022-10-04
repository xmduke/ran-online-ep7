#include "stdafx.h"
#include "LogControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace	NS_LOG_CONTROL
{
	ULONGLONG	nCurPos = 0;
	ULONGLONG	nCurEnd = 0;
	ULONGLONG	nAllPos = 0;
	ULONGLONG	nAllEnd = 0;

	CString		strFilePath = "";
	CString		strFileName = "";

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
};

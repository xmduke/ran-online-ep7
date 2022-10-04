#pragma once

namespace	NS_LOG_CONTROL
{
    void	SetProcessCurPosition ( ULONGLONG  CurPos,  ULONGLONG  CurEnd );
	void	GetProcessCurPosition ( ULONGLONG* pCurPos, ULONGLONG* pCurEnd);
	void	GetProcessCurPositionAddr ( ULONGLONG** ppCurPos, ULONGLONG** ppCurEnd);

	void	SetProcessAllPosition ( ULONGLONG  AllPos,  ULONGLONG  AllEnd );
	void	GetProcessAllPosition ( ULONGLONG* pAllPos, ULONGLONG* pAllEnd);
};
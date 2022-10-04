#pragma once

#include "fdi.h"

/*
 * Function prototypes 
 */
//	szPathName에는 타겟 디렉토리를 써야하고, szFileName에는 FullPathFileName을 줘야한다.

BOOL	Extract ( const char* szPathName, const char* szFileName );	
const char*	GetErrorMsg ();

void	SetCabForceTerminate ( BOOL* pForceTerminate );
void	SetCabProgressValue ( ULONGLONG* pCur, ULONGLONG* pEnd );
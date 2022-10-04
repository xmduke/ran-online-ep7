#include "StdAfx.h"
#include ".\mintimer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

minTimer::minTimer(void)
{
}

minTimer::~minTimer(void)
{	
}

/** 
* rdtsc 는 팬티엄/AMD 는 지원
* 다른 cpu 에서 지원하지 않아 삭제
* CPU 클락이 변하면 값이 변해서 사용이 힘듬
*/
/*
DWORD minTimer::getRDTSCount()
{
	DWORD dwHi, dwLow;

	__asm
	{
		rdtsc              ; 클럭카운트를 edx:eax레지스터에 저장
		mov	dwHi, edx      ; 상위 dword 메모리에 백업
		mov dwLow, eax     ; 하위 dword 메모리에 백업
	}
	return MAKEULONG(dwLow, dwHi);
}
*/
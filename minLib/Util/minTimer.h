#pragma once

class minTimer
{
public:
	minTimer(void);
	~minTimer(void);

    /** 
	* rdtsc 는 팬티엄/AMD 는 지원
	* 다른 cpu 에서 지원하지 않아 삭제
	* CPU 클락이 변하면 값이 변해서 사용이 힘듬
	*/
	// DWORD getRDTSCount();
};
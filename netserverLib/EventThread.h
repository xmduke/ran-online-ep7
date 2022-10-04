#ifndef _EVENT_THREAD_H_
#define _EVENT_THREAD_H_

#pragma once

#include "s_CLock.h"

class EventThread : public CLock
{
public:
	EventThread(void);
	virtual ~EventThread(void);

protected:
	DWORD	m_dwThreadID;	//! 쓰래드 ID
	HANDLE	m_hThread;		//! 쓰래드 핸들
	HANDLE	m_hEventQuit;	//! 쓰래드 종료 이벤트
	HANDLE	m_hEventWork;	//! 쓰래드 work 이벤트
	bool    m_bThreadRun;   //! 쓰래드가 실행중인가?
public:
	//! -----------------------------------------------------------------------
	//! Virtual Function	
	virtual void process() = 0;
	virtual void endThread();

public:
	void threadFunc();

protected:
	bool startThread();
	void runThread();	

}; // class EventThread

#endif // _EVENT_THREAD_H_
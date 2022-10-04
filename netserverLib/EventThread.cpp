#include "StdAfx.h"
#include <process.h>
#include "./EventThread.h"

EventThread::EventThread(void)
	: m_dwThreadID(UINT_MAX)
	, m_hThread(NULL)	
	, m_bThreadRun(false)
{	
	m_hEventQuit = ::CreateEvent( 0, FALSE, FALSE, 0 );
	m_hEventWork = ::CreateEvent( 0, FALSE, FALSE, 0 );
	startThread();
}

EventThread::~EventThread(void)
{
	endThread();
}

void EventThread::runThread()
{
	::SetEvent(m_hEventWork);
}

void EventThread::endThread()
{
	if (m_hThread != NULL)
	{
		::SetEvent( m_hEventQuit );
		::WaitForSingleObject( m_hThread, INFINITE );
		m_hThread = NULL;
	}

	CloseHandle( m_hEventWork );
	CloseHandle( m_hEventQuit );
}

unsigned int WINAPI extThreadFunc( void* pThread )
{
	if ( pThread != NULL )
	{
		EventThread* pClass = (EventThread*) pThread;
		pClass->threadFunc();
		return 1;
	}
	else
	{
		return 0;
	}
}

bool EventThread::startThread()
{
	// Thread 생성	
	m_hThread = (HANDLE) ::_beginthreadex(
							NULL,
							0,
							extThreadFunc,
							this,
							0,
							(unsigned int*) &m_dwThreadID );
	if ( NULL == m_hThread )
	{
		return FALSE;
	}
	else 
	{
		m_bThreadRun = false;
		return TRUE;
	}
}

void EventThread::threadFunc()
{
	::ResetEvent( m_hEventQuit );
	::ResetEvent( m_hEventWork );

	HANDLE hEvents[] = { m_hEventQuit, m_hEventWork };

	DWORD dwReturn = 0;

	m_bThreadRun = true;

	DWORD dwTimeOut=100;

	while (m_bThreadRun)
	{		
		dwReturn = ::WaitForMultipleObjects(
						2, // // hEvents 가 2개의 이벤트이기 때문에...
						hEvents,
						FALSE,
						dwTimeOut );
						//INFINITE );

		switch (dwReturn)
		{
		case WAIT_OBJECT_0: // m_hEventQuit
			{
				m_bThreadRun = false;
				break;
			}
		case WAIT_OBJECT_0+1: // m_hEventWork
			{
				process();
				break;
			}
		case WAIT_TIMEOUT:
			{
				process();
				break;
			}
		default:
			{
				// Unknown Event, 알수없는 이벤트...
			}
			break;
		}		
	}
}
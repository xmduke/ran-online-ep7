#include "StdAfx.h"
#include <process.h>
#include "s_cxthread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace XThread;

CXThread::CXThread(int nPriority)
{
	m_hThread		= INVALID_HANDLE_VALUE;
	m_dwThreadID	= 0;
	m_bTerminate	= true;
	m_bSuspend		= true;
	m_bIsRunning	= false;
	m_nInitPriority = nPriority;
	m_hEvent		= ::CreateEvent(NULL,FALSE,FALSE,NULL);      
}

bool CXThread::Start()
{
	if (m_bTerminate)
	{
		m_hThread = (HANDLE) _beginthreadex(NULL, 0, _ThreadProc, (LPVOID)this, 0/* CREATE_SUSPENDED*/, &m_dwThreadID); 

		m_bTerminate = false;
		m_bSuspend = false;

		if (m_hThread != INVALID_HANDLE_VALUE)
			return true;
		return false;
	}
	else if (m_bSuspend)
	{
		DWORD nRet = ::ResumeThread(m_hThread);
		if (nRet == 0xFFFFFFFF)
			return false;
		m_bSuspend = false;
			return true;
	}
	return true;
}

bool CXThread::StartAndWait()
{
	bool bRet = Start();
	if (bRet)
		::WaitForSingleObject(m_hEvent,INFINITE);
	return bRet;
}

bool CXThread::Pause()
{
	if (m_bTerminate)
		return false;
	if (m_bSuspend)
		return true;
	DWORD nRet = ::SuspendThread(m_hThread);
	if (nRet == 0xFFFFFFFF)
		return false;
	m_bSuspend = true;
	return true;
}

bool CXThread::IsRunning()
{
	return m_bIsRunning;
}

bool CXThread::IsTerminated()
{
	return m_bTerminate;
}

bool CXThread::IsSuspend()
{
	return m_bSuspend;
}

void CXThread::Terminate()
{
	DWORD dwExitCode;
	m_bIsRunning = false;
	m_bTerminate = true;
	
	if (m_hThread != INVALID_HANDLE_VALUE)
	{
		::GetExitCodeThread(m_hThread, &dwExitCode);
		if (STILL_ACTIVE == dwExitCode)
		{
			::TerminateThread(m_hThread,dwExitCode);
			::CloseHandle(m_hThread);
			m_hThread = INVALID_HANDLE_VALUE;
		}
	}
}

void CXThread::Exit()
{
	DWORD dwExitCode;
	
	m_bIsRunning = false;
	m_bTerminate = true;

	if (m_hThread != INVALID_HANDLE_VALUE)
	{
		::GetExitCodeThread(m_hThread, &dwExitCode);
		if (STILL_ACTIVE == dwExitCode)
		{
			_endthreadex(dwExitCode);
			::ExitThread(dwExitCode);
			::CloseHandle(m_hThread);
			m_hThread = INVALID_HANDLE_VALUE;
		}
	}	
}

bool CXThread::WaitUntilTerminate(DWORD dwMiliSec)
{
	if ( ::WaitForSingleObject(m_hThread,dwMiliSec) == WAIT_TIMEOUT )
		return false;
	m_bIsRunning = false;
	m_bTerminate = true;
	return true;
}

void CXThread::SetPriority(int nLevel)
{
	::SetThreadPriority(m_hThread,nLevel);
}

int CXThread::GetPriority()
{
	return ::GetThreadPriority(m_hThread);
}

void CXThread::SpeedUp()
{
	int nOldLevel;
	int nNewLevel;

	nOldLevel = GetPriority();
	if (THREAD_PRIORITY_TIME_CRITICAL == nOldLevel)
		return;
	else if (THREAD_PRIORITY_HIGHEST == nOldLevel)
		nNewLevel = THREAD_PRIORITY_TIME_CRITICAL;
	else if (THREAD_PRIORITY_ABOVE_NORMAL == nOldLevel)
		nNewLevel = THREAD_PRIORITY_HIGHEST;
	else if (THREAD_PRIORITY_NORMAL == nOldLevel)
		nNewLevel = THREAD_PRIORITY_ABOVE_NORMAL;
	else if (THREAD_PRIORITY_BELOW_NORMAL == nOldLevel)
		nNewLevel = THREAD_PRIORITY_NORMAL;
	else if (THREAD_PRIORITY_LOWEST == nOldLevel)
		nNewLevel = THREAD_PRIORITY_BELOW_NORMAL;
	else if (THREAD_PRIORITY_IDLE == nOldLevel)
		nNewLevel = THREAD_PRIORITY_LOWEST;

	SetPriority(nNewLevel);
} 

void CXThread::SlowDown()
{
	int nOldLevel;
	int nNewLevel;

	nOldLevel = GetPriority();
	if (THREAD_PRIORITY_TIME_CRITICAL == nOldLevel)
		nNewLevel = THREAD_PRIORITY_HIGHEST;
	else if (THREAD_PRIORITY_HIGHEST == nOldLevel)
		nNewLevel = THREAD_PRIORITY_ABOVE_NORMAL;
	else if (THREAD_PRIORITY_ABOVE_NORMAL == nOldLevel)
		nNewLevel = THREAD_PRIORITY_NORMAL;
	else if (THREAD_PRIORITY_NORMAL == nOldLevel)
		nNewLevel = THREAD_PRIORITY_BELOW_NORMAL;
	else if (THREAD_PRIORITY_BELOW_NORMAL == nOldLevel)
		nNewLevel = THREAD_PRIORITY_LOWEST;
	else if (THREAD_PRIORITY_LOWEST == nOldLevel)
		nNewLevel = THREAD_PRIORITY_IDLE;
	else if (THREAD_PRIORITY_IDLE == nOldLevel)
		return;

	SetPriority(nNewLevel);
}

unsigned __stdcall CXThread::_ThreadProc(LPVOID lpParameter)
{
	CXThread* pThread = reinterpret_cast<CXThread*> (lpParameter);

	pThread->SetPriority(pThread->m_nInitPriority);
	pThread->m_bIsRunning = true;
	::SetEvent(pThread->m_hEvent);
	pThread->OnInitInstance();
	pThread->OnRunning();
	return pThread->OnExitInstance();
}

///////////////////////////////////////////////////////////////////////////////
//-- Begin TXThread Implementation --
template<typename T> 
TXThread<T>::TXThread(T& thObject,void (T::*pfnOnRunning)(), int nPriority):m_thObject(thObject), m_pfnOnRunning(pfnOnRunning)
{
	m_hThread		= NULL;
	m_dwThreadID	= 0;
	m_bTerminate	= true;
	m_bSuspend		= true;
	m_bIsRunning	= false;
	m_nInitPriority = nPriority;
	m_hEvent		= ::CreateEvent(NULL,FALSE,FALSE,NULL);      
}
  
template<typename T> 
bool TXThread<T>::Start()
{
	if (m_bTerminate)
	{
		m_hThread = (HANDLE) _beginthreadex(NULL, 0, _ThreadProc, (LPVOID)this, 0/* CREATE_SUSPENDED*/, &m_dwThreadID); 

		m_bTerminate = false;
		m_bSuspend = false;

		if (m_hThread != 0)
			return true;
		return false;
	}
	else if (m_bSuspend)
	{
		DWORD nRet = ::ResumeThread(m_hThread);
		if(nRet == 0xFFFFFFFF)
			return false;
		m_bSuspend = false;
		return true;
	}
	return true;
}

template<typename T> 
bool TXThread<T>::StartAndWait()
{
	bool bRet = Start();
	if (bRet)
		::WaitForSingleObject(m_hEvent,INFINITE);
	return bRet;
}

template<typename T> 
bool TXThread<T>::Pause()
{
	if(m_bTerminate)
		return false;
	if(m_bSuspend)
		return true;
	DWORD nRet = ::SuspendThread(m_hThread);
	if(nRet == 0xFFFFFFFF)
		return false;
	m_bSuspend = true;
	return true;
}

template<typename T> 
bool TXThread<T>::IsRunning()
{
	return m_bIsRunning;
}

template<typename T> 
bool TXThread<T>::IsTerminated()
{
	return m_bTerminate;
}

template<typename T> 
bool TXThread<T>::IsSuspend()
{
	return m_bSuspend;
}

template<typename T> 
void TXThread<T>::Terminate()
{
	DWORD dwExitCode;
	::GetExitCodeThread(m_hThread,&dwExitCode);
	if (STILL_ACTIVE == dwExitCode)
	{
		::TerminateThread(m_hThread,dwExitCode);
		::CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	m_bIsRunning = false;
	m_bTerminate = true;
}

template<typename T> 
void TXThread<T>::Exit()
{
	DWORD dwExitCode;
	::GetExitCodeThread(m_hThread,&dwExitCode);
	if (STILL_ACTIVE == dwExitCode)
	{
		_endthreadex(dwExitCode);
		::ExitThread(dwExitCode);
		::CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	m_bIsRunning = false;
	m_bTerminate = true;
}

template<typename T> 
bool TXThread<T>::WaitUntilTerminate(DWORD dwMiliSec)
{
	if(WaitForSingleObject(m_hThread,dwMiliSec) == WAIT_TIMEOUT)
		return false;
	m_bIsRunning = false;
	m_bTerminate = true;
	return true;
}

template<typename T> 
void TXThread<T>::SetPriority(int nLevel)
{
	::SetThreadPriority(m_hThread,nLevel); 
}

template<typename T> 
int TXThread<T>::GetPriority()
{
	return ::GetThreadPriority(m_hThread);
}

template<typename T> 
void TXThread<T>::SpeedUp()
{
	int nOldLevel;
	int nNewLevel;

	nOldLevel = GetPriority();
	if (THREAD_PRIORITY_TIME_CRITICAL == nOldLevel)
		return;
	else if (THREAD_PRIORITY_HIGHEST == nOldLevel)
		nNewLevel = THREAD_PRIORITY_TIME_CRITICAL;
	else if (THREAD_PRIORITY_ABOVE_NORMAL == nOldLevel)
		nNewLevel = THREAD_PRIORITY_HIGHEST;
	else if (THREAD_PRIORITY_NORMAL == nOldLevel)
		nNewLevel = THREAD_PRIORITY_ABOVE_NORMAL;
	else if (THREAD_PRIORITY_BELOW_NORMAL == nOldLevel)
		nNewLevel = THREAD_PRIORITY_NORMAL;
	else if (THREAD_PRIORITY_LOWEST == nOldLevel)
		nNewLevel = THREAD_PRIORITY_BELOW_NORMAL;
	else if (THREAD_PRIORITY_IDLE == nOldLevel)
		nNewLevel = THREAD_PRIORITY_LOWEST;

	SetPriority(nNewLevel);
} 

template<typename T> 
void TXThread<T>::SlowDown()
{
	int nOldLevel;
	int nNewLevel;

	nOldLevel = GetPriority();
	if (THREAD_PRIORITY_TIME_CRITICAL == nOldLevel)
		nNewLevel = THREAD_PRIORITY_HIGHEST;
	else if (THREAD_PRIORITY_HIGHEST == nOldLevel)
		nNewLevel = THREAD_PRIORITY_ABOVE_NORMAL;
	else if (THREAD_PRIORITY_ABOVE_NORMAL == nOldLevel)
		nNewLevel = THREAD_PRIORITY_NORMAL;
	else if (THREAD_PRIORITY_NORMAL == nOldLevel)
		nNewLevel = THREAD_PRIORITY_BELOW_NORMAL;
	else if (THREAD_PRIORITY_BELOW_NORMAL == nOldLevel)
		nNewLevel = THREAD_PRIORITY_LOWEST;
	else if (THREAD_PRIORITY_LOWEST == nOldLevel)
		nNewLevel = THREAD_PRIORITY_IDLE;
	else if (THREAD_PRIORITY_IDLE == nOldLevel)
		return;

	SetPriority(nNewLevel);
}

template<typename T> 
void TXThread<T>::OnRunning()
{
	(m_thObject.*m_pfnOnRunning)();
}

template<typename T> 
unsigned __stdcall TXThread<T>::_ThreadProc(LPVOID lpParameter)
{
	TXThread<T>* pThread = reinterpret_cast<TXThread<T>*>(lpParameter);

	pThread->SetPriority(pThread->m_nInitPriority);
	pThread->m_bIsRunning = true;
	::SetEvent(pThread->m_hEvent);
	pThread->OnRunning();
	return 0;
}
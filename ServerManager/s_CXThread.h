#ifndef S_XTHREAD_H_
#define S_XTHREAD_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <process.h>
#include <assert.h>
#include "s_CClientLock.h"

namespace XThread
{
	class CXThread : public ::CClientLock
	{
	public:
		// The constructor --
		CXThread(int nPriority = THREAD_PRIORITY_NORMAL); 	
		// Destructor --
		virtual ~CXThread()
		{
			::CloseHandle(m_hEvent);
		}	

	protected:		
		HANDLE m_hThread;
		HANDLE m_hEvent; // Thread and Event handle --
		int m_nInitPriority;
		unsigned int m_dwThreadID; // Contain the thread ID --
		// Variable to know the state of the thread terminated or suspend or Running -- 
		bool m_bTerminate;
		bool m_bSuspend;
		bool m_bIsRunning;

	protected:
		// put the initialization code here.
		virtual void OnInitInstance(){}
		// put the main code of the thread here.
		virtual void OnRunning() = 0; // Must be overloaded --
		// put the cleanup code here.
		virtual DWORD OnExitInstance(){return 0;}
		// Exit the thread safety.
		void Exit(); 
		// Thread function --
		static unsigned __stdcall _ThreadProc(LPVOID lpParameter);
		
	
	public:
		// Wait until the thread terminate, after this function you are sure that the thread is terminated.
		bool WaitUntilTerminate(DWORD dwMiliSec = INFINITE);
		// Start the thread or recreate it, if it has been terminated before --
		bool Start(); 
		// Start the thread and return when it actualy start --
		bool StartAndWait();
		// Pause the thread --
		bool Pause();
		// Check if the thread is running or not.
		bool IsRunning();
		// check if the thread has been terminated or not.
		bool IsTerminated();
		// Check for the thread is suspend or not.
		bool IsSuspend();
		// Set thread priority
		void SetPriority(int nLevel);
		// Get thread priority
		int GetPriority();
		// Speed up thread execution - increase priority level
		void SpeedUp(); 
		// Slow down Thread execution - decrease priority level
		void SlowDown();
		// Terminate immediate the thread Unsafe.
		void Terminate();	
	};  	
	// End of CXThread Class Definition --
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////
	// Begin of TXThread declaration --
	template<typename T>
	class TXThread : public ::CClientLock // Template X Thread
	{
	public:
		// The constructor --
		TXThread(T& thObject, void (T::*pfnOnRunning)(), int nPriority = THREAD_PRIORITY_NORMAL); 
		// Destructor
		virtual ~TXThread() 
		{ 
			::CloseHandle(m_hEvent); 
		}

	protected:
		T& m_thObject; // The thread object --
		void (T::*m_pfnOnRunning) ();
		HANDLE m_hThread;
		HANDLE m_hEvent; // Thread and Event handle --
		int m_nInitPriority;
		unsigned int m_dwThreadID; // Contain the thread ID --
		// Variable to know the state of the thread terminated or suspend or Running -- 
		bool m_bTerminate;
		bool m_bSuspend;
		bool m_bIsRunning;
	
	protected:
		// Thread function --
		static unsigned __stdcall _ThreadProc(LPVOID lpParameter);
		// Exit the thread safety.
		void Exit(); 
		// Call the running member function --
		inline void OnRunning();

	public:
		// Wait until the thread terminate, after this function you are sure that the thread is terminated.
		bool WaitUntilTerminate(DWORD dwMiliSec = INFINITE);
		// Start the thread or recreate it, if it has been terminated before --
		bool Start(); 
		// Start the thread and return when it actualy start --
		bool StartAndWait();
		// Pause the thread --
		bool Pause();
		// Check if the thread is running or not.
		bool IsRunning();
		// check if the thread has been terminated or not.
		bool IsTerminated();
		// Check for the thread is suspend or not.
		bool IsSuspend();
		// Set thread priority
		void SetPriority(int nLevel);
		// Get thread priority
		int GetPriority();
		// Speed up thread execution - increase priority level
		void SpeedUp(); 
		// Slow down Thread execution - decrease priority level
		void SlowDown();
		// Terminate immediate the thread Unsafe.
		void Terminate();	
	};
    // End of TXThread Class Declaration --
	/////////////////////////////////////////////////////////////////////////////
};

#endif  // S_XTHREAD_H_
// End of Thread.hpp --


// example code
// TemplateThread.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
//#include <iostream>
//#include "Thread.hpp"
//
//using namespace std;
//using namespace Thread;
//
//class ThreadTest1
//{
//public:
//  void InsideThread()
//  {
//    for(int i=0;i<5;i++)
//    {
//      cout << "ThreadTest1: " << i << endl;
//    }
//  }
//};
//
//class ThreadTest2:public CThread
//{
//public:
//  void OnRunning()
//  {
//    for(int i=0;i<5;i++)
//    {
//      cout << "ThreadTest2: " << i << endl;
//    }
//  }
//};
//
//int _tmain(int argc, _TCHAR* argv[])
//{
//  int n;
//  ThreadTest1 test1;
//  ThreadTest2 test2;
//
//  TThread<ThreadTest1> thTest1(test1,&ThreadTest1::InsideThread);
//  thTest1.StartAndWait();
//  thTest1.WaitUntilTerminate();
//
//  test2.StartAndWait();
//  test2.WaitUntilTerminate();
//  cin >> n;
//	return 0;
//}
//

